#!/usr/bin/env python3
"""Naeherung an die Arduino-Uebersetzung: Der Sketch wird mit seinen eingebundenen .h-Dateien in der Reihenfolge der
#include-Zeilen zu EINEM Text zusammengesetzt. Pruefung: Ein Name, der auf oberster Ebene (Klammertiefe 0) deklariert
wird, darf nicht vor seiner ersten Deklaration benutzt werden. Ausgenommen sind Funktionen des .ino selbst (dafuer
erzeugt die IDE Prototypen). Gefunden wird z. B. 'rb42ProgramPlaying' in RavestickBoard_Anim.h, wenn Anim.h VOR
RavestickBoard_v042.h eingebunden wird (Compile-Fehler 0.6.1.0). Aufruf: decl_order.py <Pfad zur .ino>
Naeherung ohne Praeprozessor und ohne Typauflosung: kann Falschmeldungen liefern (dann gezielt in IGNORE eintragen),
ersetzt keinen Compile."""
import re, sys
from pathlib import Path
SKIP_INCLUDES = {'RavestickBoard_WebUI.h', 'RavestickBoard_Assets.h', 'RavestickBoard_Icon.h'}   # nur Daten
KEYWORDS = set('if else for while do switch case default return break continue goto sizeof static const constexpr volatile inline extern struct class enum union typedef namespace using new delete this true false nullptr void bool char int short long float double unsigned signed auto template typename public private protected operator try catch throw'.split())

def strip(text):
    out = []; i = 0; n = len(text)
    while i < n:
        c = text[i]
        if text.startswith('//', i):
            j = text.find('\n', i); j = n if j < 0 else j; out.append(' ' * (j - i)); i = j
        elif text.startswith('/*', i):
            j = text.find('*/', i + 2); j = n if j < 0 else j + 2
            out.append(''.join(ch if ch == '\n' else ' ' for ch in text[i:j])); i = j
        elif text.startswith('R"', i) and re.match(r'R"([^(\s]*)\(', text[i:]):
            m = re.match(r'R"([^(\s]*)\(', text[i:]); end = ')' + m.group(1) + '"'
            j = text.find(end, i); j = n if j < 0 else j + len(end)
            out.append(''.join(ch if ch == '\n' else ' ' for ch in text[i:j])); i = j
        elif c in '"\'':
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == '\\' else 1
            j = min(j + 1, n)
            out.append(''.join(ch if ch == '\n' else ' ' for ch in text[i:j])); i = j
        else:
            out.append(c); i += 1
    return ''.join(out)

def expand(ino_path):
    """-> Liste (Text, ist_ino) in Include-Reihenfolge, nur eigene .h-Dateien"""
    folder = ino_path.parent; src = ino_path.read_text(encoding='utf-8')
    parts = []; pos = 0; combined = ''; kinds = []
    for m in re.finditer(r'^[ \t]*#include\s+"([^"]+)"[^\n]*$', src, re.M):
        name = m.group(1)
        parts.append((src[pos:m.start()], True))
        if name not in SKIP_INCLUDES and (folder / name).exists():
            parts.append((((folder / name).read_text(encoding='utf-8')), False))
        pos = m.end()
    parts.append((src[pos:], True))
    return parts

def check(ino_path, ignore=()):
    parts = expand(Path(ino_path))
    text = ''; mask = []          # mask: True = Zeichen stammt aus dem .ino
    for t, is_ino in parts:
        s = strip(t) + '\n'
        text += s; mask += [is_ino] * len(s)
    decl = {}                     # Name -> (Offset der ersten Deklaration, aus_ino)
    depth = 0; i = 0; n = len(text); stmt_start = 0
    def note(name, off):
        if name in KEYWORDS or name in decl: return
        decl[name] = (off, mask[off])
    paren = 0
    while i < n:
        c = text[i]
        if c == '(': paren += 1
        elif c == ')': paren -= 1
        if c == '{' and paren == 0:
            if depth == 0:
                head = text[stmt_start:i]
                if re.match(r'\s*(typedef\s+)?(enum|struct|class|union)\b', head) and '(' not in head:
                    m = re.match(r'\s*(?:typedef\s+)?(?:enum|struct|class|union)\s+(?:class\s+)?(\w+)', head)
                    if m: note(m.group(1), stmt_start + head.index(m.group(1)))
                    if head.rstrip().endswith('='): pass
                elif '(' in head and re.search(r'(\w+)\s*\([^()]*(\([^()]*\)[^()]*)*\)\s*(const|noexcept|override)?\s*$', head.replace('\n', ' ')):
                    m = None
                    for m in re.finditer(r'(\w+)\s*\(', head): break
                    if m and not head.lstrip().startswith(('namespace', 'extern')): note(m.group(1), stmt_start + m.start(1))
                else:
                    m = re.search(r'(\w+)\s*(?:\[[^\]]*\])*\s*=\s*$', head)
                    if m: note(m.group(1), stmt_start + m.start(1))
            depth += 1
        elif c == '}' and paren == 0:
            depth -= 1
            if depth == 0: stmt_start = i + 1
        elif c == ';' and depth == 0 and paren == 0:
            head = text[stmt_start:i]
            hs = head.strip()
            if hs and not hs.startswith(('#', 'using', 'typedef')):
                m = re.match(r'\s*(?:enum|struct|class|union)\s+(\w+)\s*$', hs)
                if m: note(m.group(1), stmt_start + head.index(m.group(1)))
                elif '(' in hs and '=' not in hs.split('(')[0]:
                    m = re.search(r'(\w+)\s*\(', head)
                    if m: note(m.group(1), stmt_start + m.start(1))
                else:
                    core = re.split(r'=', hs, maxsplit=1)[0]
                    for part in core.split(','):
                        m = re.search(r'(\w+)\s*(?:\[[^\]]*\])*\s*$', part)
                        if m and not part.strip().endswith(')'):
                            note(m.group(1), stmt_start + head.index(part) + m.start(1))
            stmt_start = i + 1
        elif c == '#' and depth == 0:
            j = text.find('\n', i)
            while j > 0 and text[j - 1] == '\\': j = text.find('\n', j + 1)
            i = (n if j < 0 else j); stmt_start = i + 1; continue
        i += 1
    problems = []
    for name, (off, from_ino) in decl.items():
        if name in ignore or len(name) < 3: continue
        # Funktionen des .ino bekommen von der IDE Prototypen
        if from_ino and re.match(r'\s*\(', text[off + len(name):off + len(name) + 40]) and mask[off]:
            # nur Funktionsdefinitionen/-deklarationen im .ino
            continue
        for m in re.finditer(r'(?<![\w.>:])' + re.escape(name) + r'\b', text[:off]):
            k = m.start()
            line = text[text.rfind('\n', 0, k) + 1:text.find('\n', k)]
            if re.match(r'\s*(#|using)', line): continue
            problems.append(f'{name}: benutzt in Zeile {text.count(chr(10), 0, k) + 1} ({"ino" if mask[k] else "h"}), erst danach deklariert (Zeile {text.count(chr(10), 0, off) + 1})')
            break
    return problems

if __name__ == '__main__':
    ign = ()
    probs = check(sys.argv[1], ign)
    for p in probs: print('PROBLEM', p)
    print('OK' if not probs else f'{len(probs)} Problem(e)')
    sys.exit(1 if probs else 0)
