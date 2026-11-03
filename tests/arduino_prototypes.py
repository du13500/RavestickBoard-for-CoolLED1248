"""Emuliert die Prototyp-Erzeugung der Arduino-IDE: Alle Prototypen landen vor der ERSTEN Funktionsdefinition.
Jeder selbstdefinierte Typ, der in irgendeiner Funktionssignatur vorkommt, muss davor definiert (oder vorab deklariert) sein."""
import re, sys
from pathlib import Path

def check(text):
    lines = text.split('\n')
    typ = {}
    for i, l in enumerate(lines):
        m = re.match(r'(?:enum\s+class|enum|struct|class|union)\s+(\w+)\b', l)
        if m: typ.setdefault(m.group(1), i)
        m = re.match(r'(?:typedef\s+.*\s+(\w+);|using\s+(\w+)\s*=)', l)
        if m: typ.setdefault(m.group(1) or m.group(2), i)
    kw = ('if', 'for', 'while', 'switch', 'else', 'do', 'return', 'namespace', 'extern', 'typedef', 'struct', 'class', 'enum', 'union', 'using', '#', '//', '/*', '*', '}', '{')
    funcs = []
    depth = 0
    for i, l in enumerate(lines):
        if depth == 0 and l and not l[0].isspace() and not l.startswith(kw) and re.search(r'\)\s*(const\s*)?\{\s*(//.*)?$', l) and '(' in l and '=' not in l.split('(')[0]:
            funcs.append((i, l))
        depth += l.count('{') - l.count('}')
    if not funcs: return ['keine Funktion gefunden']
    first = funcs[0][0]
    problems = []
    for i, sig in funcs:
        head = sig.split('{')[0]
        for t, pos in typ.items():
            if re.search(r'\b%s\b' % re.escape(t), head) and pos > first:
                problems.append(f'Zeile {i+1}: {head.strip()} nutzt Typ {t} (Zeile {pos+1}) vor der ersten Funktion (Zeile {first+1})')
    return problems

if __name__ == '__main__':
    probs = check(Path(sys.argv[1]).read_text(encoding='utf-8'))
    print('\n'.join(probs) if probs else 'OK')
    sys.exit(1 if probs else 0)
