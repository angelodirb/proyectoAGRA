// ============================================================
// state_tests.cpp
// Pruebas unitarias para la clase State.
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cube/Cube.cpp state/State.cpp tests/state_tests.cpp -o state_tests
//   ./state_tests
// ============================================================

#include <iostream>
#include <string>
#include <array>
#include "../state/State.h"

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

// ------------------------------------------------------------
// Helpers de construccion
// ------------------------------------------------------------

// Cubo estandar con IDs 0..5 y sin oro en ninguna cara.
//   top=0, bottom=1, north=2, south=3, east=4, west=5
Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// Estado inicial tipico:
//   - posicion (0, 0)
//   - cubo estandar sin oro
//   - las 6 celdas originales tienen oro
State makeInitialState() {
    std::array<bool,6> allCellsHaveGold = {true, true, true, true, true, true};
    return State(0, 0, makeFreshCube(), allCellsHaveGold);
}

// Construye un cubo donde las 6 caras fisicas tienen oro.
//
// Secuencia verificada (Cube(0,1,2,3,4,5)):
//   bottom en cada paso: 1, 2, 0, 3, 4, 5
//   movimientos: N, N, N, E, N+N
//
// Despues de esta funcion: cube.countGoldFaces() == 6.
Cube makeCubeWithAllGold() {
    Cube c(0, 1, 2, 3, 4, 5);

    c.putGoldOnBottom();                    // cara 1
    c.rollNorth();
    c.putGoldOnBottom();                    // cara 2
    c.rollNorth();
    c.putGoldOnBottom();                    // cara 0
    c.rollNorth();
    c.putGoldOnBottom();                    // cara 3
    c.rollEast();
    c.putGoldOnBottom();                    // cara 4
    c.rollNorth(); c.rollNorth();
    c.putGoldOnBottom();                    // cara 5

    return c;
}

// ============================================================
// TEST 1 - Creacion correcta del estado
//
// Verifica que el constructor asigna correctamente todos los
// campos: row, col, orientacion del cubo, remainingGold.
// ============================================================
void test1_creacionCorrecta() {
    std::cout << "--- TEST 1: Creacion correcta ---\n";

    std::array<bool,6> remaining = {true, false, true, false, true, false};
    State s(2, 5, Cube(0, 1, 2, 3, 4, 5), remaining);

    reportTest("test1a: row asignado correctamente",
               s.row == 2);
    reportTest("test1b: col asignado correctamente",
               s.col == 5);
    reportTest("test1c: cube.getTop() == 0",
               s.cube.getTop() == 0);
    reportTest("test1d: cube.getBottom() == 1",
               s.cube.getBottom() == 1);
    reportTest("test1e: cube.getNorth() == 2",
               s.cube.getNorth() == 2);
    reportTest("test1f: cube.getSouth() == 3",
               s.cube.getSouth() == 3);
    reportTest("test1g: cube.getEast() == 4",
               s.cube.getEast() == 4);
    reportTest("test1h: cube.getWest() == 5",
               s.cube.getWest() == 5);
    reportTest("test1i: remainingGold[0] == true",
               s.remainingGold[0] == true);
    reportTest("test1j: remainingGold[1] == false",
               s.remainingGold[1] == false);
    reportTest("test1k: cubo recien creado no tiene oro en ninguna cara",
               s.cube.countGoldFaces() == 0);
    reportTest("test1l: faceHasGold(0) == false en cubo sin oro",
               s.cube.faceHasGold(0) == false);
}

// ============================================================
// TEST 2 - remainingGoldCount
//
// Verifica que el conteo de celdas con oro sea correcto para
// distintas configuraciones de remainingGold.
// ============================================================
void test2_remainingGoldCount() {
    std::cout << "\n--- TEST 2: remainingGoldCount ---\n";

    Cube c = makeFreshCube();

    State s6(0, 0, c, {true, true, true, true, true, true});
    reportTest("test2a: 6 celdas con oro -> count == 6",
               s6.remainingGoldCount() == 6);

    State s0(0, 0, c, {false, false, false, false, false, false});
    reportTest("test2b: 0 celdas con oro -> count == 0",
               s0.remainingGoldCount() == 0);

    State s3(0, 0, c, {true, false, true, false, true, false});
    reportTest("test2c: 3 celdas con oro -> count == 3",
               s3.remainingGoldCount() == 3);

    State s1last(0, 0, c, {false, false, false, false, false, true});
    reportTest("test2d: 1 celda con oro (ultima) -> count == 1",
               s1last.remainingGoldCount() == 1);

    State s1first(0, 0, c, {true, false, false, false, false, false});
    reportTest("test2e: 1 celda con oro (primera) -> count == 1",
               s1first.remainingGoldCount() == 1);
}

