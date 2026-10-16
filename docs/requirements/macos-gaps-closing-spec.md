# Plan de Cierre de Brechas QuartzDE: Fases 6 a 13

Documento para el flujo multi-agente (`phase-planner`, `planner`, `coder`, `critic-ux`). Complementa `docs/requirements/macos-1to1-spec.md`. Cada fase se divide en tareas atómicas con: archivos, cómo implementarlo, trampas conocidas y criterios de aceptación.

---

## 0. Reglas generales (leer antes de cualquier tarea)

### 0.1 Hallazgos de auditoría que condicionan el trabajo

Verificados leyendo el repositorio. El agente debe confirmarlos con `grep` antes de actuar.

| # | Hallazgo | Consecuencia |
|---|----------|--------------|
| H1 | `DesktopShortcuts` (`libquartz/src/shortcuts.rs`) solo **declara** atajos. Los únicos `QShortcut` reales están en `quartz-finder`. | `Super+Up`, `Ctrl+Space`, `Ctrl+Q`, `Ctrl+M`... no hacen nada a nivel de sesión hasta que alguien los registre globalmente. Es prerrequisito de las Fases 8, 9 y 10. |
| H2 | `QuartzDesktopConfig` y sus structs no usan `#[serde(default)]`, y `load_from_xdg()` ante un JSON que no parsea **devuelve `Default` y lo sobrescribe en disco**. | Añadir un campo nuevo destruye la configuración existente del usuario. Regla obligatoria en 0.2. |
| H3 | `QuartzDesktopConfig` no tiene campo de fondo de pantalla y `WallpaperWindow.qml` tiene `Image.source: ""`. | El fondo nunca cambia. Hay que añadir `wallpaper` a la config y cablearlo. |
| H4 | `quartz-session` lanza KWin con `--no-lockscreen`. | No existe bloqueo de pantalla. `lock_screen` del contrato D-Bus no tiene UI detrás. |
| H5 | `StageManagerEffect::toggleMissionControl()` solo cambia `m_spatialMode`. | Mission Control no dibuja nada todavía. |
| H6 | `src/finder` rellena `QListWidget`/`QTreeWidget` de forma síncrona con `QDir::entryInfoList`. | Bloquea la UI en carpetas grandes y no refleja cambios externos. Hay que corregirlo antes de añadir operaciones de archivos (tarea 6.0). |
| H7 | La Fase 5 usa `KF6::WindowSystem`/TaskManager. En Wayland, `org_kde_plasma_window_management` es una interfaz **privilegiada**: KWin solo la ofrece a clientes cuyo `.desktop` declare `X-KDE-Wayland-Interfaces`. | Si `quartz-shell` no tiene ese `.desktop` registrado y no se lanza a través de él, no recibirá ventanas y el Dock no mostrará apps en ejecución, aunque compile y los tests (con mocks) pasen. Verificar en la VM en 9.0. |
| H8 | La spec antigua habla de `KDecoration2`; el código y `status.md` usan `KDecoration3`. | Manda el código. No "corregir" hacia KDecoration2. |
| H9 | `scope-gate.py` tiene `workspace_root` fijo en `/home/pedro/Proyectos/WhiteQuartz` y una lista `ALLOWED` cerrada. | Cada fase que cree un directorio nuevo en la raíz debe añadirlo a `ALLOWED` en su primera tarea. |

### 0.2 Reglas de ingeniería (no negociables)

