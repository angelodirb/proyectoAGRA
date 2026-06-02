// ============================================================
// solver_tests.cpp
// Pruebas unitarias para solve().
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cube/Cube.cpp state/State.cpp \
//       transition/Transition.cpp grid/Grid.cpp \
//       solver/StateKey.cpp solver/Solver.cpp \
//       tests/solver_tests.cpp -o solver_tests
//   ./solver_tests
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include "../grid/Grid.h"
#include "../solver/Solver.h"

// ------------------------------------------------------------
// Contadores globales de tests
// ------------------------------------------------------------
static int testsPassed = 0;
static int testsFailed = 0;

void reportTest(const std::string& name, bool passed) {
    if (passed) {
        std::cout << "[PASS] " << name << "\n";
        ++testsPassed;
    } else {
        std::cout << "[FAIL] " << name << "\n";
        ++testsFailed;
    }
}

// ============================================================
// TEST 1 - Grid imposible: celda de oro inaccesible
//
// Layout (6 columnas, 3 filas):
//   "GGGSGG"   fila 0: G en (0,0),(0,1),(0,2),(0,4),(0,5)
//              start en (0,3)
//   "######"   fila 1: pared completa, infranqueable
//   "G....."   fila 2: G en (2,0), completamente aislado
//
// El cubo parte en (0,3) y solo puede moverse este/oeste
// dentro de la fila 0. La pared de la fila 1 bloquea todo
// acceso a la fila 2.
//
// La celda gc[5]=(2,0) es inalcanzable: el cubo nunca puede
// realizar el evento CASO 1 que pondria oro en la 6a cara.
//
// Esperado: solve() == -1
// ============================================================
void test1_imposible() {
    std::cout << "--- TEST 1: Grid imposible ---\n";

    std::vector<std::string> layout = {
        "GGGSGG",
        "######",
        "G....."
    };
    Grid g(layout);

    int result = solve(g, 1, 1);
    reportTest("test1a: resultado == -1 (sin solucion)", result == -1);
}

// ============================================================
// TEST 2 - Grid simple: la solucion existe
//
// Layout (4 columnas, 4 filas):
//   "..GG"   fila 0
//   "GGG."   fila 1
//   "G..."   fila 2
//   "S..."   fila 3: start en (3,0)
//
// No hay paredes. Todos los movimientos hacia oro son validos.
// Verificamos que el solver termina y retorna un valor no negativo.
// ============================================================
void test2_solucionExiste() {
    std::cout << "\n--- TEST 2: Solucion existe ---\n";

    std::vector<std::string> layout = {
        "..GG",
        "GGG.",
        "G...",
        "S..."
    };
    Grid g(layout);

    int result = solve(g, 1, 1);
    reportTest("test2a: resultado >= 0 (solucion existe)", result >= 0);
    reportTest("test2b: resultado > 0  (no trivial: el cubo no nace con oro)",
               result > 0);
}

// ============================================================
// TEST 3 - Grid con paredes: la solucion existe por otro camino
//
// Layout (5 columnas, 4 filas):
//   ".G.G."   fila 0
//   "G.S.G"   fila 1: start en (1,2)
//   ".G.#."   fila 2: pared en (2,3)
//   "....G"   fila 3
//
// La pared bloquea algunos movimientos pero no aisla ningun
// oro. El solver debe encontrar una solucion alternativa.
//
// Esperado: solve() != -1 (solucion existe)
// ============================================================
void test3_conParedes() {
    std::cout << "\n--- TEST 3: Grid con paredes ---\n";

    std::vector<std::string> layout = {
        ".G.G.",
        "G.S.G",
        ".G.#.",
        "....G"
    };
    Grid g(layout);

    int result = solve(g, 1, 1);
    reportTest("test3a: resultado != -1 (solucion existe pese a pared)",
               result != -1);
    reportTest("test3b: resultado >= 6  (minimo provable: 6 eventos CASO 1)",
               result >= 6);
}

