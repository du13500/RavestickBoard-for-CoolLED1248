#!/usr/bin/env python3
"""Synchronize web/index.html into the Arduino web UI HEADER (not the .ino).

Run from repository root: python tools/sync_web.py
This avoids the Arduino sketch preprocessor misreading embedded JavaScript
functions as C++ when they appear inside .ino raw string literals.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
WEB = ROOT / "web" / "index.html"
HEADER = ROOT / "firmware" / "RavestickBoard_v0_4_2_4_1" / "RavestickBoard_WebUI.h"

html = WEB.read_text(encoding="utf-8").rstrip("\n")
if ')RBHTML"' in html:
    raise SystemExit("HTML contains raw-string closing delimiter; use a different delimiter.")
code = HEADER.read_text(encoding="utf-8")
pattern = r'static const char PAGE_HTML\[\] PROGMEM = R"RBHTML\(\n[\s\S]*?\n\)RBHTML";'
replacement = 'static const char PAGE_HTML[] PROGMEM = R"RBHTML(\n' + html + '\n)RBHTML";'
updated, count = re.subn(pattern, lambda _: replacement, code, count=1)
if count != 1:
    raise SystemExit("PAGE_HTML section missing in header; nothing changed.")
HEADER.write_text(updated, encoding="utf-8")
print(f"Synchronisiert: {WEB.relative_to(ROOT)} -> {HEADER.relative_to(ROOT)}")