1. **No inventar APIs.** KWin, KDecoration3, LayerShellQt y KF6 cambian de firma entre versiones. Antes de usar una API, abrir el header instalado en la VM (`/usr/include/kwin/...`, `/usr/include/KF6/...`) y copiar la firma real. Si no existe, parar y documentarlo en `status.md` en vez de improvisar.
2. **Compatibilidad de configuración.** Todo struct serializable nuevo o ampliado lleva `#[serde(default)]` (a nivel de struct y con `impl Default`). Añadir además un test que deserialice un JSON antiguo (el actual) y compruebe que no se pierde nada. Cambiar `load_from_xdg` para que, si el JSON no parsea, **renombre** el fichero a `config.json.bad` en vez de sobrescribirlo.
3. **Contratos D-Bus en un solo sitio.** Toda interfaz propia se define en `crates/libquartz/src/dbus/` (proxy y servidor con los mismos métodos). Añadir un test de paridad como el de `org.quartz.Session`. Los C++ consumen la misma interfaz por nombre.
4. **Nunca bloquear el hilo de GUI** con E/S de disco, red o D-Bus. Usar `QtConcurrent`/`QThread` o D-Bus asíncrono (`QDBusPendingCallWatcher`) en C++, y `tokio`/`zbus` async en Rust.
5. **Degradación elegante.** Si no hay batería, Wi-Fi, Bluetooth, backlight o PipeWire (caso típico de una VM), el control correspondiente se oculta o se deshabilita. Nunca excepción, nunca valor inventado.
6. **Un plugin de KWin nunca debe tirar la sesión.** `StageManagerEffect` y `QuartzDecoration` deben: comprobar `isSupported()`, validar que el shader compiló (`isValid()`), no desreferenciar punteros nulos de `EffectWindow` y desactivarse solos si algo falla. Una excepción o un `nullptr` ahí mata el compositor.
7. **Entrada no confiable.** Cuerpos de notificaciones, nombres de archivo, `.desktop` y metadatos de archivos son datos externos. No usarlos como `RichText` sin sanear, no pasarlos a una shell (`QProcess` siempre con programa + lista de argumentos), no seguir symlinks al borrar o copiar recursivamente.
8. **Autenticación y privilegios.** No escribir código propio que lea contraseñas ni ejecutar nada como root. Bloqueo y login se delegan a `kscreenlocker` y SDDM (PAM). Las acciones privilegiadas (fecha/hora, usuarios, actualizaciones) van por D-Bus + polkit.
9. **Cadenas de interfaz en español**, con `tr()`/`qsTr()` desde el principio para no repetir trabajo si se añade otro idioma.
10. **Cada tarea cierra con**: tests nuevos, `cargo test --release --workspace` y `ctest` en verde, `./scripts/build_and_install.sh --build-only` con código 0, y casilla marcada en `roadmap.md` + bloque en `status.md` (esto desbloquea `require-review.py`).

### 0.3 Orden y dependencias

```
Fase 6  Finder  ─────────────────────────────┐
Fase 7  Notificaciones, Control Center, Ajustes
Fase 8  Atajos globales + Bloqueo + Login + Energía   (prerrequisito de 9 y 10)
Fase 9  Dock/Menubar avanzados, bandeja, escritorio, capturas
Fase 10 Launchpad, Mission Control, Spaces
Fase 11 Calidad: blur, Spotlight, QuickLook, modo oscuro
Fase 12 Apps nativas o tematizadas
Fase 13 Servicios (diferibles)
```

La Fase 8.1 (atajos globales) puede adelantarse incluso antes de la 7.

---

## Fase 6: Finder funcional

### 6.0 Preparación: modelo asíncrono (nuevo, antes de las demás)

- **Archivos**: `src/finder/findercolumnview.h/.cpp`, `src/finder/main.cpp`, `tests/tst_finder_modes.cpp`.
- **Qué hacer**: sustituir el poblado manual por `QFileSystemModel` (carga en hilo propio y observa cambios del disco) con `QSortFilterProxyModel` para ordenación y filtro. Las 4 vistas leen del mismo modelo: iconos (`QListView` en modo `IconMode`), lista (`QTreeView`), columnas (`QColumnView` o varias `QListView` encadenadas), galería (lista horizontal + vista previa).
- **Cómo hacerlo bien**:
  - `setRootPath()` devuelve inmediatamente. Esperar a `directoryLoaded` antes de seleccionar o hacer scroll.
  - Opciones del modelo: `setReadOnly(false)`, `setResolveSymlinks(false)`, filtro con `QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden` activable por `Ctrl+Shift+.`.
  - No usar `QPixmap(path).scaled()` en el hilo de GUI para la galería: usar `QImageReader` con `setScaledSize()` (no carga la imagen completa) en `QtConcurrent`, con caché `QPixmapCache`.
