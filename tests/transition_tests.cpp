// ============================================================
// transition_tests.cpp
// Pruebas unitarias para el modulo de transiciones.
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cube/Cube.cpp state/State.cpp transition/Transition.cpp tests/transition_tests.cpp -o transition_tests
//   ./transition_tests
// ============================================================

#include <iostream>
#include <string>
#include <set>
#include "../transition/Transition.h"

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
// Constantes del layout de test
//
// Grilla conceptual de COLS=5 columnas.
// Celdas de oro en: (0,0), (0,4), (2,0), (2,4), (4,2), (4,4)
// ============================================================
static const int COLS = 5;

// Bitmask inicial con el oro en las 6 posiciones canonicas.
// bit (row * COLS + col) encendido si esa celda tiene oro.
static uint64_t makeInitCellGold() {
    return (1ULL << (0*COLS+0)) | (1ULL << (0*COLS+4)) |
           (1ULL << (2*COLS+0)) | (1ULL << (2*COLS+4)) |
           (1ULL << (4*COLS+2)) | (1ULL << (4*COLS+4));
}

// Consultar si la celda (row, col) tiene oro en el estado dado.
static bool cellHasGold(const State& s, int row, int col) {
    return (s.cellGold & (1ULL << (row * COLS + col))) != 0;
}

// ============================================================
// Helpers de construccion
// ============================================================

Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// Estado de partida: cubo en (2,2), sin oro en caras,
// todas las celdas de oro intactas.
// (2,2) no es celda de oro: no hay interaccion inmediata.
State makeStartState() {
    return State(2, 2, makeFreshCube(), makeInitCellGold());
}

// Verifica que los 6 face IDs del cubo sean distintos y en [0,5].
bool isValidOrientation(const Cube& c) {
    int ids[6] = { c.getTop(), c.getBottom(), c.getNorth(),
                   c.getSouth(), c.getEast(), c.getWest() };
    std::set<int> uniq(ids, ids + 6);
    if (uniq.size() != 6) return false;
    for (int id : ids) {
        if (id < 0 || id > 5) return false;
    }
    return true;
}

// Invariante de conservacion de oro:
//   caras_del_cubo_con_oro + celdas_con_oro == totalGold
// Incluye CASO3: gold depositado en celda cuenta en cellGoldCount.
bool goldConserved(const State& s, int totalGold = 6) {
    return s.cube.countGoldFaces() + s.cellGoldCount() == totalGold;
}

// ============================================================
// TEST 1 - El movimiento cambia la posicion correctamente
//
// Desde (2,2) sin celdas de oro en destino:
//   moveNorth -> (1,2)
//   moveSouth -> (3,2)
//   moveEast  -> (2,3)
//   moveWest  -> (2,1)
//
// Ninguna de estas posiciones es celda de oro, por lo que
// solo se verifica el cambio de posicion.
// ============================================================
void test1_posicion() {
    std::cout << "--- TEST 1: Cambio de posicion ---\n";

    State s = makeStartState();  // (2,2)

    State sN = moveNorth(s, COLS).nextState;
    reportTest("test1a: moveNorth -> row=1",
               sN.row == 1 && sN.col == 2);

    State sS = moveSouth(s, COLS).nextState;
    reportTest("test1b: moveSouth -> row=3",
               sS.row == 3 && sS.col == 2);

    State sE = moveEast(s, COLS).nextState;
    reportTest("test1c: moveEast -> col=3",
               sE.row == 2 && sE.col == 3);

    State sW = moveWest(s, COLS).nextState;
    reportTest("test1d: moveWest -> col=1",
               sW.row == 2 && sW.col == 1);

    reportTest("test1e: estado original sin cambios (row)",
               s.row == 2);
    reportTest("test1f: estado original sin cambios (col)",
               s.col == 2);
}

