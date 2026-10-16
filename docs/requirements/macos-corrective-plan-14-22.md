# QuartzDE: Plan Correctivo (Fases 14 a 22)

Documento para el flujo multi-agente (`phase-planner`, `planner`, `coder`, `critic-ux`). Sustituye el estado "COMPLETADA" de las fases 5, 6, 7, 8, 9, 10, 11, 12 y 13 por una lista de trabajo verificable. Complementa `macos-1to1-spec.md` y `macos-gap-closure-plan.md`.

Origen: auditoría del volcado `repomix-output_2a808e.xml` (156 archivos). Todo lo marcado **Verificado** se leyó en el código. Lo marcado **A verificar** depende del entorno (VM, versión de KWin/Plasma) y el agente debe comprobarlo antes de actuar.

---

## 0. Lectura previa obligatoria

### 0.1 Por qué existe este plan

El `roadmap.md` marca todo como completado, pero los tests actuales usan mocks y llaman a mano a los métodos (por ejemplo `dock.onWindowAdded(...)` en `tst_shell_controllers.cpp`). Eso demuestra que el método funciona, no que alguien lo invoque en producción. Resultado: el proyecto compila y pasa tests, pero la sesión real no responde al teclado, no ve ventanas y varios servicios son maquetas.

### 0.2 Nueva definición de "hecho" (sustituye a la de `status.md`)

Una tarea solo se marca `[x]` si cumple **las cuatro**:

1. Tests unitarios en verde (`cargo test --release --workspace`, `ctest`).
2. **Test de integración que use el cableado real de producción** (el objeto construido como en `main.cpp`, no un mock que se invoca a mano).
3. **Evidencia de ejecución en la VM**: comando + salida pegados en `status.md` (campo `Evidencia:`). Sin evidencia, la casilla queda como `[~]` (implementado sin verificar).
4. `./scripts/build_and_install.sh --build-only` con código 0.

Estados permitidos en el roadmap: `[ ]` pendiente, `[~]` implementado sin evidencia en VM, `[x]` verificado. Prohibido escribir "COMPLETADA (100%)" sin `[x]` en todas las tareas.

### 0.3 Hallazgos de la auditoría

Severidad: **C** crítico (el escritorio no funciona sin esto), **A** alto, **M** medio.

