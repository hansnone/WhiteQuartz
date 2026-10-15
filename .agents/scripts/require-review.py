#!/usr/bin/env python3
import sys, json, os

status_file = "/home/pedro/Proyectos/WhiteQuartz/docs/phases/status.md"
if os.path.exists(status_file):
    with open(status_file) as f:
        content = f.read()
    if "COMPLETADA" in content:
        print(json.dumps({"decision": "allow"}))
        sys.exit(0)

print(json.dumps({
    "decision": "continue",
    "reason": "Ejecuta la skill critic-ux antes de considerar la tarea terminada o actualiza status.md."
}))