// ============================================================
// TEST 2 - El movimiento rota el cubo correctamente
//
// Cubo inicial: top=0, bottom=1, north=2, south=3, east=4, west=5
//
// Rotaciones esperadas (trazadas manualmente):
//   rollNorth: top=3, bottom=2, north=0, south=1, east=4, west=5
//   rollSouth: top=2, bottom=3, north=1, south=0, east=4, west=5
//   rollEast:  top=5, bottom=4, north=2, south=3, east=0, west=1
//   rollWest:  top=4, bottom=5, north=2, south=3, east=1, west=0
// ============================================================
void test2_rotacion() {
    std::cout << "\n--- TEST 2: Rotacion correcta del cubo ---\n";

    State s = makeStartState();  // Cube(0,1,2,3,4,5) en (2,2)

    State sN = moveNorth(s, COLS).nextState;
    reportTest("test2a: moveNorth -> bottom=2",
               sN.cube.getBottom() == 2);
    reportTest("test2b: moveNorth -> top=3",
               sN.cube.getTop() == 3);
    reportTest("test2c: moveNorth -> north=0, south=1",
               sN.cube.getNorth() == 0 && sN.cube.getSouth() == 1);

    State sS = moveSouth(s, COLS).nextState;
    reportTest("test2d: moveSouth -> bottom=3",
               sS.cube.getBottom() == 3);
    reportTest("test2e: moveSouth -> top=2",
               sS.cube.getTop() == 2);

    State sE = moveEast(s, COLS).nextState;
    reportTest("test2f: moveEast -> bottom=4",
               sE.cube.getBottom() == 4);
    reportTest("test2g: moveEast -> top=5",
               sE.cube.getTop() == 5);

    State sW = moveWest(s, COLS).nextState;
    reportTest("test2h: moveWest -> bottom=5",
               sW.cube.getBottom() == 5);
    reportTest("test2i: moveWest -> top=4",
               sW.cube.getTop() == 4);
}

// ============================================================
// TEST 3 - CASO 1: pickup cuando bottom NO tiene oro
//
// Cubo fresco en (2,1). Gold cell en (2,0).
// moveWest -> (2,0): rollWest da bottom=face 5 (sin oro).
//   -> CASO 1: cubo recoge oro de la celda.
//   -> face 5 tiene oro.
//   -> celda (2,0) queda sin oro (bit apagado).
//   -> recogioOro == true.
// ============================================================
void test3_caso1_pickup() {
    std::cout << "\n--- TEST 3: CASO 1 - pickup ---\n";

    State s(2, 1, makeFreshCube(), makeInitCellGold());

    TransitionResult tr = moveWest(s, COLS);
    const State& after = tr.nextState;

    reportTest("test3a: llega a (2,0)",
               after.row == 2 && after.col == 0);

    // Tras rollWest: bottom = face 5
    reportTest("test3b: bottom es cara 5 tras rollWest",
               after.cube.getBottom() == 5);

    // La cara que quedo abajo recoge el oro
    reportTest("test3c: cara 5 tiene oro (bottomHasGold)",
               after.cube.bottomHasGold());
    reportTest("test3d: faceHasGold(5) == true",
               after.cube.faceHasGold(5));

    // La celda perdio su oro
    reportTest("test3e: celda (2,0) ya no tiene oro (CASO 1)",
               !cellHasGold(after, 2, 0));

    // Las otras 5 celdas de oro siguen intactas
    reportTest("test3f: celda (0,0) sigue con oro",  cellHasGold(after, 0, 0));
    reportTest("test3g: celda (0,4) sigue con oro",  cellHasGold(after, 0, 4));
    reportTest("test3h: celda (2,4) sigue con oro",  cellHasGold(after, 2, 4));
    reportTest("test3i: celda (4,2) sigue con oro",  cellHasGold(after, 4, 2));
    reportTest("test3j: celda (4,4) sigue con oro",  cellHasGold(after, 4, 4));

    reportTest("test3k: countGoldFaces == 1",
               after.cube.countGoldFaces() == 1);
    reportTest("test3l: recogioOro == true (CASO 1)",
               tr.recogioOro);
}

// ============================================================
// TEST 4 - CASO 2: swap cuando bottom YA tiene oro
//
// Cubo en (2,1) con face 5 pre-cargada con oro.
// Gold cell (2,0) tiene oro.
// moveWest -> (2,0): rollWest da bottom=face 5 (CON oro).
//   -> CASO 2: intercambio. El estado visible no cambia.
//   -> celda (2,0) sigue con oro.
//   -> face 5 sigue con oro.
//   -> recogioOro == false (costo A).
// ============================================================
void test4_caso2_swap() {
    std::cout << "\n--- TEST 4: CASO 2 - swap ---\n";

    // Pre-cargar oro en face 5 sin cambiar la orientacion final
    Cube c = makeFreshCube();   // top=0, bottom=1, ..., west=5
    c.rollWest();               // bottom = face 5
    c.putGoldOnBottom();        // face 5 tiene oro
    c.rollEast();               // deshace rollWest; face 5 sigue con oro

    State s(2, 1, c, makeInitCellGold());

    reportTest("test4a: precondicion - face 5 tiene oro",
               s.cube.faceHasGold(5));
    reportTest("test4b: precondicion - celda (2,0) tiene oro",
               cellHasGold(s, 2, 0));

    TransitionResult tr = moveWest(s, COLS);
    const State& after = tr.nextState;

    // CASO 2: celda sigue con oro, cara sigue con oro
    reportTest("test4c: celda (2,0) conserva su oro (CASO 2)",
               cellHasGold(after, 2, 0));
    reportTest("test4d: face 5 sigue con oro tras swap",
               after.cube.faceHasGold(5));
    reportTest("test4e: countGoldFaces no cambia (sigue en 1)",
               after.cube.countGoldFaces() == 1);

    // Conservacion total: caras + celdas antes == despues
    reportTest("test4f: conservacion total invariante",
               after.cube.countGoldFaces() + after.cellGoldCount() ==
               s.cube.countGoldFaces()     + s.cellGoldCount());

    reportTest("test4g: recogioOro == false (CASO 2, costo A)",
               !tr.recogioOro);
}