| ID | Sev | Hallazgo (Verificado) | Evidencia |
|----|-----|-----------------------|-----------|
| F1 | C | **Nadie alimenta la lista de ventanas.** `DockController::onWindowAdded/Removed/ActiveWindowChanged`, `MenuBarController::onActiveWindowChanged` y `MissionControlController::updateWindows` no tienen ningún llamador en producción, solo en tests. No hay script de KWin, ni uso de `KWindowSystem`/TaskManager en el código (solo se enlaza `KF6::WindowSystem` para el blur). | grep de `onWindowAdded`: solo definición, cabecera y tests |
| F2 | C | **Nombres D-Bus del compositor inconsistentes.** El efecto registra su objeto con `QDBusConnection::sessionBus().registerObject(...)` dentro del proceso de KWin, así que el servicio dueño es `org.kde.KWin`. `GlobalShortcuts` y `MissionControlController` llaman a `org.quartz.Compositor` (no existe). Además llaman a `toggle_mission_control` / `toggle_app_expose` y el efecto exporta `toggleMissionControl` / `toggleAppExpose`. El proxy Rust de `libquartz` usa `org.kde.KWin` y nombres `set_enabled` (zbus los convierte a `SetEnabled`, que tampoco coincide con `setEnabled` de Qt). | `stagemanagereffect.cpp`, `GlobalShortcuts.cpp`, `MissionControlController.cpp`, `dbus/stagemanager.rs` |
| F3 | C | **Atajos globales no enlazados.** `GlobalShortcuts` solo expone métodos D-Bus. No hay `KGlobalAccel` ni `QAction`, y `CMakeLists.txt` del shell no busca `KF6GlobalAccel`. | grep `KGlobalAccel`: 0 |
| F4 | C | **Sin pantalla de bloqueo.** `quartz-session` lanza `kwin_wayland --xwayland --no-lockscreen`. Además `metadata.json` del Look-and-Feel usa `ServiceTypes` (estilo Plasma 5). | `main.rs`, `metadata.json` |
| F5 | C | **Cuatro overlays sin configurar como layer-shell.** `configureLayerShell` no trata los tipos `app-switcher`, `force-quit`, `launchpad` ni `mission-control`, que se cargan en `main.cpp`. Se abrirán como ventanas normales (xdg-toplevel). | `main.cpp` |
| F6 | C | **Orden de creación de las superficies layer-shell.** `loadComponent` hace `component.create()` y luego `configureLayerShell`. Si el QML tiene `visible: true` (el de `WallpaperWindow.qml` lo tiene), la ventana se muestra antes de asignar el rol layer-shell y la configuración puede ignorarse o fallar. Tampoco se llama a `LayerShellQt::Shell::useLayerShell()`. | `main.cpp`, `WallpaperWindow.qml` |
| F7 | C | **Efecto de KWin sin protección y con transformaciones sobre todas las ventanas.** (a) `drawWindow` aplica el shader squircle a **cualquier** ventana (barra de menús, Dock, fondo, bloqueo). (b) `apply()` con Stage Manager activo escala, rota y traslada **todas** las ventanas al mismo punto `(24, 70)`. (c) 0 apariciones de `nullptr`, `isValid`, `isSupported` en `stagemanagereffect.cpp` y `quartzdecoration.cpp`. (d) `reconfigure()` regenera el shader en cada llamada. | `stagemanager_plugin.cpp` |
| F8 | A | **Mission Control no existe en el compositor.** El efecto solo cambia `m_spatialMode`. El plugin no usa `calculateMissionControlTransform`. El controlador del shell crea espacios falsos ("Escritorio 1/2") si KWin no responde. `activateWindow`, `removeSpace` y `switchToSpace` no llaman a KWin. | `stagemanager_plugin.cpp`, `MissionControlController.cpp` |
| F9 | A | **Doble disparo latente en Mission Control.** `TriggerMissionControl()` alterna el estado local **y** pide al compositor que alterne; el controlador además conecta `spatialModeChanged` al slot `toggle()`. Cuando se arreglen los nombres D-Bus (F2), cada pulsación activará y desactivará. | `GlobalShortcuts.cpp`, `MissionControlController.cpp` |
| F10 | A | **Esquinas activas rotas en Wayland.** Se consulta `QCursor::pos()` cada 50 ms. En Wayland, un cliente solo conoce el puntero mientras está sobre sus superficies; fuera devuelve el último valor o `(0,0)`, que cuenta como esquina superior izquierda. Además `main.cpp` fija las acciones (esquina 0 = Mission Control, 1 = Centro de Notificaciones, 2 = Spotlight, 3 = nada) e ignora la configuración. | `HotCornersController.cpp`, `main.cpp` |
| F11 | A | **Bandeja del sistema engañosa.** `IsStatusNotifierHostRegistered` devuelve siempre `true`, pero no existe ningún host que pinte los iconos. Las apps creen que hay bandeja, no usan su alternativa y el icono desaparece. Tampoco se lee `IconPixmap` ni `DBusMenu`. Además, `RegisterStatusNotifierItem` ignora el remitente del mensaje: si la app pasa solo una ruta (`/StatusNotifierItem`), el item queda sin servicio. | `StatusNotifierWatcher.*` |
| F12 | A | **Control Center con lecturas vacías.** `queryPulseAudio`, `queryBacklight`, `queryBluez` y `queryMpris` tienen el cuerpo vacío. El estado inicial nunca refleja el sistema. Las acciones de medios llaman al nombre `org.mpris.MediaPlayer2.active`, que no existe (el nombre real es `org.mpris.MediaPlayer2.<reproductor>`). Bluetooth usa `rfkill` en vez de `Adapter1.Powered`. Brillo usa `brightnessctl` y volumen lanza un `wpctl` por cada cambio del slider. Cada tick crea un `QDBusInterface` nuevo (introspección repetida). | `SystemHardwareController.cpp` |
| F13 | A | **Notificaciones: saneado y semántica incompletos.** El saneado solo elimina `<script>...</script>` con una regex sin `DotMatchesNewlineOption`, y deja pasar `<img src=http://...>`, `<a href=javascript:...>`, `onerror=`, etc. `expire_timeout <= 0` se trata como 5 s (cero debe significar "no expira"). Se ignora la urgencia. `NotificationClosed` se emite siempre con razón 3 (el usuario descarta → debe ser 2; expiración → 1). No hay límites de tamaño ni de frecuencia. Se anuncia la capacidad `persistence` sin persistir nada. | `NotificationServer.cpp` |
| F14 | A | **Energía y sesión con comandos externos.** Reposo/reinicio/apagado usan `systemctl` con `startDetached`; bloqueo usa `loginctl lock-session`; cerrar sesión usa `loginctl terminate-session ""` (id vacío). Una rama del menú Apple usa `xkill` (solo X11). El Dock usa `killall <binario>` y `killall -9`, que mata **todos** los procesos con ese nombre. Nada comprueba *inhibitors*. | `MenuBarController.cpp`, `DockController.cpp` |
| F15 | A | **Sesión incompleta.** `DISPLAY` queda fijado a `:0`. Solo se lanzan `quartz-quicklookd`, `quartz-spotlight` y `quartz-shell`: no hay agente de polkit, portales XDG, `kglobalacceld`, ni importación del entorno a D-Bus/systemd (los servicios activados por D-Bus no verán `WAYLAND_DISPLAY`). Si `quartz-shell` cae, nadie lo reinicia: el bucle solo vigila el compositor. | `quartz-session/src/main.rs` |
| F16 | A | **Time Machine simulado.** `startBackup()` pone el progreso a 100 %, añade una instantánea con ruta inventada y existe una instantánea falsa "2026-10-15 12:00:00". No invoca `restic` ni toca disco. | `QuartzServices.cpp` |
| F17 | A | **Llavero falso e inseguro.** Las credenciales viven en un `QVariantMap` (`m_vault`) en memoria, en claro, y se exponen por el bus de sesión sin autenticación. No persisten. | `QuartzServices.cpp` |
| F18 | M | **Impresoras.** `PrintService` ejecuta `lpstat` con `waitForFinished(500)` en el constructor, en el hilo de GUI y sin forzar `LC_ALL=C` (la salida de `lpstat` está traducida). | `QuartzServices.cpp` |
| F19 | M | **Aplicaciones del sistema** son `.desktop` que lanzan `kate`, `okular`, `plasma-systemmonitor`, con descripciones como "visor nativo". `setup_vm_dependencies.sh` no instala ninguna, ni `konsole`, `spectacle`, `brightnessctl`, `kglobalacceld`, ni el paquete de desarrollo de GlobalAccel. | `data/applications/*.desktop`, `setup_vm_dependencies.sh` |
| F20 | M | **Dependencias opcionales silenciosas.** `find_package(LayerShellQt QUIET)` y `KF6WindowSystem QUIET`: si faltan, el shell compila sin layer-shell y sin blur y no avisa. | `src/shell/CMakeLists.txt` |
| F21 | M | **Spotlight**: sin evaluador de expresiones real, sin cancelación de consultas, sin vigilancia de cambios de disco, sin control de symlinks. | `quartz-spotlight/src/main.rs` |
| F22 | M | **Capturas de pantalla** dependen de `spectacle` por línea de comandos (no del portal). | `GlobalShortcuts.cpp` |
| F23 | M | `X-KDE-Wayland-Interfaces` aparece en `data/sessions/quartz.desktop` (archivo de sesión), donde no tiene efecto. Solo importa en el `.desktop` del cliente que lo necesita. | `quartz.desktop` |
| F24 | M | `quartz-session` guarda `ConfigManager` y `SessionLifecycleManager` solo en memoria de proceso; si el proceso muere se pierde el estado de apps abiertas. | `main.rs` |

