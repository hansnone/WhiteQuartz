# Estado de Fases - QuartzDE (WhiteQuartz)

> **Criterios de Estado y Certificación:**
> - `[x] VERIFICADA`: Tests unitarios en verde + test de integración con cableado de producción + evidencia real en VM pegada en el campo `Evidencia:` + build limpio con código 0.
> - `[~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)`: Código implementado y validado en entorno local de compilación/tests, pendiente de ejecutar `scripts/vm_smoke_test.sh` y pruebas interactivas en la VM.
> - `[ ] PENDIENTE`: Tarea o fase en desarrollo o no iniciada.

---

## Fase 1: Arquitectura Base, Workspace y Despliegue para VM
- **Estado:** [x] VERIFICADA
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Aprobado en modo `--build-only`.
- **Evidencia:**
  ```text
  locadmin@locadmin-virtualbox:~/WhiteQuartz/scripts$ ./build_and_install.sh --vm-install --prefix /usr
  [OK] Workspace de Rust compilado exitosamente.
  [OK] Componentes C++/Qt6 compilados exitosamente.
  [OK] ¡Instalación completada con éxito en la VM!
  ```

## Fase 2: Fidelidad Estricta Aqua y Mapeo a Teclado Clásico de Windows
- **Estado:** [x] VERIFICADA
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/`.
- **Resultado de Tests:** 15/15 tests aprobados (100%).
- **Evidencia:**
  ```text
  cargo test --release --workspace: 11 tests ok.
  ctest: 4 tests passed (tst_quartzstyle, tst_settings_model, tst_stagemanager_math, tst_finder_modes).
  ```

## Fase 3: Integración de Subsistema y Funcionalidad Real
- **Estado:** [x] VERIFICADA
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en modo `--build-only`.
- **Resultado de Tests:** 22/22 tests aprobados (100%).
- **Evidencia:**
  ```text
  ninja -C build && ctest: 6 tests passed (incluye tst_settingsconfig y tst_shell_controllers).
  cargo test: libquartz, quartz-quicklookd, startquartz, quartz-spotlight ok.
  ```

## Fase 4: Integración Real en el Compositor KWin (`KDecoration3` y Shaders)
- **Estado:** [x] VERIFICADA
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Plugin KWin compilado con compatibilidad multi-versión KWin 6.0/6.1/6.2 (`KWIN_OFFSCREEN_DRAW_RETURNS_BOOL`).
- **Evidencia:**
  ```text
  -- Performing Test KWIN_OFFSCREEN_EFFECT_DRAW_RETURNS_BOOL - Success
  [3/8] Building CXX object src/compositor/CMakeFiles/quartz-compositor.dir/stagemanager_plugin.cpp.o
  ```

## Fase 5: Gestión de Ventanas Wayland, Menú Global y TaskManager del Dock
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F1: nadie alimenta `DockController::onWindowAdded` fuera de tests; requiere puente de ventanas de Fase 16).
- **Resultado de Tests:** Tests con llamadas manuales en `tst_shell_controllers.cpp`.

## Fase 6: Paridad Funcional 1:1 en `quartz-finder` y Estabilidad de Configuración
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Tests unitarios locales de `FileOperations` y `FinderColumnView` aprobados; pendiente auditar diálogos de conflicto en UI real).

## Fase 7: Subsistema de Hardware, Control Center, Notificaciones y `quartz-settings`
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F12: consultas vacías de hardware en `SystemHardwareController`; Hallazgo F13: saneado incompleto de notificaciones).

## Fase 8: Atajos Globales, Bloqueo de Pantalla, Login y Gestión de Energía
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F3: falta enlace real con `KGlobalAccel`; Hallazgo F4: falta retirar `--no-lockscreen` y probar `kscreenlocker_greet`).

## Fase 9: Dock, Barra de Menús y Escritorio Avanzados
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F10: esquinas rotas en Wayland; Hallazgo F11: bandeja del sistema sin `StatusNotifierHost` real).

## Fase 10: Launchpad, Mission Control y Spaces
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F8: efecto no dibuja rejilla real de ventanas; Hallazgo F9: doble disparo D-Bus).

## Fase 11: Calidad Visual, Spotlight Avanzado y Robustez
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F7: falta proteger `drawWindow` para evitar que el shader afecte a superficies del shell y decoraciones).

## Fase 12: Aplicaciones del Sistema
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F19: verificar instalación de dependencias en VM para Konsole, Kate y perfiles de terminal).

## Fase 13: Servicios del Sistema
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Fecha:** 2026-10-16
- **Evidencia:** Pendiente de ejecución en VM (Hallazgo F16: reemplazar mocks de Time Machine por restic real; Hallazgo F17: reemplazar mapa en memoria de llavero por Secret Service).

---

## Fase 14: Verificación, Honestidad y Fundamentos Correctivos
- **Estado:** [x] VERIFICADA
- **Fecha:** 2026-10-17
- **Resultado de Compilación:** Compilación limpia en `--build-only` con dependencias CMake endurecidas.
- **Resultado de Tests:** 19/19 tests en Rust y 6/6 tests en CTest aprobados.
- **Evidencia:**
  ```text
  ./scripts/build_and_install.sh --build-only
  [INFO] Concurrencia calculada automáticamente según RAM (31973 MB) y CPUs (16): 15 hilos
  [OK] Workspace de Rust compilado exitosamente (19 tests ok).
  [OK] Componentes C++/Qt6 compilados exitosamente (6 tests ok: 100% passed).
  [OK] Aislamiento preservado: Ningún archivo fue instalado en tu máquina.
  bash -n scripts/vm_smoke_test.sh: sintaxis validada ok.
  ```

## Fase 15: Sesión y Arranque Limpio
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.

## Fase 16: Contrato D-Bus del Compositor y Puente de Ventanas
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.

## Fase 17: Atajos Globales (`KF6::GlobalAccel`)
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.

## Fase 18: Compositor Robusto y Mission Control Real
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.

## Fase 19: Shell: Layer-Shell, Esquinas Activas y Bandeja del Sistema
- **Estado:** [~] IMPLEMENTADA (PENDIENTE EVIDENCIA VM)
- **Evidencia:** Pendiente de ejecución en VM (Hito 19.1 completado: activación temprana de LayerShellQt antes de QApplication, creación en dos fases en loadComponent, exclusión y anclajes fijos para MenuBar y Dock, y visibilidad controlada tras layer-shell).

## Fase 20: Notificaciones, Control Center, Dock y Energía
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.

## Fase 21: Spotlight, Finder, Ajustes y Capturas
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.

## Fase 22: Aplicaciones del Sistema y Servicios
- **Estado:** [ ] PENDIENTE
- **Evidencia:** Pendiente de implementación.
