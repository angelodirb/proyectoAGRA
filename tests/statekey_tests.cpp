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

Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// State base: posicion (2,3), cubo fresco, 6 bits encendidos (ALL6).
static constexpr uint64_t ALL6 = 0x3FULL;

State makeBaseState() {
    return State(2, 3, makeFreshCube(), ALL6);
}

// ============================================================
// TEST 1 - Construccion correcta desde State
//
// Verifica que todos los campos del StateKey coincidan
// exactamente con los del State de origen.
// ============================================================
void test1_construccion() {
    std::cout << "--- TEST 1: Construccion correcta desde State ---\n";

    // bits 0, 2, 4 encendidos: patron alternado
    uint64_t cellGoldPattern = (1ULL<<0) | (1ULL<<2) | (1ULL<<4);
    State s(2, 3, Cube(0, 1, 2, 3, 4, 5), cellGoldPattern);
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
    reportTest("test1i: cubeGold todo false (cubo sin oro)",  allCubeFacesEmpty);

    // Oro en celdas: bitmask copiado exactamente
    reportTest("test1j: cellGold bit 0 encendido",  (k.cellGold & (1ULL<<0)) != 0);
    reportTest("test1k: cellGold bit 1 apagado",    (k.cellGold & (1ULL<<1)) == 0);
    reportTest("test1l: cellGold bit 2 encendido",  (k.cellGold & (1ULL<<2)) != 0);
    reportTest("test1m: cellGold coincide con State", k.cellGold == s.cellGold);
}

// ============================================================
// TEST 2 - Mismo State produce mismo StateKey
// ============================================================
void test2_mismosEstados() {
    std::cout << "\n--- TEST 2: Mismo State -> mismo StateKey ---\n";

    State s1 = makeBaseState();
    State s2 = makeBaseState();

    StateKey k1(s1);
    StateKey k2(s2);

    reportTest("test2a: k1 == k2  (operator==)",  k1 == k2);
    reportTest("test2b: !(k1 < k2) (operator<)", !(k1 < k2));
    reportTest("test2c: !(k2 < k1) (operator<)", !(k2 < k1));
}

// ============================================================
// TEST 3 - Diferente posicion -> distinto StateKey
// ============================================================
void test3_diferentePosicion() {
    std::cout << "\n--- TEST 3: Diferente posicion -> distinto StateKey ---\n";

    StateKey kA(State(1, 0, makeFreshCube(), ALL6));  // row=1
    StateKey kB(State(2, 0, makeFreshCube(), ALL6));  // row=2
    StateKey kC(State(1, 1, makeFreshCube(), ALL6));  // row=1, col=1

    reportTest("test3a: diferente row -> keys distintas",  kA != kB);
    reportTest("test3b: diferente col -> keys distintas",  kA != kC);
    reportTest("test3c: row=1 < row=2 (orden correcto)",   kA < kB && !(kB < kA));
}

// ============================================================
// TEST 4 - Diferente orientacion -> distinto StateKey
// ============================================================
void test4_diferenteOrientacion() {
    std::cout << "\n--- TEST 4: Diferente orientacion -> distinto StateKey ---\n";

    Cube rotated = makeFreshCube();
    rotated.rollNorth();  // top=3, bottom=2, north=0, south=1, east=4, west=5

    StateKey kBase(State(0, 0, makeFreshCube(), ALL6));
    StateKey kRot (State(0, 0, rotated,         ALL6));

    reportTest("test4a: orientacion diferente -> keys distintas",  kBase != kRot);
    reportTest("test4b: top difiere  (0 != 3)",    kBase.top    != kRot.top);
    reportTest("test4c: bottom difiere (1 != 2)",  kBase.bottom != kRot.bottom);
}

// ============================================================
// TEST 5 - Diferente oro en cubo -> distinto StateKey
// ============================================================
void test5_diferenteOroCubo() {
    std::cout << "\n--- TEST 5: Diferente oro en cubo -> distinto StateKey ---\n";

    Cube cWithGold = makeFreshCube();
    cWithGold.putGoldOnBottom();  // cara 1 recibe oro

    StateKey kNoGold  (State(0, 0, makeFreshCube(), ALL6));
    StateKey kWithGold(State(0, 0, cWithGold,       ALL6));

    reportTest("test5a: cubeGold diferente -> keys distintas",     kNoGold != kWithGold);
    reportTest("test5b: kWithGold.cubeGold[1] == true",             kWithGold.cubeGold[1]);
    reportTest("test5c: kNoGold.cubeGold[1]   == false",           !kNoGold.cubeGold[1]);
}

// ============================================================
// TEST 6 - Diferente cellGold -> distinto StateKey
//
// Mismo cubo, misma posicion, pero bitmasks de celda distintos.
// ============================================================
void test6_diferenteCellGold() {
    std::cout << "\n--- TEST 6: Diferente cellGold -> distinto StateKey ---\n";

    Cube c = makeFreshCube();
    uint64_t cellGoldAll  = 0x3FULL;  // bits 0-5 encendidos
    uint64_t cellGoldMiss = 0x3EULL;  // bit 0 apagado

    StateKey kAll (State(0, 0, c, cellGoldAll));
    StateKey kMiss(State(0, 0, c, cellGoldMiss));

    reportTest("test6a: cellGold diferente -> keys distintas",
               kAll != kMiss);
    reportTest("test6b: kAll tiene bit 0 encendido",
               (kAll.cellGold & 1ULL) != 0);
    reportTest("test6c: kMiss tiene bit 0 apagado",
               (kMiss.cellGold & 1ULL) == 0);
    reportTest("test6d: kAll.cellGold == 0x3F",
               kAll.cellGold == 0x3FULL);
    reportTest("test6e: kMiss.cellGold == 0x3E",
               kMiss.cellGold == 0x3EULL);
}