### 0.4 Correcciones a la auditoría previa

- **`ActionInvoked` sí se emite.** Declarado ausente inicialmente al mirar solo `NotificationServer.cpp`, pero está declarado como señal en el servidor y lo emite `NotificationModel::invokeAction`. Sí es pendiente que `NotificationClosed` use siempre razón 3 (F13).
- **Saneado de notificaciones**: existe, pero es insuficiente (F13).
- **`GlobalShortcuts`**: expone métodos D-Bus útiles para pruebas (`Trigger*`). Se conservan; faltaba el enlace a teclas (F3).

### 0.5 Reglas de ingeniería

No inventar APIs (abrir el header instalado en la VM), `#[serde(default)]` en todo struct de configuración, contratos D-Bus en un solo sitio, no bloquear el hilo de GUI, degradación elegante, un plugin de KWin nunca debe tirar la sesión, entrada externa no confiable, no escribir código propio de autenticación, cadenas con `tr()`/`qsTr()`.

### 0.6 Orden de ejecución y dependencias

```
Fase 14  Verificación y honestidad (roadmap, entorno, humo en VM)
Fase 15  Sesión y arranque (entorno, bloqueo, energía, supervisión)
Fase 16  Contrato D-Bus del compositor + puente de ventanas   ← desbloquea Dock, menú, switcher, Mission Control
Fase 17  Atajos globales
Fase 18  Compositor robusto + Mission Control real
Fase 19  Shell: layer-shell, esquinas activas, bandeja
Fase 20  Notificaciones, Control Center, Dock y energía
Fase 21  Spotlight, Finder, Ajustes y capturas (auditoría + correcciones)
Fase 22  Apps del sistema y servicios (Time Machine, llavero, impresoras)
```

