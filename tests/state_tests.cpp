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
Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// Bitmask con los 6 primeros bits encendidos (bits 0-5).
// Representa 6 celdas con oro en posiciones arbitrarias para tests de State.
static constexpr uint64_t ALL6 = 0x3FULL;

// Estado inicial tipico:
//   posicion (0,0), cubo sin oro, 6 celdas con oro (bits 0-5).
State makeInitialState() {
    return State(0, 0, makeFreshCube(), ALL6);
}

// Cubo donde las 6 caras fisicas tienen oro.
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
// campos: row, col, orientacion del cubo, cellGold.
// ============================================================
void test1_creacionCorrecta() {
    std::cout << "--- TEST 1: Creacion correcta ---\n";

    // bits 0, 2, 4 encendidos (patron alternado, 3 celdas con oro)
    uint64_t goldPattern = (1ULL<<0) | (1ULL<<2) | (1ULL<<4);
    State s(2, 5, Cube(0, 1, 2, 3, 4, 5), goldPattern);

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
    reportTest("test1i: cellGold bit 0 encendido",
               (s.cellGold & (1ULL<<0)) != 0);
    reportTest("test1j: cellGold bit 1 apagado",
               (s.cellGold & (1ULL<<1)) == 0);
    reportTest("test1k: cellGold bit 2 encendido",
               (s.cellGold & (1ULL<<2)) != 0);
    reportTest("test1l: cubo recien creado no tiene oro en ninguna cara",
               s.cube.countGoldFaces() == 0);
    reportTest("test1m: faceHasGold(0) == false en cubo sin oro",
               s.cube.faceHasGold(0) == false);
}

// ============================================================
// TEST 2 - cellGoldCount
//
// Verifica que el conteo de celdas con oro sea correcto para
// distintas configuraciones de cellGold (bitmask).
// ============================================================
void test2_cellGoldCount() {
    std::cout << "\n--- TEST 2: cellGoldCount ---\n";

    Cube c = makeFreshCube();

    State s6(0, 0, c, 0x3FULL);   // bits 0-5: 6 celdas
    reportTest("test2a: 6 bits encendidos -> count == 6",
               s6.cellGoldCount() == 6);

    State s0(0, 0, c, 0ULL);      // ningun bit
    reportTest("test2b: 0 bits encendidos -> count == 0",
               s0.cellGoldCount() == 0);

    // bits 0, 2, 4 encendidos: 3 celdas
    State s3(0, 0, c, (1ULL<<0) | (1ULL<<2) | (1ULL<<4));
    reportTest("test2c: 3 bits alternados -> count == 3",
               s3.cellGoldCount() == 3);

    State s1last(0, 0, c, 1ULL<<5);   // solo bit 5
    reportTest("test2d: bit 5 solo -> count == 1",
               s1last.cellGoldCount() == 1);

    State s1first(0, 0, c, 1ULL<<0);  // solo bit 0
    reportTest("test2e: bit 0 solo -> count == 1",
               s1first.cellGoldCount() == 1);

    // Bit alto: posicion (7,7) en grilla de 8 columnas = bit 63
    State sHigh(0, 0, c, 1ULL<<63);
    reportTest("test2f: bit 63 encendido -> count == 1",
               sHigh.cellGoldCount() == 1);
}

// ============================================================
// TEST 3 - allGoldCollected
//
// Verifica que allGoldCollected() retorne true solo cuando
// las 6 caras fisicas del cubo tienen oro simultaneamente.
// El cellGold no afecta este criterio (es el objetivo: cara del cubo).
// ============================================================
void test3_allGoldCollected() {
    std::cout << "\n--- TEST 3: allGoldCollected ---\n";

    State s = makeInitialState();
    reportTest("test3a: estado inicial -> false",
               !s.allGoldCollected());

    s.cube.putGoldOnBottom();
    reportTest("test3b: 1 cara con oro -> false",
               !s.allGoldCollected());

    State sAll(0, 0, makeCubeWithAllGold(), 0ULL);
    reportTest("test3c: 6 caras con oro -> true",
               sAll.allGoldCollected());
    reportTest("test3d: cube.countGoldFaces() == 6",
               sAll.cube.countGoldFaces() == 6);

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

    State s1 = makeInitialState();
    State s2 = makeInitialState();

    reportTest("test4a: estados identicos -> ==",
               s1 == s2);
    reportTest("test4b: estados identicos -> no !=",
               !(s1 != s2));

    // Diferente row
    State sDiffRow(1, 0, makeFreshCube(), ALL6);
    reportTest("test4c: diferente row -> !=",
               s1 != sDiffRow);
    reportTest("test4d: diferente row -> no ==",
               !(s1 == sDiffRow));

    // Diferente col
    State sDiffCol(0, 3, makeFreshCube(), ALL6);
    reportTest("test4e: diferente col -> !=",
               s1 != sDiffCol);

    // Diferente orientacion del cubo
    State sDiffOrient(0, 0, Cube(1, 0, 2, 3, 4, 5), ALL6);
    reportTest("test4f: diferente orientacion del cubo -> !=",
               s1 != sDiffOrient);

    // Diferente oro en caras del cubo
    Cube cWithGold = makeFreshCube();
    cWithGold.putGoldOnBottom();
    State sDiffGold(0, 0, cWithGold, ALL6);
    reportTest("test4g: diferente oro en caras del cubo -> !=",
               s1 != sDiffGold);

    // Diferente cellGold: bit 0 apagado
    State sDiffCell(0, 0, makeFreshCube(), ALL6 & ~1ULL);
    reportTest("test4h: diferente cellGold -> !=",
               s1 != sDiffCell);

    // Copia independiente (value semantics)
    State s3 = s1;
    s3.row = 7;
    reportTest("test4i: modificar copia no afecta original",
               s1.row == 0 && s3.row == 7);
    reportTest("test4j: copia modificada es distinta del original",
               s1 != s3);

    // Cubo rotado
    Cube rotated = makeFreshCube();
    rotated.rollNorth();
    State sRotated(0, 0, rotated, ALL6);
    reportTest("test4k: cubo rotado -> distinto aunque misma posicion",
               s1 != sRotated);
}

