# Especificación Técnica de Paridad 1:1 macOS (Sonoma/Sequoia) y Hoja de Ruta para Agentes

Este documento está estructurado para ser ingerido directamente por el flujo multi-agente del repositorio (`.agents/skills/phase-planner`, `planner`, `coder` y `critic-ux`).

---

## 0. Corrección Previa de Infraestructura de Agentes (`scope-gate.py`)

* **Bloqueo potencial en Hooks**: El archivo `.agents/scripts/scope-gate.py` define la lista `ALLOWED` sin incluir el directorio `themes/`, el cual ha sido incorporado recientemente al árbol del repositorio (`themes/Colloid-icon-theme/` y `themes/Orchis-kde/`).
* **Acción requerida**: Añadir `"themes/"` a la lista `ALLOWED` en `.agents/scripts/scope-gate.py` si algún agente necesita parchear archivos de temas o metadatos Aurorae/Kvantum.

---

## 1. Auditoría de Brechas Técnicas (Código Actual vs. Objetivo 1:1)

| Módulo / Ruta | Estado Actual en el Código | Brecha Técnica Real para Paridad 1:1 |
| --- | --- | --- |
| **`crates/libquartz/src/dbus/session.rs` vs `crates/quartz-session/src/main.rs`** | `SessionProxy` define `lock_screen`, `request_logout`, `request_reboot`, `request_shutdown`, `is_locked`. `SessionLifecycleManager` expone `on_window_opened`, `on_window_closed`, `quit_application`, `is_app_running`. | **Incompatibilidad de contrato D-Bus**: El proxy cliente y el servidor implementan métodos disjuntos bajo la misma interfaz `org.quartz.Session`. |
| **`src/shell/`** | Basado en ventanas QML (`ShellWindow.qml`, `MenuBarWindow.qml`, `DockWindow.qml`, `SpotlightOverlay.qml`, `QuickLookModal.qml`) usando flags estándar `Qt.FramelessWindowHint \| Qt.WindowStaysOnBottomHint`. | **Ausencia de protocolo Wayland `wlr-layer-shell`**: Sin `LayerShellQt`, las barras no reservan `exclusiveZone`, son cubiertas por ventanas maximizadas y no se superponen a aplicaciones a pantalla completa. |
| **`src/shell/dock/DockController.cpp`** | Lanza aplicaciones mediante `QProcess::startDetached(cmd, {})` y devuelve un `QVariantList` estático en `defaultApps()`. | **Fallo en comandos con argumentos y falta de TaskManager**: Comandos como `"quartz-finder ~/Descargas"` fallan al no separar binario de argumentos. No detecta ventanas abiertas reales de Wayland ni agrupa ventanas minimizadas. |
| **`src/shell/menubar/`** | Reloj funcional y menú Apple básico lanzado con `systemctl`. Textos de menú (`Archivo`, `Edición`, etc.) y `activeAppTitle` (`"Finder"`) cableados estáticamente. | **Ausencia de Menú Global (`DBusMenu`)**: No implementa `com.canonical.AppMenu.Registrar` ni importa los `QMenuBar` exportados por las aplicaciones activas. |
| **`src/compositor/`** | `QuartzDecoration` y `StageManagerEffect` heredan de `QObject`. El shader `squircle.frag` existe en disco pero ningún archivo C++ lo carga ni compila. | **Inexistente integración con KWin**: KWin exige heredar de `KDecoration2::Decoration` y `KWin::Effect` / `KWin::OffscreenEffect`, además de metadatos JSON de plugin (`K_PLUGIN_CLASS_WITH_JSON`). |
| **`crates/quartz-spotlight/`** | `SpotlightEngine::query` solo evalúa una suma/resta/multiplicación/división simple (`split_once`) y filtra una matriz fija de 4 tuplas (`known_apps`). | **Sin indexación real**: No lee ficheros `.desktop` de XDG ni indexa el sistema de archivos del usuario. |
| **`crates/quartz-quicklookd/`** | `PreviewInspector::inspect` solo lee extensión, tamaño en bytes y número de líneas de texto. | **Sin renderizado multimedia**: No sirve buffers de imagen, páginas PDF, ni streams de audio/vídeo a `QuickLookModal.qml`. |
| **`src/finder/`** | 4 vistas en `QStackedWidget` poblando `QListWidget` y `QTreeWidget` síncronamente con `QDir::entryInfoList`. | **Visor de solo lectura incompleto**: `setupGalleryView()` nunca recibe datos en `setRootPath()`. El sidebar y botones `<`/`>` no están conectados. Sin copiar, pegar, borrar a papelera, renombrar, doble clic ni montaje USB. |
| **`src/settings/`** | `SettingsNavigationModel` define 18 paneles (`qrc:/qml/panes/*.qml`), pero los archivos QML no existen y `SettingsMainWindow.qml` solo muestra 2 `Switch` estáticos. | **Carencia de los 18 paneles QML y backends de hardware**: Sin conexión real a `NetworkManagerQt`, `BluezQt`, `PulseAudioQt`, `KScreen` ni `libinput`. |
| **`themes/` y `scripts/build_and_install.sh`** | Los repositorios `Colloid-icon-theme` y `Orchis-kde` están presentes en `themes/`, pero `build_and_install.sh` no los instala ni `startquartz` los activa. | **Temas inactivos en la VM**: Los iconos, cursores y esquemas de color no se copian a `${INSTALL_PREFIX}/share/` ni se exportan en las variables de entorno de la sesión. |

