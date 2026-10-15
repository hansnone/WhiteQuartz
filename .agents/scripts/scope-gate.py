#!/usr/bin/env python3
import sys, json, os

payload = json.load(sys.stdin)
tool = payload.get("toolCall", {})
args = tool.get("args", {})

# Ajusta los paths permitidos según tu plan
ALLOWED = [
    "src/", "tests/", "docs/", "crates/", "scripts/", 
    "data/", "Cargo.toml", "Cargo.lock", "CMakeLists.txt",
    "README.md", ".agents/", ".gitignore"
]

target = args.get("TargetFile") or args.get("path") or ""
workspace_root = "/home/pedro/Proyectos/WhiteQuartz"

if target:
    # Permitir artifacts del brain
    if target.startswith("/home/pedro/.gemini/antigravity/brain/"):
        print(json.dumps({"decision": "allow"}))
        sys.exit(0)
        
    rel = os.path.relpath(target, workspace_root) if os.path.isabs(target) else target
    if rel.startswith("./"):
        rel = rel[2:]
        
    if not any(rel == p.rstrip("/") or rel.startswith(p) for p in ALLOWED):
        print(json.dumps({
            "decision": "deny",
            "reason": f"Path no permitido: {target} (relativo: {rel}). Solo se permiten: {ALLOWED}"
        }))
        sys.exit(0)

print(json.dumps({"decision": "allow"}))