// ============================================================
// TEST 5 - CASO 3: deposit cuando bottom tiene oro y celda esta vacia
//
// Cubo en (2,1) con face 5 cargada con oro.
// Celda (2,0) SIN oro (bit apagado artificialmente).
// moveWest -> (2,0): rollWest da bottom=face 5 (CON oro), celda vacia.
//   -> CASO 3: la cara deposita su oro en la celda.
//   -> celda (2,0) recibe oro (bit encendido).
//   -> face 5 pierde oro.
//   -> recogioOro == false (costo A).
// ============================================================
void test5_caso3_deposit() {
    std::cout << "\n--- TEST 5: CASO 3 - deposit ---\n";

    // Misma pre-carga de face 5 que en test4
    Cube c = makeFreshCube();
    c.rollWest();
    c.putGoldOnBottom();   // face 5 tiene oro
    c.rollEast();

    // Celda (2,0) sin oro: apagar bit 10 = 2*5+0
    uint64_t cellGoldSin20 = makeInitCellGold() & ~(1ULL << (2*COLS+0));

    State s(2, 1, c, cellGoldSin20);

    reportTest("test5a: precondicion - face 5 tiene oro",
               s.cube.faceHasGold(5));
    reportTest("test5b: precondicion - celda (2,0) sin oro",
               !cellHasGold(s, 2, 0));

    TransitionResult tr = moveWest(s, COLS);
    const State& after = tr.nextState;

    reportTest("test5c: llega a (2,0)",
               after.row == 2 && after.col == 0);

    // CASO 3: cara deposita oro en celda vacia
    reportTest("test5d: celda (2,0) recibio oro del cubo (CASO 3)",
               cellHasGold(after, 2, 0));
    reportTest("test5e: face 5 ya no tiene oro",
               !after.cube.faceHasGold(5));
    reportTest("test5f: countGoldFaces disminuyo en 1",
               after.cube.countGoldFaces() == s.cube.countGoldFaces() - 1);
    reportTest("test5g: cellGoldCount aumento en 1",
               after.cellGoldCount() == s.cellGoldCount() + 1);

    // Conservacion total: oro se movi o de cara a celda, total igual
    reportTest("test5h: conservacion total invariante (CASO 3)",
               after.cube.countGoldFaces() + after.cellGoldCount() ==
               s.cube.countGoldFaces()     + s.cellGoldCount());

    reportTest("test5i: recogioOro == false (CASO 3, costo A)",
               !tr.recogioOro);
}

// ============================================================
// TEST 6 - Conservacion del oro a traves de secuencia de movimientos
//
// Invariante: cube.countGoldFaces() + cellGoldCount() == 6 en todo momento.
//
// La secuencia pasa por CASO 1 en (2,0) y potencialmente CASO 3
// en pasos posteriores. El invariante se mantiene independientemente.
//
// Ruta desde (3,1):
//   moveWest  -> (3,0)  : celda sin oro
//   moveNorth -> (2,0)  : gold cell -> CASO 1 (bottom=face 2 recoge)
//   moveNorth -> (1,0)  : celda sin oro
//   moveEast  -> (1,1)  : celda sin oro
//   moveSouth -> (2,1)  : celda sin oro, PERO bottom es face 2 (con oro)
//                         -> CASO 3: face 2 deposita en (2,1)
// ============================================================
void test6_conservacionOro() {
    std::cout << "\n--- TEST 6: Conservacion del oro (CASO 1 + CASO 3) ---\n";

    State s(3, 1, makeFreshCube(), makeInitCellGold());

    reportTest("test6a: invariante en estado inicial",
               goldConserved(s));

    s = moveWest(s, COLS).nextState;     // (3,0): sin oro
    reportTest("test6b: invariante tras moveWest a (3,0)",
               goldConserved(s));

    // CASO 1 en (2,0)
    s = moveNorth(s, COLS).nextState;
    reportTest("test6c: invariante tras moveNorth a (2,0) [CASO 1]",
               goldConserved(s));
    reportTest("test6d: celda (2,0) ya no tiene oro",
               !cellHasGold(s, 2, 0));
    reportTest("test6e: cube tiene exactamente 1 cara con oro",
               s.cube.countGoldFaces() == 1);

    s = moveNorth(s, COLS).nextState;    // (1,0): sin oro
    reportTest("test6f: invariante tras moveNorth a (1,0)",
               goldConserved(s));

    s = moveEast(s, COLS).nextState;     // (1,1): sin oro
    reportTest("test6g: invariante tras moveEast a (1,1)",
               goldConserved(s));

    // (2,1) esta vacio y la cara con oro queda abajo -> CASO 3
    s = moveSouth(s, COLS).nextState;
    reportTest("test6h: invariante tras moveSouth a (2,1) [posible CASO 3]",
               goldConserved(s));
    reportTest("test6i: posicion final es (2,1)",
               s.row == 2 && s.col == 1);
    reportTest("test6j: invariante de conservacion en estado final",
               goldConserved(s));
}