// ============================================================
// TEST 4 - Distancia exacta conocida: 6 movimientos
//
// Layout (4 columnas, 4 filas):
//   "..GG"   fila 0: gc[0]=(0,2), gc[1]=(0,3)
//   "GGG."   fila 1: gc[2]=(1,0), gc[3]=(1,1), gc[4]=(1,2)
//   "G..."   fila 2: gc[5]=(2,0)
//   "S..."   fila 3: start en (3,0)
//
// Camino optimo trazado manualmente: N, N, E, E, N, E
//   Cubo inicial: top=0, bottom=1, N=2, S=3, E=4, W=5
//
//   Paso 1  N -> (2,0)=gc[5]  rollN: bottom=cara2  CASO 1: cara2 recoge oro
//   Paso 2  N -> (1,0)=gc[2]  rollN: bottom=cara0  CASO 1: cara0 recoge oro
//   Paso 3  E -> (1,1)=gc[3]  rollE: bottom=cara4  CASO 1: cara4 recoge oro
//   Paso 4  E -> (1,2)=gc[4]  rollE: bottom=cara1  CASO 1: cara1 recoge oro
//   Paso 5  N -> (0,2)=gc[0]  rollN: bottom=cara3  CASO 1: cara3 recoge oro
//   Paso 6  E -> (0,3)=gc[1]  rollE: bottom=cara5  CASO 1: cara5 recoge oro
//
// Resultado: las 6 caras tienen oro. allGoldCollected() == true.
//
// Minimo provable: 6 movimientos
//   Justificacion: cada cara gana oro por un unico evento CASO 1.
//   Cada movimiento produce a lo sumo 1 evento CASO 1.
//   Por lo tanto, se necesitan al menos 6 movimientos.
//
// El camino N,N,E,E,N,E logra exactamente 6.
// Dijkstra garantiza que si existe solucion en 6 pasos,
// retorna 6 (no mas).
//
// Esperado: solve() == 6
// ============================================================
void test4_distanciaExacta() {
    std::cout << "\n--- TEST 4: Distancia exacta = 6 ---\n";

    std::vector<std::string> layout = {
        "..GG",
        "GGG.",
        "G...",
        "S..."
    };
    Grid g(layout);

    int result = solve(g, 1, 1);
    reportTest("test4a: resultado == 6 (camino optimo exacto)", result == 6);
}

// ============================================================
// TEST 5 - Determinismo: misma grilla -> mismo resultado siempre
//
// El solver usa estructuras deterministas (std::map con
// operator< lexicografico, Dijkstra con cola ordenada).
// Dos llamadas con la misma grilla deben dar el mismo resultado.
//
// Tambien verifica terminacion: si hubiera un loop infinito,
// la segunda llamada nunca terminaria.
// ============================================================
void test5_determinismo() {
    std::cout << "\n--- TEST 5: Determinismo y terminacion ---\n";

    std::vector<std::string> layout = {
        "..GG",
        "GGG.",
        "G...",
        "S..."
    };
    Grid g(layout);

    int r1 = solve(g, 1, 1);
    int r2 = solve(g, 1, 1);

    reportTest("test5a: primera llamada termina  (>= -1)", r1 >= -1);
    reportTest("test5b: segunda llamada termina  (>= -1)", r2 >= -1);
    reportTest("test5c: resultados identicos",              r1 == r2);
}

// ============================================================
// TEST 6 - Modo debug: no causa crashes, retorna el mismo valor
//
// Activa debug=true y verifica que:
//   - El solver imprime sin crashear
//   - El resultado con debug coincide con el resultado sin debug
// ============================================================
void test6_debug() {
    std::cout << "\n--- TEST 6: Modo debug ---\n";

    // Grilla imposible (debug con retorno -1)
    {
        std::vector<std::string> impossible = {
            "GGGSGG",
            "######",
            "G....."
        };
        Grid gi(impossible);
        std::cout << "\n[debug en grilla imposible]\n";
        int rd = solve(gi, 1, 1, true);
        int rn = solve(gi, 1, 1, false);
        reportTest("test6a: debug en grilla imposible no crashea",  rd >= -1);
        reportTest("test6b: debug e imposible coinciden (ambos -1)", rd == rn);
    }

    // Grilla con solucion (debug con retorno 6)
    {
        std::vector<std::string> solvable = {
            "..GG",
            "GGG.",
            "G...",
            "S..."
        };
        Grid gs(solvable);
        std::cout << "\n[debug en grilla con solucion]\n";
        int rd = solve(gs, 1, 1, true);
        int rn = solve(gs, 1, 1, false);
        reportTest("test6c: debug en grilla solvable no crashea",    rd >= 0);
        reportTest("test6d: debug y no-debug dan el mismo resultado", rd == rn);
    }
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "===========================================\n";
    std::cout << "  Solver Tests\n";
    std::cout << "===========================================\n\n";

    test1_imposible();
    test2_solucionExiste();
    test3_conParedes();
    test4_distanciaExacta();
    test5_determinismo();
    test6_debug();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