---

## Fase 3: Protocolos Wayland (`LayerShellQt`), Temas, Wallpaper e Indexación Real

**Objetivo**: Dotar a `quartz-shell` de superficies Wayland reales, desplegar automáticamente los temas visuales en la VM y convertir Spotlight y QuickLook en servicios plenamente funcionales con datos del sistema.

### Tareas Atómicas para `planner` / `coder`:

1. **Unificación del Contrato D-Bus de Sesión (`crates/libquartz` y `crates/quartz-session`)**:
   * **Archivos**: `crates/libquartz/src/dbus/session.rs`, `crates/quartz-session/src/main.rs`.
   * **Implementación**: Añadir a `SessionLifecycleManager` (`#[interface(name = "org.quartz.Session")]`) los métodos `lock_screen`, `request_logout`, `request_reboot`, `request_shutdown` e `is_locked` definidos en `SessionProxy`, e incluir en `SessionProxy` los métodos `on_window_opened`, `on_window_closed`, `quit_application` e `is_app_running`.
   * **Lógica de energía**: `request_reboot` y `request_shutdown` deben invocar `org.freedesktop.login1.Manager` vía D-Bus (`Reboot(true)`, `PowerOff(true)`) o finalizar limpiamente `SessionManager::terminate_all()` en `request_logout`.

2. **Integración de `LayerShellQt` y Superficie de Wallpaper en `quartz-shell`**:
   * **Archivos**: `CMakeLists.txt`, `src/shell/CMakeLists.txt`, `src/shell/main.cpp`, `src/shell/qml/MenuBarWindow.qml`, `src/shell/qml/DockWindow.qml`, `src/shell/qml/SpotlightOverlay.qml`, `src/shell/qml/QuickLookModal.qml`, `src/shell/qml/WallpaperWindow.qml` (nuevo).
   * **Dependencias CMake**: Añadir `find_package(LayerShellQt REQUIRED)` y enlazar `LayerShellQt::Interface` en `quartz-shell`.
   * **Configuración de superficies C++**:
     * `WallpaperWindow`: `LayerShellQt::Window::LayerBackground`, anclado a los 4 bordes (`AnchorTop | AnchorBottom | AnchorLeft | AnchorRight`), `exclusiveZone = -1`. Renderiza el wallpaper configurado en `QuartzDesktopConfig` escalado con `Image.PreserveAspectCrop`.
     * `MenuBarWindow`: `LayerShellQt::Window::LayerTop`, `AnchorTop | AnchorLeft | AnchorRight`, altura fija `24`, `exclusiveZone = 24`.
     * `DockWindow`: `LayerShellQt::Window::LayerTop`, `AnchorBottom`, margen inferior `6`, `exclusiveZone = 0` (si `autohide == true`) o `54` (si `autohide == false`).
     * `SpotlightOverlay` y `QuickLookModal`: `LayerShellQt::Window::LayerOverlay`, `KeyboardInteractivityOnDemand` / `KeyboardInteractivityExclusive` al mostrarse.