Reglas de orden: **14 → 15 → 16 → 17** son secuenciales. 18 depende de 16. 19 depende de 16 (ventanas) y 17. 20 y 21 son paralelizables tras 17. 22 va al final.

---

## Fase 14: Verificación y honestidad (S)

### 14.1 Reiniciar el roadmap
- **Archivos**: `docs/phases/roadmap.md`, `docs/phases/status.md`.
- **Qué hacer**: cambiar a `[~]` o `[ ]` las casillas que el hallazgo contradiga: Fase 5 (seguimiento de ventanas), 7.2, 7.1 (parcial), 8.1, 8.2, 8.4, 9.1 (minimización), 9.3, 9.4, 10.2, 11.2, 11.5, 12, 13. Añadir un campo `Evidencia:` a cada bloque de `status.md`.
- **Aceptación**: `require-review.py` exige que `status.md` tenga `Evidencia:` no vacío para pasar a `[x]`.

### 14.2 Dependencias y compilación estricta
- **Archivos**: `scripts/setup_vm_dependencies.sh`, `src/shell/CMakeLists.txt`, `CMakeLists.txt`, `scripts/build_and_install.sh`.
- **Paquetes a añadir**:
  - Compilación: `libkf6globalaccel-dev`, `libkf6windowsystem-dev`, `libkf6kio-dev`, `qt6-pdf-dev`, `qt6-multimedia-dev`, `libkf6screen-dev` (si existe).
  - Ejecución: `kglobalacceld`, `kscreenlocker`, `polkit-kde-agent-1`, `xdg-desktop-portal`, `xdg-desktop-portal-kde`, `kded6`, `konsole`, `kate`, `okular`, `spectacle`, `plasma-systemmonitor`, `restic`, `sddm`, `kde-cli-tools`, `pipewire`, `wireplumber`, `pipewire-pulse`, `upower`, `network-manager`, `bluez`, `udisks2`, `cups`.
- **CMake**: `find_package(LayerShellQt REQUIRED)`, `find_package(KF6WindowSystem REQUIRED)` y `find_package(KF6GlobalAccel REQUIRED)` en el shell. Se permite degradar **solo** con una opción explícita `-DQUARTZ_ALLOW_MISSING_DEPS=ON`, que imprime un `message(WARNING ...)` visible. Quitar `QUIET`.
- **Aceptación**: en una VM sin `libkf6globalaccel-dev`, `cmake` falla con un mensaje claro, no compila un shell sin atajos.

