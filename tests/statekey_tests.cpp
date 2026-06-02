// ============================================================
// statekey_tests.cpp
// Pruebas unitarias para StateKey.
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cube/Cube.cpp state/State.cpp \
//       solver/StateKey.cpp tests/statekey_tests.cpp \
//       -o statekey_tests
//   ./statekey_tests
// ============================================================

#include <iostream>
#include <string>
#include <array>
#include <map>
#include "../solver/StateKey.h"

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

// Cubo estandar: top=0, bottom=1, north=2, south=3, east=4, west=5
Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// State con todas las celdas de oro presentes y cubo fresco.
State makeBaseState() {
    std::array<bool,6> allGold = {true, true, true, true, true, true};
    return State(2, 3, makeFreshCube(), allGold);
}

// ============================================================
// TEST 1 - Construccion correcta desde State
//
// Verifica que todos los campos del StateKey coincidan
// exactamente con los del State de origen.
//
// State usado:
//   row=2, col=3
//   Cube(0,1,2,3,4,5): top=0, bottom=1, north=2, south=3, east=4, west=5
//   cubeGold: ningun oro (fresh cube)
//   remainingGold: {true,false,true,false,true,false}
// ============================================================
void test1_construccion() {
    std::cout << "--- TEST 1: Construccion correcta desde State ---\n";

    std::array<bool,6> rg = {true, false, true, false, true, false};
    State s(2, 3, Cube(0, 1, 2, 3, 4, 5), rg);
    StateKey k(s);

    // Posicion
    reportTest("test1a: row == 2",  k.row == 2);
    reportTest("test1b: col == 3",  k.col == 3);

    // Orientacion
    reportTest("test1c: top == 0",    k.top    == 0);
    reportTest("test1d: bottom == 1", k.bottom == 1);
    reportTest("test1e: north == 2",  k.north  == 2);
    reportTest("test1f: south == 3",  k.south  == 3);
    reportTest("test1g: east == 4",   k.east   == 4);
    reportTest("test1h: west == 5",   k.west   == 5);

    // Oro en caras fisicas: ninguna (fresh cube)
    bool allCubeFacesEmpty = true;
    for (int i = 0; i < 6; ++i)
        if (k.cubeGold[i]) { allCubeFacesEmpty = false; break; }
    reportTest("test1i: cubeGold todo false (sin oro)",  allCubeFacesEmpty);

    // Oro en celdas
    reportTest("test1j: remainingGold[0] == true",  k.remainingGold[0] == true);
    reportTest("test1k: remainingGold[1] == false", k.remainingGold[1] == false);
}

// ============================================================
// TEST 2 - Mismo State produce mismo StateKey
//
// Dos States construidos identicamente deben producir
// StateKeys iguales. Garantia de determinismo.
// ============================================================
void test2_mismosEstados() {
    std::cout << "\n--- TEST 2: Mismo State -> mismo StateKey ---\n";

    State s1 = makeBaseState();
    State s2 = makeBaseState();

    StateKey k1(s1);
    StateKey k2(s2);

    reportTest("test2a: k1 == k2  (operator==)",   k1 == k2);
    reportTest("test2b: !(k1 < k2) (operator<)",  !(k1 < k2));
    reportTest("test2c: !(k2 < k1) (operator<)",  !(k2 < k1));
}

// ============================================================
// TEST 3 - Diferente posicion -> distinto StateKey
//
// Un cambio en row o col debe distinguir los states.
// El orden lexicografico de row es correcto.
// ============================================================
void test3_diferentePosicion() {
    std::cout << "\n--- TEST 3: Diferente posicion -> distinto StateKey ---\n";

    std::array<bool,6> allGold = {true,true,true,true,true,true};

    StateKey kA(State(1, 0, makeFreshCube(), allGold));  // row=1
    StateKey kB(State(2, 0, makeFreshCube(), allGold));  // row=2
    StateKey kC(State(1, 1, makeFreshCube(), allGold));  // row=1, col=1

    reportTest("test3a: diferente row -> k != k",  kA != kB);
    reportTest("test3b: diferente col -> k != k",  kA != kC);
    reportTest("test3c: row=1 < row=2 (orden correcto)", kA < kB && !(kB < kA));
}

// ============================================================
// TEST 4 - Diferente orientacion del cubo -> distinto StateKey
//
// Cubo base: Cube(0,1,2,3,4,5)
// Cubo rotado (rollNorth):
//   top=3, bottom=2, north=0, south=1, east=4, west=5
// ============================================================
void test4_diferenteOrientacion() {
    std::cout << "\n--- TEST 4: Diferente orientacion -> distinto StateKey ---\n";

    std::array<bool,6> allGold = {true,true,true,true,true,true};

    Cube rotated = makeFreshCube();
    rotated.rollNorth();  // top=3, bottom=2, north=0, south=1, east=4, west=5

    StateKey kBase(State(0, 0, makeFreshCube(), allGold));
    StateKey kRot (State(0, 0, rotated,         allGold));

    reportTest("test4a: orientacion diferente -> k != k",  kBase != kRot);
    reportTest("test4b: top difiere  (0 != 3)",  kBase.top    != kRot.top);
    reportTest("test4c: bottom difiere (1 != 2)", kBase.bottom != kRot.bottom);
}