// ============================================================
// TEST 7 - Invariantes en secuencia larga de movimientos
//
// A) Orientacion del cubo valida (6 IDs distintos, en [0,5])
// B) Conservacion del oro en cada paso
// C) Posicion final correcta
//
// La secuencia recorre el perimetro de una grilla 5x5 pasando
// por todas las celdas de oro.
// ============================================================
void test7_invariantesSecuencia() {
    std::cout << "\n--- TEST 7: Invariantes en secuencia larga ---\n";

    State s(1, 1, makeFreshCube(), makeInitCellGold());

    bool orientationOk = true;
    bool goldOk        = true;

    // 16 movimientos: un recorrido que visita (2,0),(4,2),(4,4),(2,4),(0,4),(0,0)
    using MoveFunc = TransitionResult(*)(const State&, int);
    MoveFunc moves[] = {
        moveWest, moveSouth, moveSouth, moveSouth,
        moveEast, moveEast,  moveEast,  moveEast,
        moveNorth,moveNorth, moveNorth, moveNorth,
        moveWest, moveWest,  moveWest,  moveWest
    };

    for (int i = 0; i < 16; ++i) {
        s = moves[i](s, COLS).nextState;
        if (!isValidOrientation(s.cube)) orientationOk = false;
        if (!goldConserved(s))           goldOk        = false;
    }

    reportTest("test7a: orientacion valida en todos los pasos",
               orientationOk);
    reportTest("test7b: oro conservado en todos los pasos",
               goldOk);
    reportTest("test7c: posicion final es (0,0)",
               s.row == 0 && s.col == 0);
    reportTest("test7d: conservacion en estado final",
               s.cube.countGoldFaces() + s.cellGoldCount() == 6);
}

// ============================================================
// TEST 8 - Los movimientos no modifican el estado original
//
// Verifica value semantics: cada funcion de movimiento trabaja
// sobre una copia interna y retorna el nuevo estado.
// El estado original debe quedar identico.
// ============================================================
void test8_valueSemantics() {
    std::cout << "\n--- TEST 8: Value semantics (original sin cambios) ---\n";

    State original(2, 1, makeFreshCube(), makeInitCellGold());

    // moveWest hacia (2,0): recoge oro (CASO 1)
    State afterN = moveNorth(original, COLS).nextState;
    State afterS = moveSouth(original, COLS).nextState;
    State afterE = moveEast(original, COLS).nextState;
    State afterW = moveWest(original, COLS).nextState;

    // El original NO debe haber cambiado
    reportTest("test8a: original.row sin cambios",
               original.row == 2);
    reportTest("test8b: original.col sin cambios",
               original.col == 1);
    reportTest("test8c: orientacion original intacta (bottom=1)",
               original.cube.getBottom() == 1);
    reportTest("test8d: original sin oro en caras (countGoldFaces=0)",
               original.cube.countGoldFaces() == 0);
    reportTest("test8e: original.cellGold intacto",
               original.cellGold == makeInitCellGold());

    // afterW si recoge oro en (2,0)
    reportTest("test8f: afterW tiene oro en cara 5",
               afterW.cube.faceHasGold(5));
    reportTest("test8g: afterW: celda (2,0) sin oro (CASO 1)",
               !cellHasGold(afterW, 2, 0));

    // Los demas movimientos funcionaron
    reportTest("test8h: afterN.row == 1", afterN.row == 1);
    reportTest("test8i: afterS.row == 3", afterS.row == 3);
    reportTest("test8j: afterE.col == 2", afterE.col == 2);
    reportTest("test8k: afterW.col == 0", afterW.col == 0);
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "===========================================\n";
    std::cout << "  Transition Tests\n";
    std::cout << "===========================================\n";

    test1_posicion();
    test2_rotacion();
    test3_caso1_pickup();
    test4_caso2_swap();
    test5_caso3_deposit();
    test6_conservacionOro();
    test7_invariantesSecuencia();
    test8_valueSemantics();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
