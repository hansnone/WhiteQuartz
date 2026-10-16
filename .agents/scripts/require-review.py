#!/usr/bin/env python3
import sys, json, os, re

status_file = "/home/pedro/Proyectos/WhiteQuartz/docs/phases/status.md"
if not os.path.exists(status_file):
    print(json.dumps({
        "decision": "continue",
        "reason": f"No se encontró {status_file}."
    }))
    sys.exit(0)

with open(status_file, "r", encoding="utf-8") as f:
    content = f.read()

# Prohibir la etiqueta genérica no verificada "COMPLETADA (100%)"
if "COMPLETADA (100%)" in content:
    print(json.dumps({
        "decision": "continue",
        "reason": "Prohibido escribir 'COMPLETADA (100%)'. Usa la convención de estados [x] VERIFICADA (con Evidencia:), [~] IMPLEMENTADA o [ ] PENDIENTE."
    }))
    sys.exit(0)

# Dividir por secciones de Fase
phases = re.split(r'\n(?=## Fase \d+)', content)

for phase in phases:
    if not phase.strip().startswith("## Fase"):
        continue

    # Extraer título
    title_match = re.search(r'## (Fase \d+[^:\n]*)', phase)
    title = title_match.group(1) if title_match else "Fase desconocida"

    # Verificar estado
    status_match = re.search(r'-\s+\*\*Estado:\*\*\s*(.+)', phase)
    if not status_match:
        continue
    status_str = status_match.group(1).strip()

    # Si está marcada como verificada [x], exigir campo Evidencia no vacío y válido
    if "[x]" in status_str:
        evidencia_match = re.search(r'-\s+\*\*Evidencia:\*\*\s*(.+?)(?=\n-|\Z)', phase, re.DOTALL)
        if not evidencia_match:
            print(json.dumps({
                "decision": "continue",
                "reason": f"En {title}, el estado es [x] pero no se encontró el campo '- **Evidencia:**'."
            }))
            sys.exit(0)

        evidencia = evidencia_match.group(1).strip()
        if not evidencia or "pendiente" in evidencia.lower() or len(evidencia) < 15:
            print(json.dumps({
                "decision": "continue",
                "reason": f"En {title}, el estado es [x] pero 'Evidencia:' no contiene comandos y salidas válidas. Si aún no se ha probado en la VM, usa el estado [~]."
            }))
            sys.exit(0)

print(json.dumps({"decision": "allow"}))
sys.exit(0)