3. **Física Gaussiana Continua y Corrección de `launchApp` en el Dock**:
   * **Archivos**: `src/shell/dock/DockController.cpp`, `src/shell/dock/DockController.h`, `src/shell/qml/DockWindow.qml`.
   * **Corrección de proceso**: En `DockController::launchApp(const QString &cmd)`, reemplazar `QProcess::startDetached(cmd, {})` por `QProcess::splitCommand(cmd)` para invocar `program` y `parts`.
   * **Física QML**: En `DockWindow.qml`, añadir un `HoverHandler` sobre el contenedor principal que exponga `point.position.x`. Cada delegado del `Repeater` debe calcular la distancia entre su centro horizontal y `point.position.x`, invocando `dockCtrl.calculateGaussianMagnification(distance, 65.0)` para determinar su `width`/`height` en tiempo real, y `dockCtrl.calculateParabolicBounce(progress, 22.0)` mediante un `NumberAnimation` de `progress` (0.0 a 1.0, duración `650ms`) al recibir la señal `appBouncingRequested`.

4. **Indexador Real XDG (`.desktop`), Archivos y Calculadora en `quartz-spotlight`**:
   * **Archivos**: `crates/quartz-spotlight/src/main.rs`, `crates/quartz-spotlight/Cargo.toml`.
   * **Implementación**:
     * Escanear al inicio (y cachear en memoria) `/usr/share/applications`, `/usr/local/share/applications` y `$HOME/.local/share/applications`. Parsear `Name`, `GenericName`, `Comment`, `Exec` (limpiando códigos de campo `%U`, `%u`, `%F`, `%f`) e `Icon`, omitiendo entradas con `NoDisplay=true`.
     * Añadir búsqueda asíncrona de archivos en `$HOME` (hasta profundidad 3, ignorando directorios ocultos `.*` y `target`/`node_modules`/`build`) devolviendo `SearchResultCategory::File`.
     * Ampliar `SpotlightServer::search` en D-Bus para devolver `Vec<(String, String, String, String, String)>`: `(title, subtitle, category, icon_name, action_payload)`.

5. **Previsualización Multimedia e Inspección Real en `quartz-quicklookd` y `QuickLookModal.qml`**:
   * **Archivos**: `crates/quartz-quicklookd/src/main.rs`, `src/shell/quicklook/QuickLookController.h/.cpp`, `src/shell/qml/QuickLookModal.qml`.
   * **Implementación**:
     * Modificar `PreviewInspector::inspect` para leer los primeros `64 KB` de texto si es `PreviewType::Text` y exponer un método D-Bus `GetCurrentPreview() -> (String, String, String, u64, String, String)` (`uri, file_name, preview_type, size_bytes, summary, text_content`) además de emitir una señal D-Bus `PreviewChanged`.
     * En `QuickLookModal.qml`, usar un `StackLayout` según `previewType`:
       * `"Image"`: Componente `Image` nativo cargando `file://` con `fillMode: Image.PreserveAspectFit`.
       * `"Text"`: `ScrollView` + `TextArea` monoespaciado de solo lectura con el contenido del fichero.
       * `"Audio"` / `"Video"`: `MediaPlayer` + `VideoOutput` (de `QtMultimedia`).
       * `"Generic"`: Vista de icono grande + metadatos (`summary`, `size_bytes`).