// ============================================================
// TEST 5 - Persistencia correcta del bitmask cellGold
//
// Verifica que los bits del bitmask se lean correctamente
// y no se confundan entre posiciones.
// ============================================================
void test5_persistenciaBitmask() {
    std::cout << "\n--- TEST 5: Persistencia del bitmask cellGold ---\n";

    Cube c = makeFreshCube();

    // Solo un bit encendido en posiciones especificas
    for (int bit = 0; bit < 8; ++bit) {
        State s(0, 0, c, 1ULL << bit);
        bool onlyBitSet = (s.cellGold == (1ULL << bit));
        bool countIsOne = (s.cellGoldCount() == 1);
        if (!onlyBitSet || !countIsOne) {
            reportTest("test5a: bit " + std::to_string(bit) + " persistido correctamente",
                       false);
            return;
        }
    }
    reportTest("test5a: bits 0-7 persistidos individualmente sin interferencia",
               true);

    // Combinacion de bits
    uint64_t mask = (1ULL<<3) | (1ULL<<7) | (1ULL<<15);
    State sCombo(0, 0, c, mask);
    reportTest("test5b: combinacion de bits 3,7,15 -> count == 3",
               sCombo.cellGoldCount() == 3);
    reportTest("test5c: bit 3 encendido",  (sCombo.cellGold & (1ULL<<3))  != 0);
    reportTest("test5d: bit 7 encendido",  (sCombo.cellGold & (1ULL<<7))  != 0);
    reportTest("test5e: bit 15 encendido", (sCombo.cellGold & (1ULL<<15)) != 0);
    reportTest("test5f: bit 0 apagado",    (sCombo.cellGold & (1ULL<<0))  == 0);
    reportTest("test5g: bit 14 apagado",   (sCombo.cellGold & (1ULL<<14)) == 0);

    // Modificar cellGold directamente (los tests de transicion lo hacen via copia)
    State s2(0, 0, c, mask);
    s2.cellGold |= (1ULL<<0);    // encender bit 0
    s2.cellGold &= ~(1ULL<<7);   // apagar bit 7
    reportTest("test5h: encender bit 0 y apagar bit 7 -> count sigue en 3",
               s2.cellGoldCount() == 3);

    // Confirmar: 3,15 siguen; 0 nuevo; 7 eliminado
    reportTest("test5i: bit 0 encendido tras OR",   (s2.cellGold & 1ULL)      != 0);
    reportTest("test5j: bit 7 apagado tras AND-NOT", (s2.cellGold & (1ULL<<7)) == 0);
    reportTest("test5k: bit 3 intacto",              (s2.cellGold & (1ULL<<3)) != 0);
}

// ============================================================
// TEST 6 - printState
//
// Verifica que printState() no cause crashes.
// ============================================================
void test6_printState() {
    std::cout << "\n--- TEST 6: printState ---\n";

    std::cout << "\n[estado inicial]\n";
    makeInitialState().printState();

    Cube c(0, 1, 2, 3, 4, 5);
    c.putGoldOnBottom();   // cara 1
    c.rollNorth();
    c.putGoldOnBottom();   // cara 2

    // bits 2,3,4,5 encendidos (4 celdas con oro)
    std::cout << "\n[2 caras con oro, posicion (3,4), 4 celdas con oro]\n";
    State s2(3, 4, c, (1ULL<<2)|(1ULL<<3)|(1ULL<<4)|(1ULL<<5));
    s2.printState();

    std::cout << "\n[objetivo logrado: 6 caras con oro]\n";
    State sGoal(7, 7, makeCubeWithAllGold(), 0ULL);
    sGoal.printState();

    reportTest("test6: printState ejecuto sin errores en 3 configuraciones",
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
    test2_cellGoldCount();
    test3_allGoldCollected();
    test4_comparacion();
    test5_persistenciaBitmask();
    test6_printState();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