// ============================================================
// TEST 3 - allGoldCollected
//
// Verifica que allGoldCollected() retorne true solo cuando
// las 6 caras fisicas del cubo tienen oro simultaneamente.
// ============================================================
void test3_allGoldCollected() {
    std::cout << "\n--- TEST 3: allGoldCollected ---\n";

    std::array<bool,6> allRG = {true, true, true, true, true, true};
    std::array<bool,6> noRG  = {false, false, false, false, false, false};

    // Estado inicial: ningun oro en el cubo
    State s = makeInitialState();
    reportTest("test3a: estado inicial -> false",
               !s.allGoldCollected());

    // Una cara con oro: aun no alcanza el objetivo
    s.cube.putGoldOnBottom();
    reportTest("test3b: 1 cara con oro -> false",
               !s.allGoldCollected());

    // Cubo con todas las caras con oro
    State sAll(0, 0, makeCubeWithAllGold(), noRG);
    reportTest("test3c: 6 caras con oro -> true",
               sAll.allGoldCollected());
    reportTest("test3d: cube.countGoldFaces() == 6",
               sAll.cube.countGoldFaces() == 6);

    // Verificar via faceHasGold que todas las caras tienen oro
    bool allFacesHaveGold = true;
    for (int i = 0; i < 6; ++i) {
        if (!sAll.cube.faceHasGold(i)) { allFacesHaveGold = false; break; }
    }
    reportTest("test3e: faceHasGold(i) == true para toda i",
               allFacesHaveGold);
}

// ============================================================
// TEST 4 - Comparacion de estados (operator== y operator!=)
//
// Verifica que dos estados iguales sean considerados iguales,
// y que cualquier diferencia en cualquier campo los distinga.
// ============================================================
void test4_comparacion() {
    std::cout << "\n--- TEST 4: Comparacion de estados ---\n";

    std::array<bool,6> allRG = {true, true, true, true, true, true};

    State s1 = makeInitialState();
    State s2 = makeInitialState();

    // Estados identicos
    reportTest("test4a: estados identicos -> ==",
               s1 == s2);
    reportTest("test4b: estados identicos -> no !=",
               !(s1 != s2));

    // Diferente row
    State sDiffRow(1, 0, makeFreshCube(), allRG);
    reportTest("test4c: diferente row -> !=",
               s1 != sDiffRow);
    reportTest("test4d: diferente row -> no ==",
               !(s1 == sDiffRow));

    // Diferente col
    State sDiffCol(0, 3, makeFreshCube(), allRG);
    reportTest("test4e: diferente col -> !=",
               s1 != sDiffCol);

    // Diferente orientacion del cubo (top y bottom intercambiados)
    State sDiffOrient(0, 0, Cube(1, 0, 2, 3, 4, 5), allRG);
    reportTest("test4f: diferente orientacion del cubo -> !=",
               s1 != sDiffOrient);

    // Diferente oro en caras del cubo: un cubo tiene 1 cara con oro
    Cube cWithGold = makeFreshCube();
    cWithGold.putGoldOnBottom();  // cara 1 tiene oro
    State sDiffGold(0, 0, cWithGold, allRG);
    reportTest("test4g: diferente oro en caras del cubo -> !=",
               s1 != sDiffGold);

    // Diferente remainingGold (celda 0 ya recogida)
    std::array<bool,6> rgPartial = {false, true, true, true, true, true};
    State sDiffRemaining(0, 0, makeFreshCube(), rgPartial);
    reportTest("test4h: diferente remainingGold -> !=",
               s1 != sDiffRemaining);

    // Copia debe ser independiente (value semantics)
    State s3 = s1;
    s3.row = 7;
    reportTest("test4i: modificar copia no afecta original",
               s1.row == 0 && s3.row == 7);
    reportTest("test4j: copia modificada es distinta del original",
               s1 != s3);

    // Cubo rotado: distinto aunque misma posicion
    Cube rotated = makeFreshCube();
    rotated.rollNorth();
    State sRotated(0, 0, rotated, allRG);
    reportTest("test4k: cubo rotado -> distinto aunque misma posicion",
               s1 != sRotated);
}

// ============================================================
// TEST 5 - printState
//
// Verifica que printState() no cause crashes y produzca
// salida coherente para distintas configuraciones de estado.
// ============================================================
void test5_printState() {
    std::cout << "\n--- TEST 5: printState ---\n";

    // Estado inicial (cubo sin oro, todas las celdas con oro)
    std::cout << "\n[estado inicial]\n";
    makeInitialState().printState();

    // Estado con 2 caras del cubo con oro y algunas celdas recogidas
    Cube c(0, 1, 2, 3, 4, 5);
    c.putGoldOnBottom();   // cara 1
    c.rollNorth();
    c.putGoldOnBottom();   // cara 2

    std::cout << "\n[2 caras con oro, posicion (3,4), 2 celdas recogidas]\n";
    State s2(3, 4, c, {false, false, true, true, true, true});
    s2.printState();

    // Estado objetivo: todas las caras del cubo con oro
    std::cout << "\n[objetivo logrado: 6 caras con oro]\n";
    State sGoal(7, 7, makeCubeWithAllGold(), {false,false,false,false,false,false});
    sGoal.printState();

    reportTest("test5: printState ejecuto sin errores en 3 configuraciones",
               true);
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "===========================================\n";
    std::cout << "  State Tests\n";
    std::cout << "===========================================\n";

    test1_creacionCorrecta();
    test2_remainingGoldCount();
    test3_allGoldCollected();
    test4_comparacion();
    test5_printState();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