6. **Despliegue de Temas (`Colloid` y `Orchis`) en `build_and_install.sh` y `startquartz`**:
   * **Archivos**: `scripts/build_and_install.sh`, `crates/quartz-session/src/main.rs`.
   * **Script de instalación**: En la sección `--vm-install` de `scripts/build_and_install.sh`, copiar `themes/Colloid-icon-theme/cursors/dist` a `${INSTALL_PREFIX}/share/icons/Colloid-cursors`, ejecutar `themes/Colloid-icon-theme/install.sh -d ${INSTALL_PREFIX}/share/icons` y copiar `themes/Orchis-kde/color-schemes/*` a `${INSTALL_PREFIX}/share/color-schemes/`.
   * **Variables de entorno**: En `SessionManager::setup_environment()` (`crates/quartz-session/src/main.rs`), añadir:
     ```rust
     env::set_var("XCURSOR_THEME", "Colloid-cursors");
     env::set_var("XCURSOR_SIZE", "24");
     env::set_var("QT_QPA_PLATFORMTHEME", "kde");
     ```

---

## Fase 4: Integración Real en el Compositor KWin (`KDecoration2`, `KWin::Effect` y Shaders GLSL)

**Objetivo**: Reemplazar las clases simuladas en `src/compositor/` por plugins nativos cargables por `kwin_wayland` para renderizar el semáforo Aqua, sombras dinámicas, recorte superelíptico $G^2$ ($n=4.5$) y transformaciones 3D de Stage Manager.

### Tareas Atómicas para `planner` / `coder`:

1. **Plugin de Decoración de Ventanas (`KDecoration2::Decoration`)**:
   * **Archivos**: `src/compositor/CMakeLists.txt`, `src/compositor/quartzdecoration.h`, `src/compositor/quartzdecoration.cpp`, `src/compositor/decoration.json` (nuevo).
   * **Dependencias CMake**: `find_package(KDecoration2 REQUIRED)`. Compilar un plugin compartido `quartzdecoration` instalado en `${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2`.
   * **Implementación**:
     * Hacer que `QuartzDecoration` herede de `KDecoration2::Decoration` (manteniendo un helper o lógica interna para los tests unitarios existentes en `tst_stagemanager_math`).
     * Sobrescribir `init()` para crear los `KDecoration2::DecorationButton` o gestionar directamente los eventos de ratón (`hoverMoveEvent`, `mousePressEvent`, `mouseReleaseEvent`) sobre `closeButtonRect()`, `minimizeButtonRect()` y `maximizeButtonRect()`.
     * Sobrescribir `paint(QPainter *painter, const QRect &repaintRegion)` invocando `QuartzStyle::drawTrafficLightButton` para dibujar los tres botones a `14px` del margen izquierdo (`TrafficLightLeftMargin`), con espaciado de `8px` (`TrafficLightSpacing`) y diámetro `12px` (`TrafficLightButtonSize`), pintando los glifos `x`, `-`, `+` cuando el cursor esté sobre cualquiera de los tres botones del grupo.
     * Configurar `borders()` con `top = titleBarHeight()` (`28px` estándar, `52px` unificada) y `left = right = bottom = 0`.
     * Instanciar `KDecoration2::DecorationShadow` usando los parámetros de `StageManagerEffect::shadowForWindow(client()->isActive())` (`blurRadius = 32.0` activa vs `14.0` inactiva).