### 14.3 Script de humo en VM (`scripts/vm_smoke_test.sh`)
- Se ejecuta **dentro** de la sesión QuartzDE ya iniciada. Comprueba y devuelve código ≠ 0 si algo falla:
  1. `busctl --user list` contiene `org.quartz.Session`, `org.quartz.Config`, `org.freedesktop.Notifications`, `org.kde.StatusNotifierWatcher`, `org.quartz.Compositor`.
  2. `busctl --user call org.quartz.Compositor /org/quartz/Compositor/WindowBridge org.quartz.Compositor.WindowBridge ListWindows` devuelve JSON válido.
  3. `notify-send "prueba" "cuerpo"` crea una notificación.
  4. `qdbus6 org.kde.kglobalaccel /kglobalaccel org.kde.KGlobalAccel.allComponents` incluye el componente `quartz`.
  5. `pgrep -a kscreenlocker_greet` no existe aún; tras `busctl --user call org.freedesktop.ScreenSaver /ScreenSaver org.freedesktop.ScreenSaver Lock` sí existe.
  6. `ps -o pid,cmd -C quartz-shell,quartz-spotlight,quartz-quicklookd` están vivos.
  7. `echo $WAYLAND_DISPLAY $DISPLAY` y `systemctl --user show-environment | grep WAYLAND`.
  8. `journalctl --user -b | grep -iE "segfault|QML|critical"` sin errores nuevos.

---

## Fases 15 a 22 (Visión General)
- **Fase 15**: Sesión y arranque (entorno, bloqueo, energía, supervisión).
- **Fase 16**: Contrato D-Bus del compositor y puente de ventanas.
- **Fase 17**: Atajos globales (`KF6::GlobalAccel`).
- **Fase 18**: Compositor robusto y Mission Control real.
- **Fase 19**: Shell: layer-shell, esquinas activas y bandeja.
- **Fase 20**: Notificaciones, Control Center, Dock y energía.
- **Fase 21**: Spotlight, Finder, Ajustes y capturas.
- **Fase 22**: Aplicaciones del sistema y servicios (Time Machine, llavero, impresoras).

---

## Apéndices

### Apéndice A. Configuración nueva (`libquartz::config`, todo con `#[serde(default)]`)
- `dock.pinned: Vec<String>`
- `dock.position: Bottom|Left|Right`
- `hot_corners.{top_left,top_right,bottom_left,bottom_right}`
- `hot_corners.dwell_ms`
- `lockscreen.{autolock, timeout_min, require_password}`
- `notifications.{dnd, per_app, history_limit}`
- `accessibility.{reduce_transparency, reduce_motion, increase_contrast}`
- `clock.{show_seconds, hour24, show_date}`
- `backup.*`
- `shortcuts` (mapa acción → secuencia)

### Apéndice B. Contratos D-Bus resultantes
- `org.quartz.Session` (`/org/quartz/Session`)
- `org.quartz.Config` (`/org/quartz/Config`)
- `org.quartz.Compositor` (`/org/quartz/Compositor/StageManager`)
- `org.quartz.Compositor` (`/org/quartz/Compositor/WindowBridge`)
- `org.quartz.GlobalShortcuts` (`/org/quartz/GlobalShortcuts`)
- `org.freedesktop.Notifications` (`/org/freedesktop/Notifications`)
- `org.kde.StatusNotifierWatcher` (`/StatusNotifierWatcher`)
- `com.canonical.AppMenu.Registrar` (`/com/canonical/AppMenu/Registrar`)

### Apéndice C. Matriz de aceptación final (para la VM)
Ver detalles en el plan correctivo principal para validación en ejecución viva en VirtualBox.

### Apéndice D. Lista de comprobación de seguridad
- Ningún código propio lee contraseñas (`kscreenlocker`/SDDM).
- Ningún `killall`; solo señales a PID validado y lista negra.
- Ningún `startDetached` con cadenas concatenadas; programa + argumentos.
- Notificaciones: lista blanca de HTML, límites de tamaño y frecuencia.
- Secretos: nunca en claro en disco; permisos 0600 en claves.
- Acciones privilegiadas por D-Bus + polkit; sesión nunca root.