// ============================================================
// TEST 7 - operator< es un orden total estricto
//
// - Irreflexividad : !(k < k)
// - Asimetria      : k1 < k2 => !(k2 < k1)
// - Transitividad  : k1 < k2 && k2 < k3 => k1 < k3
// ============================================================
void test7_ordenTotalEstricto() {
    std::cout << "\n--- TEST 7: operator< es orden total estricto ---\n";

    StateKey k1(State(1, 0, makeFreshCube(), ALL6));
    StateKey k2(State(2, 0, makeFreshCube(), ALL6));
    StateKey k3(State(3, 0, makeFreshCube(), ALL6));

    reportTest("test7a: irreflexividad !(k1 < k1)",  !(k1 < k1));
    reportTest("test7b: k1 < k2",                     k1 < k2);
    reportTest("test7c: asimetria !(k2 < k1)",       !(k2 < k1));
    reportTest("test7d: k2 < k3",                     k2 < k3);
    reportTest("test7e: transitividad k1 < k3",       k1 < k3);
}

// ============================================================
// TEST 8 - std::map<StateKey, int> funciona correctamente
// ============================================================
void test8_stdMap() {
    std::cout << "\n--- TEST 8: std::map<StateKey, int> ---\n";

    State s1(0, 0, makeFreshCube(), ALL6);
    State s2(1, 0, makeFreshCube(), ALL6);
    State s3(2, 0, makeFreshCube(), ALL6);

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

    // Copia del mismo State indexa la misma clave
    State s1_copy = s1;
    reportTest("test8e: copia de s1 tiene la misma clave",
               dist.count(StateKey(s1_copy)) == 1);
}

// ============================================================
// TEST 9 - Igualdad y desigualdad: cellGold distingue estados
//
// Verifica que cellGold forme parte correcta del criterio de
// identidad en StateKey: mismo cubo, misma posicion, distinto
// bitmask => keys distintas.
// ============================================================
void test9_cellGoldDistingue() {
    std::cout << "\n--- TEST 9: cellGold distingue states correctamente ---\n";

    Cube c = makeFreshCube();

    StateKey kFull (State(0, 0, c, 0x3FULL));
    StateKey kNone (State(0, 0, c, 0ULL));
    StateKey kPart (State(0, 0, c, 0x1FULL));  // bits 0-4

    // Distintos bitmasks -> distintas keys
    reportTest("test9a: 0x3F != 0x00 -> keys distintas",   kFull != kNone);
    reportTest("test9b: 0x3F != 0x1F -> keys distintas",   kFull != kPart);
    reportTest("test9c: 0x1F != 0x00 -> keys distintas",   kPart != kNone);

    // Mismo bitmask -> misma key
    StateKey kFull2(State(0, 0, c, 0x3FULL));
    reportTest("test9d: mismo 0x3F -> keys iguales",       kFull == kFull2);

    // Verificar valores almacenados
    reportTest("test9e: kFull.cellGold == 0x3F",   kFull.cellGold == 0x3FULL);
    reportTest("test9f: kNone.cellGold == 0",       kNone.cellGold == 0ULL);
    reportTest("test9g: kPart.cellGold == 0x1F",   kPart.cellGold == 0x1FULL);

    // Diferencia por un solo bit
    StateKey kA(State(0, 0, c, 0x3FULL));         // bit 0 encendido
    StateKey kB(State(0, 0, c, 0x3FULL & ~1ULL)); // bit 0 apagado
    reportTest("test9h: diferencia de 1 bit -> keys distintas", kA != kB);

    // Mismas keys en std::map
    std::map<StateKey, int> m;
    m[kFull]  = 1;
    m[kNone]  = 2;
    m[kPart]  = 3;
    m[kFull2] = 99;  // sobreescribe kFull (misma key)
    reportTest("test9i: kFull2 sobreescribe kFull en mapa",
               m[kFull] == 99 && m.size() == 3);
}

// ============================================================
// TEST 10 - printKey() no causa crashes
// ============================================================
void test10_printKey() {
    std::cout << "\n--- TEST 10: printKey ---\n";

    std::cout << "\n[StateKey: estado inicial, todo gold]\n";
    StateKey(makeBaseState()).printKey();

    // Estado con oro en cubo y cellGold parcial
    Cube c = makeFreshCube();
    c.putGoldOnBottom();   // cara 1
    c.rollNorth();
    c.putGoldOnBottom();   // cara 2
    State s(4, 1, c, (1ULL<<2)|(1ULL<<3)|(1ULL<<4)|(1ULL<<5));

    std::cout << "\n[StateKey: 2 caras con oro, 4 celdas con oro]\n";
    StateKey(s).printKey();

    reportTest("test10a: printKey sin oro en cubo -> sin errores",  true);
    reportTest("test10b: printKey con oro en cubo -> sin errores",  true);
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
    test6_diferenteCellGold();
    test7_ordenTotalEstricto();
    test8_stdMap();
    test9_cellGoldDistingue();
    test10_printKey();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