2. **Plugin de Efectos de Composición (`KWin::OffscreenEffect`) y Carga de `squircle.frag`**:
   * **Archivos**: `src/compositor/CMakeLists.txt`, `src/compositor/stagemanagereffect.h`, `src/compositor/stagemanagereffect.cpp`, `src/compositor/metadata.json` (nuevo), `src/compositor/shaders/squircle.frag`.
   * **Implementación**:
     * Cargar `src/compositor/shaders/squircle.frag` mediante `KWin::ShaderManager::instance()->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(), QStringLiteral(":/effects/quartz/shaders/squircle.frag"))`.
     * En `paintWindow(KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, KWin::EffectWindow *w, int mask, QRegion region, KWin::WindowPaintData &data)`:
       1. Enlazar el shader con `KWin::ShaderBinder binder(m_squircleShader.get())`.
       2. Pasar los uniforms definidos en `squircle.frag`:
          * `uWindowSize` $\leftarrow$ `QVector2D(w->width(), w->height())`
          * `uCornerRadius` $\leftarrow$ `11.0f` (`StyleMetrics::WindowCornerRadius`)
          * `uPower` $\leftarrow$ `4.5f`
          * `uBorderColor` $\leftarrow$ `QVector4D(1.0f, 1.0f, 1.0f, 0.15f)` en modo oscuro o `QVector4D(0.0f, 0.0f, 0.0f, 0.12f)` en modo claro.
       3. Si `m_enabled == true` (Stage Manager activo) y la ventana no pertenece a `m_activeGroupId`, aplicar la matriz devuelta por `calculateStageTransform(w->frameGeometry(), stageIndex)` a `data` (`data.setScale(...)`, `data.setRotationAxis(Qt::YAxis)`, `data.setRotationAngle(14.0)`, `data.translate(...)`).
     * Registrar el objeto D-Bus `/org/quartz/Compositor/StageManager` (`org.quartz.Compositor.StageManager`) dentro del constructor del efecto en KWin para que `StageManagerProxy` (`crates/libquartz/src/dbus/stagemanager.rs`) se comunique directamente con KWin.

---

## Fase 5: Gestión de Ventanas Wayland, Menú Global (`DBusMenu`) y TaskManager del Dock

**Objetivo**: Conectar la barra de menús superior y el Dock inferior al estado real de las ventanas en ejecución dentro de la sesión Wayland.

### Tareas Atómicas para `planner` / `coder`:

1. **Seguimiento de Ventanas Activas en `DockController` y `MenuBarController`**:
   * **Archivos**: `src/shell/dock/DockController.h/.cpp`, `src/shell/menubar/MenuBarController.h/.cpp`, `src/shell/CMakeLists.txt`.
   * **Implementación**:
     * Integrar `KF6::WindowSystem` o `LibTaskManager` (Plasma Workspace) / script puente de KWin que notifique vía D-Bus a `org.quartz.Shell` los eventos: `windowAdded(String appId, String title, String icon, bool active)`, `windowRemoved(String appId)`, `activeWindowChanged(String appId, String title, String dbusMenuService, String dbusMenuPath)`.
     * En `MenuBarController`, actualizar automáticamente `setActiveAppTitle(title)` cuando cambie la ventana con foco en Wayland.
     * En `DockController`, fusionar la lista de aplicaciones ancladas (`defaultApps()`) con las aplicaciones en ejecución reales:
       * Si una aplicación anclada está abierta, marcar `running = true` para encender el indicador de `4pt`.
       * Si se abre una aplicación no anclada, añadirla dinámicamente antes del separador del Dock (o en la sección de recientes si `DockConfig::show_recent_apps == true`).
       * Al hacer clic en un icono del Dock con `running == true`, si ya tiene ventanas abiertas, pedir a KWin que active sus ventanas; si no tiene ventanas pero es residente (`SessionLifecycleManager::is_app_running`), invocar la apertura de una nueva ventana.

2. **Servidor `AppMenu.Registrar` e Importador `DBusMenu` en `MenuBarController`**:
   * **Archivos**: `src/shell/menubar/MenuBarController.h/.cpp`, `src/shell/menubar/appmenuregistrar.h/.cpp` (nuevo), `src/shell/qml/MenuBarWindow.qml`.
   * **Implementación**:
     * Registrar en el bus de sesión el servicio `com.canonical.AppMenu.Registrar` en la ruta `/com/canonical/AppMenu/Registrar` con los métodos `RegisterWindow(uint windowId, QDBusObjectPath menuObjectPath)` y `UnregisterWindow(uint windowId)`.
     * Cuando la ventana activa tenga un `menuObjectPath` registrado, consultar su estructura mediante la interfaz `com.canonical.dbusmenu` (`GetLayout(0, -1, QStringList())`) y exponer un `QVariantList activeAppMenus` a QML.
     * En `MenuBarWindow.qml`, reemplazar los `Text` estáticos (`Archivo`, `Edición`, `Visualización`, `Ventana`, `Ayuda`) por un `Repeater` vinculado a `menuBarCtrl.activeAppMenus` que despliegue el `Menu` correspondiente al hacer clic e invoque `Event(id, "clicked", QVariant(0), timestamp)` sobre el servicio `dbusmenu` de la aplicación.