- **Aceptación**: los 4 modos siguen pasando `tst_finder_modes`; abrir una carpeta con 20 000 archivos no congela la ventana; crear un archivo desde fuera (`touch`) aparece sin recargar.

### 6.1 Barra de ruta y espacio libre

- **Archivos**: `src/finder/main.cpp`, `src/finder/findercolumnview.*`.
- **Cómo**: `QStatusBar` fija de 22 px. A la izquierda, migas de pan como botones planos (`QToolButton`, `autoRaise`), cada uno navega a su tramo; el último elide con `QFontMetrics::elidedText`. A la derecha: `"N elementos, X GB disponibles"` con `QStorageInfo(path).bytesAvailable()` (refrescar al cambiar de carpeta y con un `QTimer` de 30 s). Si hay selección, mostrar `"N de M seleccionados"`.
- **Aceptación**: test con `QTemporaryDir` comprobando el texto de las migas y que el botón del tramo padre navega.

### 6.2 Operaciones de archivos

Implementar en una clase aparte `FileOperations` (`src/finder/fileoperations.h/.cpp`), sin dependencias de widgets, para poder testearla.

| Acción | Atajo (mapeo PC) | Detalle correcto |
|--------|------------------|------------------|
| Nueva carpeta | `Ctrl+Shift+N` | Nombre `"carpeta sin título"`, si existe `"carpeta sin título 2"`... Crear con `QDir::mkdir`, luego editar el nombre inline (`edit(index)`). |
| Copiar / Cortar | `Ctrl+C` / `Ctrl+X` | `QMimeData` con `text/uri-list` y, para cortar, además `application/x-kde-cutselection` = `"1"` (convención KDE) para que otras apps lo entiendan. También poner el texto de las rutas en `text/plain`. |
| Pegar | `Ctrl+V` | Leer el clipboard; si es corte, mover (`QFile::rename`, y si falla por distinto dispositivo, copiar y borrar el origen **solo si la copia terminó sin error**). |
| Mover aquí | `Ctrl+Alt+V` | Equivalente a "Mover elemento aquí" de macOS. |
| Duplicar | `Ctrl+D` | `"nombre copia.ext"`, `"nombre copia 2.ext"`. La extensión va al final (`QFileInfo::completeBaseName()` + `suffix()`). Para `.tar.gz` usar `QMimeDatabase::suffixForFileName()`. |
| Renombrar | `Return` / `F2` | Selección del nombre sin extensión al entrar en edición. Validar: no vacío, sin `/`, no existente (si existe, avisar y revertir), no `.` ni `..`. |
| A la papelera | `Delete` / `Ctrl+Backspace` | `QFile::moveToTrash(path, &out)`. En macOS el sustituto de "Borrar inmediatamente" es `Ctrl+Alt+Backspace`, con diálogo de confirmación. |
| Poner de nuevo | menú contextual en la Papelera | Leer `~/.local/share/Trash/info/<n>.trashinfo` (`Path=`, URL-encoded) y mover de vuelta. |
| Deshacer | `Ctrl+Z` | `QUndoStack` con comandos para renombrar, mover, crear carpeta y mandar a papelera (restaurar). |

