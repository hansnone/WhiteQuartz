Aquí tienes un `README.md` completo y listo para copiar en la raíz del proyecto:

```markdown
# Flujo de desarrollo multi-agente (Antigravity)

Sistema de skills para desarrollo profesional de software con roles separados, planificación por fases y control de calidad + UX/UI.

## Estructura del proyecto

```
.agents/
├── skills/
│   ├── phase-planner/SKILL.md
│   ├── codebase-analyzer/SKILL.md
│   ├── planner/SKILL.md
│   ├── coder/SKILL.md
│   └── critic-ux/SKILL.md
├── rules/
│   └── project.md
├── scripts/
│   ├── scope-gate.py
│   ├── force-artifact.py
│   └── require-review.py
└── hooks.json

docs/
├── requirements/          # Documentos técnicos de origen
├── phases/                # Roadmap y estado de fases
│   ├── roadmap.md
│   └── status.md
└── references/            # Análisis de otros softwares / features
```

## Skills disponibles

| Skill                | Responsabilidad                                      | Cuándo usarla                                      |
|----------------------|------------------------------------------------------|----------------------------------------------------|
| `phase-planner`      | Genera roadmap por fases a partir de requisitos     | Inicio de proyecto o replanificación               |
| `codebase-analyzer`  | Extrae características reutilizables de otro software | Cuando se quiere copiar/inspirarse en features     |
| `planner`            | Genera plan de implementación detallado              | Antes de escribir código                           |
| `coder`              | Implementa el plan de forma literal                  | Después de aprobar el plan                         |
| `critic-ux`          | Revisa código + UX/UI y da veredicto                 | Después de cada implementación                     |

## Flujo de trabajo

### Proyecto nuevo / grande

```
docs/requirements/
        ↓
phase-planner          →  docs/phases/roadmap.md
        ↓
codebase-analyzer      →  (opcional) docs/references/
        ↓
planner                →  plan de la fase activa
        ↓
[Aprobación humana]
        ↓
coder                  →  implementación + tests
        ↓
critic-ux              →  informe de revisión
        ↓
RECHAZADO → coder (con feedback)
APROBADO  → siguiente fase / commit / PR
```

### Proyecto ya existente

- **Nueva feature o cambio**:
  ```
  /skill planner
  Quiero añadir/modificar [descripción]. Analiza el código actual y genera el plan.
  ```
- **Refactor**:
  ```
  /skill planner
  Analiza el módulo X y propone un plan de refactor seguro.
  ```
- **Extraer features de otro software**:
  ```
  /skill codebase-analyzer
  Analiza [ruta o descripción] y extrae las características de [lista].
  ```
- **Revisión de calidad**:
  ```
  /skill critic-ux
  Revisa los cambios recientes / el módulo Y.
  ```

## Comandos de interacción recomendados

| Situación                        | Mensaje                                                      |
|----------------------------------|--------------------------------------------------------------|
| Generar roadmap                  | `/skill phase-planner` + “Analiza docs/requirements/”       |
| Analizar software de referencia  | `/skill codebase-analyzer` + ruta o descripción             |
| Planificar fase o feature        | `/skill planner` + descripción clara                        |
| Aprobar plan                     | `Plan aprobado`                                              |
| Implementar                      | `Ejecuta el plan con coder`                                  |
| Revisar                          | `/skill critic-ux`                                           |
| Corregir tras rechazo            | `Aplica las correcciones del informe del critic-ux`         |
| Cerrar fase                      | `Actualiza docs/phases/status.md`                            |

## Reglas permanentes (`.agents/rules/project.md`)

- Lee siempre los documentos de `docs/requirements/` antes de planificar o implementar.
- Si existe `docs/phases/roadmap.md`, no implementes fuera de la fase activa aprobada.
- Antes de proponer soluciones, busca en `docs/references/`.
- Cita el documento de origen o el análisis de referencia en cada decisión importante.
- Al finalizar una fase, actualiza `docs/phases/status.md`.

## Buenas prácticas

1. **Control humano en los puntos críticos**: aprueba siempre el plan del `planner` y el veredicto del `critic-ux`.
2. **Planes pequeños**: máximo 8-12 tareas atómicas por plan.
3. **Trazabilidad**: guarda los Artifacts (planes, informes, análisis) en `docs/`.
4. **Proyectos existentes**: no es obligatorio generar roadmap completo. Usa `planner` + `coder` + `critic-ux` directamente.
5. **Si las skills no aparecen**: cierra y vuelve a cargar el Project en Antigravity.

## Hooks (opcional)

El archivo `.agents/hooks.json` aplica:
- Control de paths permitidos (`src/`, `tests/`, `docs/`).
- Recordatorio de ejecutar `critic-ux` al finalizar.

## Resumen rápido

```
1. phase-planner     → roadmap
2. codebase-analyzer → (opcional) features de referencia
3. planner           → plan de la fase/feature
4. [Aprobación]
5. coder             → implementación
6. critic-ux         → revisión
7. Corregir o avanzar
```
```

Copia el contenido anterior en un archivo `README.md` en la raíz del repositorio. Con esto tendrás toda la documentación del flujo centralizada y clara para el futuro.