3. **Stacks del Dock (`Descargas`) y Estado Dinámico de Papelera**:
   * **Archivos**: `src/shell/dock/DockController.h/.cpp`, `src/shell/qml/DockWindow.qml`.
   * **Implementación**:
     * Añadir `Q_INVOKABLE QVariantList listStackDirectory(const QString &dirPath) const` en `DockController` que devuelva los últimos 10 archivos de `~/Descargas` (nombre, ruta, icono MIME). En `DockWindow.qml`, al hacer clic en `Descargas`, abrir un `Popup` tipo abanico/rejilla (*Grid Stack*) sobre el icono en lugar de ejecutar el binario externo.
     * Monitorizar `~/.local/share/Trash/files` con `QFileSystemWatcher` en `DockController` para alternar automáticamente el icono de la Papelera entre `user-trash` (vacía) y `user-trash-full` (llena) del tema `Colloid`.

---

## Fase 6: Paridad Funcional 1:1 en `quartz-finder`

**Objetivo**: Transformar `quartz-finder` de un visor estático de directorios en un gestor de archivos completo con las 4 vistas operativas, historial, operaciones de E/S, menús contextuales, pestañas y montaje de volúmenes.

### Tareas Atómicas para `planner` / `coder`:

1. **Conexión del Sidebar, Historial (`‹` / `›`), Barra de Ruta (*Path Bar*) y Vista Galería**:
   * **Archivos**: `src/finder/main.cpp`, `src/finder/findercolumnview.h`, `src/finder/findercolumnview.cpp`.
   * **Implementación**:
     * En `src/finder/main.cpp`, asociar cada `QListWidgetItem` del `sidebar` con su ruta real en `Qt::UserRole` (`QDir::homePath()`, `QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)`, `DocumentsLocation`, `DownloadLocation`, `QDir::homePath() + "/Proyectos"`) y conectar `QListWidget::itemClicked` a `columnView->navigateTo(path)`.
     * Implementar en `FinderColumnView` dos pilas `QStack<QString> m_backHistory` y `m_forwardHistory` con métodos `goBack()`, `goForward()`, `canGoBack()` y `canGoForward()`, conectados a las acciones `‹` y `›` del `QToolBar` en `main.cpp`.
     * En `FinderColumnView::setRootPath`, poblar también `m_galleryThumbnailStrip` (`setupGalleryView()`), conectando `m_galleryThumbnailStrip::currentItemChanged` para renderizar la miniatura escalada (`QPixmap(filePath).scaled(...)`) en `m_galleryMainPreview` y los metadatos en `m_galleryDetailsLabel`.
     * Añadir una barra inferior (`QStatusBar` o `QWidget` de `22px` de alto) que muestre la ruta de migas de pan (*Path Bar*: `Disco > Usuarios > pedro > ...`) y el espacio libre del volumen (`QStorageInfo`).

2. **Doble Clic, Operaciones de Sistema de Archivos y Atajos de Teclado**:
   * **Archivos**: `src/finder/findercolumnview.h/.cpp`, `src/finder/main.cpp`.
   * **Implementación**:
     * Conectar `itemDoubleClicked` en `m_iconsList`, `m_listTree` y `m_columns`: si el ítem es un directorio, llamar a `navigateTo(filePath)`; si es un archivo, emitir `fileDoubleClicked(filePath)` y ejecutar `QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))`.
     * Implementar métodos e invocarlos mediante `QShortcut` y `QMenu` contextual (clic derecho):
       * **Nueva Carpeta (`Ctrl+Shift+N`)**: `QDir::mkdir` con edición inline del nombre.
       * **Copiar (`Ctrl+C`) / Cortar (`Ctrl+X`) / Pegar (`Ctrl+V`)**: Escribir `QList<QUrl>` en `QApplication::clipboard()->setMimeData(...)` y realizar copia/movimiento recursivo asíncrono al pegar.
       * **Renombrar (`Return` / `F2`)**: Activar `Qt::ItemIsEditable` en el elemento seleccionado y ejecutar `QFile::rename` al confirmar.
       * **Duplicar (`Ctrl+D`)**: Crear copia con sufijo `" copia"` en el mismo directorio.
       * **Mover a la Papelera (`Delete` / `Ctrl+Backspace`)**: Invocar `QFile::moveToTrash(selectedFilePath())`.