// ============================================================
// TEST 5 - Diferente oro en cubo -> distinto StateKey
//
// Cubo base: sin oro.
// Cubo con oro: putGoldOnBottom() -> cara 1 (bottom inicial) tiene oro.
// ============================================================
void test5_diferenteOroCubo() {
    std::cout << "\n--- TEST 5: Diferente oro en cubo -> distinto StateKey ---\n";

    std::array<bool,6> allGold = {true,true,true,true,true,true};

    Cube cWithGold = makeFreshCube();
    cWithGold.putGoldOnBottom();  // cara 1 recibe oro

    StateKey kNoGold  (State(0, 0, makeFreshCube(), allGold));
    StateKey kWithGold(State(0, 0, cWithGold,       allGold));

    reportTest("test5a: cubeGold diferente -> k != k",       kNoGold != kWithGold);
    reportTest("test5b: kWithGold.cubeGold[1] == true",       kWithGold.cubeGold[1]);
    reportTest("test5c: kNoGold.cubeGold[1]   == false",     !kNoGold.cubeGold[1]);
}

// ============================================================
// TEST 6 - Diferente remainingGold -> distinto StateKey
//
// Mismo cubo, misma posicion, pero celdas de oro distintas.
// ============================================================
void test6_diferenteRemainingGold() {
    std::cout << "\n--- TEST 6: Diferente remainingGold -> distinto StateKey ---\n";

    Cube c = makeFreshCube();
    std::array<bool,6> allGold  = {true,  true, true, true, true, true};
    std::array<bool,6> oneMiss  = {false, true, true, true, true, true};

    StateKey kAll (State(0, 0, c, allGold));
    StateKey kMiss(State(0, 0, c, oneMiss));

    reportTest("test6a: remainingGold diferente -> k != k",  kAll != kMiss);
    reportTest("test6b: kAll.remainingGold[0]=true, kMiss[0]=false",
               kAll.remainingGold[0] && !kMiss.remainingGold[0]);
}

// ============================================================
// TEST 7 - operator< es un orden total estricto
//
// Verifica las tres propiedades fundamentales:
//   - Irreflexividad : !(k < k)
//   - Asimetria      : k1 < k2 => !(k2 < k1)
//   - Transitividad  : k1 < k2 && k2 < k3 => k1 < k3
//
// Usando tres estados con rows 1, 2, 3.
// ============================================================
void test7_ordenTotalEstricto() {
    std::cout << "\n--- TEST 7: operator< es orden total estricto ---\n";

    std::array<bool,6> allGold = {true,true,true,true,true,true};

    StateKey k1(State(1, 0, makeFreshCube(), allGold));
    StateKey k2(State(2, 0, makeFreshCube(), allGold));
    StateKey k3(State(3, 0, makeFreshCube(), allGold));

    reportTest("test7a: irreflexividad !(k1 < k1)",  !(k1 < k1));
    reportTest("test7b: k1 < k2",                     k1 < k2);
    reportTest("test7c: asimetria !(k2 < k1)",       !(k2 < k1));
    reportTest("test7d: k2 < k3",                     k2 < k3);
    reportTest("test7e: transitividad k1 < k3",       k1 < k3);
}

// ============================================================
// TEST 8 - std::map<StateKey, int> funciona correctamente
//
// Verifica que StateKey puede usarse como clave de std::map:
//   - Insercion correcta
//   - Lookup por clave
//   - Sin colisiones entre claves distintas
//   - Sobreescritura de valor
//   - Copia del mismo State produce la misma clave (lookup exitoso)
// ============================================================
void test8_stdMap() {
    std::cout << "\n--- TEST 8: std::map<StateKey, int> ---\n";

    std::array<bool,6> allGold = {true,true,true,true,true,true};

    State s1(0, 0, makeFreshCube(), allGold);
    State s2(1, 0, makeFreshCube(), allGold);
    State s3(2, 0, makeFreshCube(), allGold);

    std::map<StateKey, int> dist;
    dist[StateKey(s1)] = 10;
    dist[StateKey(s2)] = 20;
    dist[StateKey(s3)] = 30;

    reportTest("test8a: mapa tiene 3 entradas",
               dist.size() == 3);
    reportTest("test8b: dist[key(s1)] == 10",
               dist[StateKey(s1)] == 10);
    reportTest("test8c: dist[key(s2)] == 20",
               dist[StateKey(s2)] == 20);

    // Sobreescritura
    dist[StateKey(s1)] = 99;
    reportTest("test8d: sobreescritura de s1 -> 99",
               dist[StateKey(s1)] == 99);

    // Copia del mismo State debe indexar la misma clave
    State s1_copy = s1;
    reportTest("test8e: copia de s1 tiene la misma clave",
               dist.count(StateKey(s1_copy)) == 1);
}

// ============================================================
// TEST 9 - printKey() no causa crashes
//
// Verifica que printKey() ejecuta sin errores para distintas
// configuraciones: sin oro, con oro en caras y en celdas.
// ============================================================
void test9_printKey() {
    std::cout << "\n--- TEST 9: printKey ---\n";

    // Estado fresco: sin oro en ninguna cara
    std::cout << "\n[StateKey: estado inicial]\n";
    StateKey(makeBaseState()).printKey();

    // Estado con oro en cubo y algunas celdas recogidas
    Cube c = makeFreshCube();
    c.putGoldOnBottom();   // cara 1
    c.rollNorth();
    c.putGoldOnBottom();   // cara 2
    std::array<bool,6> rg = {false, false, true, true, true, true};
    State s(4, 1, c, rg);

    std::cout << "\n[StateKey: 2 caras con oro, 2 celdas recogidas]\n";
    StateKey(s).printKey();

    reportTest("test9a: printKey sin oro ejecuto sin errores",  true);
    reportTest("test9b: printKey con oro ejecuto sin errores",  true);
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "===========================================\n";
    std::cout << "  StateKey Tests\n";
    std::cout << "===========================================\n";

    test1_construccion();
    test2_mismosEstados();
    test3_diferentePosicion();
    test4_diferenteOrientacion();
    test5_diferenteOroCubo();
    test6_diferenteRemainingGold();
    test7_ordenTotalEstricto();
    test8_stdMap();
    test9_printKey();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