Cómo hacerlo bien:
- **Copia recursiva asíncrona** en `QThread`/`QtConcurrent` con señales `progress(bytesDone, bytesTotal)`, `conflict(...)` y `finished(ok, error)`. Recorrer con `QDirIterator` **sin seguir symlinks** (`QDirIterator::NoIteratorFlags` y comprobar `QFileInfo::isSymLink()`; copiar el enlace como enlace con `QFile::symLinkTarget` + `QFile::link`).
- Impedir copiar/mover una carpeta dentro de sí misma (`QDir::cleanPath` + `startsWith(origen + '/')`).
- Conservar permisos (`QFile::setPermissions`) y fecha de modificación (`QFileDevice::setFileTime`). **Copiar también el xattr `user.xdg.tags`** o las etiquetas se perderán al copiar.
- Conflictos: diálogo con *Detener / Mantener ambos / Reemplazar* y casilla "Aplicar a todos". Para *Reemplazar*, copiar a un nombre temporal en el destino y renombrar al terminar (atómico), nunca borrar el destino antes.
- Barra de progreso en un `Sheet` (hoja modal) con botón de cancelar; cancelar deja el estado coherente (borra el temporal).
- Errores (permiso denegado, disco lleno) se acumulan y se muestran al final, sin abortar el resto.
- Menú contextual (clic derecho): Abrir, Abrir con, Obtener información (`Ctrl+I`), Renombrar, Duplicar, Mover a la Papelera, Copiar, Etiquetas, Vista rápida. "Abrir con" usa `KApplicationTrader::queryByMimeType` (`KF6::Service`).
- Arrastrar y soltar: mover si es el mismo volumen, copiar si es otro (`QStorageInfo::device()`); con `Alt` fuerza copia.
- **Tests con `QTemporaryDir`**: antes de probar `moveToTrash`, fijar `XDG_DATA_HOME` a un directorio temporal, para no tocar la papelera real del usuario.

### 6.3 Pestañas, etiquetas y volúmenes

- **Pestañas**: `QTabWidget` con `setTabsClosable(true)` y `tabBar()->setAutoHide(true)`. Cada pestaña es su propio `FinderColumnView` con su propio historial. `Ctrl+T` nueva, `Ctrl+W` cierra la pestaña activa y, si es la última, la ventana (respetando el ciclo de vida de `quartz-session`: la app sigue residente hasta `Ctrl+Q`), `Ctrl+Tab`/`Ctrl+Shift+Tab` para cambiar.
- **Etiquetas de color**: atributo extendido `user.xdg.tags` (lista separada por comas, UTF-8) con `getxattr`/`setxattr` de `<sys/xattr.h>`.
  - Leer-modificar-escribir: conservar las etiquetas que no sean de color (las crean otras apps).
  - Definir una tabla fija nombre→color (Rojo, Naranja, Amarillo, Verde, Azul, Morado, Gris).
  - Manejar `ENOTSUP`/`EPERM` (el sistema de archivos puede no soportar xattr de usuario): mostrar el menú deshabilitado, no fallar.
  - Pintar un círculo de 8 px en vista Lista y Columnas con un `QStyledItemDelegate`. Sección "Etiquetas" en el sidebar que filtra por etiqueta (recorrer con `QtConcurrent`, no bloquear).
- **Volúmenes**: sección "Ubicaciones" con `QStorageInfo::mountedVolumes()` filtrando `!isValid() || !isReady()`, tipos de sistema de archivos pseudo (`proc`, `sysfs`, `tmpfs`, `devtmpfs`, `cgroup*`, `overlay`, `squashfs`) y puntos de montaje que no sean `/` ni cuelguen de `/media`, `/run/media/$USER` o `/mnt`.
  - Para **montar/expulsar** de verdad, usar UDisks2 por D-Bus (`org.freedesktop.UDisks2`, `Filesystem.Mount/Unmount`, `Drive.Eject`) y escuchar `InterfacesAdded/Removed` para refrescar en vivo. Botón de expulsar (▲) junto al volumen.
  - `QStorageInfo` solo se actualiza llamando a `refresh()`; no asumir que se actualiza solo.