3. **Pestañas (`Ctrl+T`), Etiquetas de Color (Finder Tags) y Volúmenes (`UDisks2` / `QStorageInfo`)**:
   * **Archivos**: `src/finder/main.cpp`, `src/finder/findercolumnview.h/.cpp`.
   * **Implementación**:
     * Envolver `FinderColumnView` en un `QTabWidget` (con `tabBarAutoHide = true` cuando hay 1 sola pestaña) y registrar `Ctrl+T` para abrir una nueva pestaña en el directorio actual.
     * Poblar una segunda sección `"Dispositivos"` en el `sidebar` enumerando `QStorageInfo::mountedVolumes()` (filtrando pseudo-sistemas `/proc`, `/sys`, `/run`, `tmpfs`).
     * Soportar etiquetas de color (Rojo, Naranja, Amarillo, Verde, Azul, Morado, Gris) guardadas en el atributo extendido `user.xdg.tags` (`setxattr` / `getxattr`) y pintar un círculo de `8px` junto al nombre del archivo en las vistas de Lista y Columnas.

---

## Fase 7: Subsistema de Hardware, Control Center, Notificaciones y `quartz-settings`

**Objetivo**: Completar los 18 paneles de Ajustes del Sistema y añadir el Control Center y el Centro de Notificaciones en el Shell para controlar el hardware real de la máquina.

### Tareas Atómicas para `planner` / `coder`:

1. **Demonio de Notificaciones (`org.freedesktop.Notifications`) y Panel Lateral en `quartz-shell`**:
   * **Archivos**: `src/shell/menubar/MenuBarController.h/.cpp`, `src/shell/notifications/NotificationServer.h/.cpp` (nuevo), `src/shell/qml/NotificationCenter.qml` (nuevo).
   * **Implementación**:
     * Implementar la especificación D-Bus `org.freedesktop.Notifications` en `/org/freedesktop/Notifications` (`GetCapabilities`, `Notify`, `CloseNotification`, `GetServerInformation`).
     * Almacenar las notificaciones recibidas en un `QAbstractListModel` expuesto a QML.
     * Mostrar banners flotantes temporales (esquina superior derecha, bajo el MenuBar de `24px`, con radio `11px`) durante 5 segundos y deslizar el panel lateral `NotificationCenter.qml` (con widgets de Fecha, Tiempo y lista de notificaciones) cuando se invoque `MenuBarController::toggleNotificationCenter()`.

2. **Control Center (Centro de Control) y OSD de Volumen/Brillo en `quartz-shell`**:
   * **Archivos**: `src/shell/controlcenter/SystemHardwareController.h/.cpp` (nuevo), `src/shell/qml/ControlCenterPopup.qml` (nuevo), `src/shell/qml/MenuBarWindow.qml`.
   * **Implementación**:
     * Crear `SystemHardwareController` conectando con:
       * Audio: `KF6::PulseAudioQt` (o `wpctl` / D-Bus PipeWire) para leer/modificar volumen maestro y estado `mute`.
       * Brillo: `/sys/class/backlight/` o `org.kde.Solid.PowerManagement.Actions.BrightnessControl` vía D-Bus.
       * Wi-Fi y Bluetooth: Estado de activación (`nmcli` / `org.freedesktop.NetworkManager` y `org.bluez`) y SSID conectado.
       * Batería: `org.freedesktop.UPower` (porcentaje y estado de carga).
     * Hacer que los iconos de estado en `MenuBarWindow.qml` muestren el nivel real y, al hacer clic, desplieguen `ControlCenterPopup.qml` con las tarjetas translúcidas estilo macOS Sonoma (Wi-Fi, Bluetooth, Modo Concentración, Brillo, Sonido y reproductor MPRIS2).

