#!/usr/bin/env python3
"""
validate.py — Validador de casos oficiales para AGRA_FINAL

Uso:
    python validate.py                              # pruebas/in.txt y pruebas/out.txt
    python validate.py pruebas2/in.txt pruebas2/out.txt

Requiere: g++ en PATH, ejecutar desde la raiz del proyecto.
"""

import subprocess
import sys
import os
import platform

# ────────────────────────────────────────────────────────────
# Configuracion
# ────────────────────────────────────────────────────────────
SOURCES = [
    "cube/Cube.cpp",
    "state/State.cpp",
    "transition/Transition.cpp",
    "grid/Grid.cpp",
    "cost/Cost.cpp",
    "solver/StateKey.cpp",
    "solver/Solver.cpp",
    "main.cpp",
]
BINARY       = "./main.exe" if platform.system() == "Windows" else "./main"
COMPILE_CMD  = ["g++", "-std=c++17", "-O2"] + SOURCES + ["-o", "main"]
TIMEOUT_SECS = 10


# ────────────────────────────────────────────────────────────
# Helpers de presentacion
# ────────────────────────────────────────────────────────────
SEP = "=" * 47

def section(title=""):
    if title:
        print(f"\n{SEP}\n  {title}\n{SEP}")
    else:
        print(SEP)


# ────────────────────────────────────────────────────────────
# Paso 1: Compilar
# ────────────────────────────────────────────────────────────
def compile_project():
    section("Compilando")

    if not os.path.exists("main.cpp"):
        print("[ERROR] main.cpp no existe. Impleméntalo primero.")
        sys.exit(1)

    result = subprocess.run(COMPILE_CMD, capture_output=True, text=True)
    if result.returncode != 0:
        print("[ERROR DE COMPILACIÓN]")
        print(result.stderr)
        sys.exit(1)

    print("[OK] Compilación exitosa.")


# ────────────────────────────────────────────────────────────
# Paso 2: Ejecutar con timeout
# ────────────────────────────────────────────────────────────
def run_program(in_path):
    section("Ejecutando programa")

    try:
        with open(in_path, "r") as fin:
            result = subprocess.run(
                [BINARY],
                stdin=fin,
                capture_output=True,
                text=True,
                timeout=TIMEOUT_SECS,
            )
    except subprocess.TimeoutExpired:
        print(f"[TIMEOUT] el programa excedió {TIMEOUT_SECS} segundos")
        sys.exit(1)

    if result.returncode != 0:
        print(f"[ERROR DE EJECUCIÓN] código de salida: {result.returncode}")
        if result.stderr:
            print(result.stderr)
        sys.exit(1)

    return result.stdout


# ────────────────────────────────────────────────────────────
# Normalizacion (CAMBIO 1)
#
# - splitlines() convierte CRLF → LF y elimina newline final
#   del archivo sin tocar el contenido de cada linea.
# - NO se hace .strip() ni .rstrip() por linea: espacios
#   internos y trailing spaces se preservan exactamente.
#   "abc " != "abc" — el judge lo diferencia, nosotros tambien.
# ────────────────────────────────────────────────────────────
def normalize(text):
    return text.splitlines()


# ────────────────────────────────────────────────────────────
# Diagnostico de mismatch (CAMBIO 2)
# ────────────────────────────────────────────────────────────
def diagnose(expected, got):
    if expected.rstrip("\r\n") == got.rstrip("\r\n"):
        return "difieren solo en whitespace al final de línea"
    if expected.lower() == got.lower():
        return "difieren solo en mayúsculas/minúsculas"
    if expected.replace(" ", "") == got.replace(" ", ""):
        return "difieren solo en espacios internos"
    return None


def report_mismatch(case_num, global_line, expected, got):
    print(f"\n[FAIL] caso {case_num}  (línea global: {global_line})")
    print(f"  EXPECTED:\n    {expected}")
    print(f"  GOT:\n    {got}")

    hint = diagnose(expected, got)
    if hint:
        print(f"  NOTA: {hint}")
        print(f"  repr expected: {repr(expected)}")
        print(f"  repr got:      {repr(got)}")


# ────────────────────────────────────────────────────────────
# Paso 5: Comparar caso por caso
# ────────────────────────────────────────────────────────────
def compare(expected_lines, got_lines, out_path, gen_path):
    section("Comparando resultados")

    n_exp = len(expected_lines)
    n_got = len(got_lines)

    if n_exp != n_got:
        print(f"[ERROR ESTRUCTURAL]")
        print(f"  expected: {n_exp} líneas")
        print(f"  got:      {n_got} líneas")
        print(f"  Revisa:   diff -u {out_path} {gen_path}")
        sys.exit(1)

    passed = 0
    failed = []

    for i, (exp, got) in enumerate(zip(expected_lines, got_lines)):
        case_num    = i + 1
        global_line = i + 1
        if exp == got:
            print(f"[PASS] caso {case_num}")
            passed += 1
        else:
            report_mismatch(case_num, global_line, exp, got)
            failed.append(case_num)

    return passed, n_exp, failed


# ────────────────────────────────────────────────────────────
# Main
# ────────────────────────────────────────────────────────────
def main():
    in_path  = sys.argv[1] if len(sys.argv) > 1 else "pruebas/in.txt"
    out_path = sys.argv[2] if len(sys.argv) > 2 else "pruebas/out.txt"
    gen_path = os.path.join(os.path.dirname(os.path.abspath(out_path)), "generated.txt")

    # Paso 1
    compile_project()

    # Paso 2
    raw_output = run_program(in_path)

    # Paso 3: guardar output generado (CAMBIO 3)
    with open(gen_path, "w", newline="\n") as f:
        f.write(raw_output)
    print(f"[INFO] Output guardado en: {gen_path}")

    # Paso 4: parsear con normalizacion correcta (CAMBIO 1)
    with open(out_path, "r", newline="") as f:
        expected_raw = f.read()

    expected_lines = normalize(expected_raw)
    got_lines      = normalize(raw_output)

    # Paso 5
    passed, total, failed = compare(expected_lines, got_lines, out_path, gen_path)

    # Reporte final
    print()
    section()
    print(f"  Resultado: {passed}/{total} passed")
    if failed:
        print(f"  Fallidos:  {failed}")
    section()

    if failed:
        print(f"\nPara debugging manual:")
        print(f"  diff -u {out_path} {gen_path}\n")
        sys.exit(1)
    else:
        print("\n[OK] Todos los casos pasaron.\n")


if __name__ == "__main__":
    main()