- **Aceptación**: tests de `FileOperations` (nombres de duplicado, conflicto, copia recursiva con symlink, cortar/pegar entre carpetas, etiquetas con ida y vuelta de xattr si el FS lo soporta, `QSKIP` si no) y de pestañas (historial independiente).

---

## Fase 7: Notificaciones, Control Center y Ajustes

### 7.1 Servidor de notificaciones

- **Archivos**: `src/shell/notifications/NotificationServer.h/.cpp`, `NotificationModel.h/.cpp`, `src/shell/qml/NotificationBanner.qml`, `NotificationCenter.qml`.
- **Cómo**:
  - Registrar `org.freedesktop.Notifications` en `/org/freedesktop/Notifications`.
  - Métodos spec 1.2: `GetCapabilities`, `Notify`, `CloseNotification`, `GetServerInformation`.
  - Señales: `NotificationClosed` y `ActionInvoked`.
  - Saneado de HTML y capa LayerOverlay dimensionada exclusivamente al banner.
  - Panel lateral `NotificationCenter.qml`.

### 7.2 Control Center y OSD

- **Archivos**: `src/shell/controlcenter/SystemHardwareController.h/.cpp`, `ControlCenterPopup.qml`, `OsdWindow.qml`, `MenuBarWindow.qml`.
- Backends: PulseAudio/PipeWire, login1 (brillo), NetworkManager, Bluez, UPower, MPRIS2.

### 7.3 Los 18 paneles de Ajustes del Sistema

- **Archivos**: `src/settings/qml/SettingsMainWindow.qml`, `src/settings/resources.qrc`, `src/settings/qml/panes/*.qml`, `settingsconfigcontroller.*`.

---

## Fase 8: Atajos globales, bloqueo, login y energía

- **8.1 Atajos globales**: registro vía `KF6::GlobalAccel` y atajos de compositor.
- **8.2 Bloqueo de pantalla**: paquete look-and-feel para `kscreenlocker` en `data/lookandfeel/org.quartz.desktop/`.
- **8.3 Login (SDDM)**: tema QML en `data/sddm/themes/quartz/`.
- **8.4 Menú Apple y energía**: `login1` para reposo, reinicio y apagado.

---

## Fase 9: Dock, barra de menús y escritorio avanzados

- **9.0 Verificación H7**: KWin Wayland interfaces para el Dock.
- **9.1 Dock**: menú contextual, persistencia de `.desktop`, arrastre y minimización.
- **9.2 Cambiador de aplicaciones (`Alt+Tab`)**: overlay exclusivo MRU.
- **9.3 Esquinas activas**.
- **9.4 Bandeja del sistema (`StatusNotifierItem`)**.
- **9.5 Iconos de escritorio**.
- **9.6 Capturas de pantalla**: portales XDG o `ScreenShot2`.
- **9.7 Forzar salida y Acerca de este Mac**.
- **9.8 Barra de menús**.

---

## Fase 10: Launchpad, Mission Control y Spaces

- **10.1 Launchpad**: overlay con rejilla paginada y `ListApps()` de Spotlight.
- **10.2 Mission Control y App Exposé**: distribución de ventanas en `StageManagerEffect` y Spaces con `VirtualDesktopManager`.

---

## Fase 11: Calidad y fidelidad visual

- **11.1 Blur (vibrancy)**: `KWindowEffects::enableBlurBehind`.
- **11.2 Spotlight**: `evalexpr`, fuzzy matching e índice con vigilancia.
- **11.3 Quick Look**: PDF con `QtPdf`, multimedia con `QtMultimedia`.
- **11.4 Modo oscuro auto y acento**.
- **11.5 Robustez del compositor**: guardas `nullptr` y modo seguro.

---

## Fase 12: Aplicaciones del sistema

- Tematización e integración de Terminal, Visor PDF/Imágenes, Editor de texto y Monitor de actividad.

---

## Fase 13: Servicios

- Time Machine (interfaz sobre restic/btrfs), llavero y soporte de impresoras.