3. **Carga Dinámica de los 18 Paneles en `quartz-settings` e Integración de Configuración**:
   * **Archivos**: `src/settings/CMakeLists.txt`, `src/settings/resources.qrc` (nuevo), `src/settings/qml/SettingsMainWindow.qml`, `src/settings/qml/panes/*.qml` (nuevos), `src/settings/settingsconfigcontroller.h/.cpp`.
   * **Implementación**:
     * Añadir en `SettingsMainWindow.qml` un `Loader` dentro del `ScrollView` derecho cuya propiedad `source` esté vinculada al rol `qmlPath` (`QmlPathRole = Qt::UserRole + 5`) del elemento activo en `panesListView`.
     * Crear los paneles prioritarios funcionales en `src/settings/qml/panes/` registrados en `src/settings/resources.qrc`:
       * `AppearancePane.qml`: Selector visual de `AppearanceMode` (`Light`, `Dark`, `Auto`) y los 9 botones circulares de `AccentColor` (`Multicolor`, `Blue`, `Purple`, `Pink`, `Red`, `Orange`, `Yellow`, `Green`, `Graphite`), persistiendo en `$XDG_CONFIG_HOME/quartz/config.json` y emitiendo señal D-Bus `org.quartz.Config.ConfigChanged`.
       * `DockPane.qml`: Sliders para `base_size` (`32` a `80`), `max_magnified_size` (`48` a `128`), switches para `magnification`, `autohide`, `bouncing_animation`, `show_recent_apps` y `stage_manager.enabled`.
       * `WallpaperPane.qml`: Selector de fondos de pantalla del sistema y de usuario, actualizando en vivo `WallpaperWindow.qml`.
       * `DisplaysPane.qml`, `SoundPane.qml`, `WifiPane.qml`, `BluetoothPane.qml`, `KeyboardPane.qml`, `TrackpadPane.qml`: Conectados a `SystemHardwareController` y a la configuración de `DesktopShortcuts` (`crates/libquartz/src/shortcuts.rs`).

---

## Plan de Verificación Global (Criterios de Aceptación para `critic-ux`)

Para que cualquier fase sea marcada como `COMPLETADA` en `docs/phases/status.md` (desbloqueando el hook `.agents/scripts/require-review.py`), el agente `coder` debe garantizar:

1. **Suite Rust (`cargo test --release --workspace`)**:
   * Tests de paridad de interfaz en `libquartz` (`SessionProxy` vs `SessionLifecycleManager`).
   * Tests de parseo de archivos `.desktop` reales y evaluación de expresiones encadenadas en `quartz-spotlight`.
   * Tests de extracción de contenido de texto y metadatos MIME en `quartz-quicklookd`.

2. **Suite C++/Qt6 (`ctest --test-dir build --output-on-failure`)**:
   * `tst_quartzstyle`: Verificación de métricas, renderizado de semáforo y hojas modales.
   * `tst_stagemanager_math`: Verificación de matrices 3D, distancia superelíptica $G^2$ y sombras.
   * `tst_finder_modes`: Verificación de los 4 modos (incluyendo población de `Gallery`), historial `goBack()`/`goForward()` y operaciones de archivo en un `QTemporaryDir`.
   * `tst_settings_model` y `tst_settingsconfig`: Verificación de los 18 paneles, filtrado y lectura/escritura atómica del JSON de `QuartzDesktopConfig`.
   * `tst_shell_controllers`: Verificación de separación de argumentos en `DockController::launchApp`, curva gaussiana y estado de `MenuBarController`.

3. **Aislamiento de Compilación**:
   * `./scripts/build_and_install.sh --build-only` debe finalizar con código `0` sin escribir fuera de `build/` y `target/`.
