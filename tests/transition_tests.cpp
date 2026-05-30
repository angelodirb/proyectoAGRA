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
#include <array>
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
// Helpers de construccion
// ============================================================

// Cubo estandar sin oro.
//   top=0, bottom=1, north=2, south=3, east=4, west=5
Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// Layout de celdas de oro usado en todos los tests.
//
//   Indice | (row, col)
//   -------|----------
//     0    |  (0, 0)
//     1    |  (0, 4)
//     2    |  (2, 0)
//     3    |  (2, 4)
//     4    |  (4, 2)
//     5    |  (4, 4)
//
// Se elige este layout para que las tests puedan planear
// movimientos predecibles hacia celdas de oro especificas.
GoldCells makeTestGoldCells() {
    return {{{0,0}, {0,4}, {2,0}, {2,4}, {4,2}, {4,4}}};
}

// Estado de partida: cubo en (2,2), sin oro en caras,
// todas las celdas de oro intactas.
// (2,2) no es celda de oro: no hay interaccion inmediata.
State makeStartState() {
    std::array<bool,6> allRG = {true, true, true, true, true, true};
    return State(2, 2, makeFreshCube(), allRG);
}

// Verifica que los 6 face IDs del cubo sean distintos y en [0,5].
// Util para detectar corrupcion de orientacion tras movimientos.
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

// Comprueba el invariante de conservacion de oro.
// En cada instante: caras_del_cubo_con_oro + celdas_con_oro == 6.
bool goldConserved(const State& s, int totalGold = 6) {
    return s.cube.countGoldFaces() + s.remainingGoldCount() == totalGold;
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

    GoldCells gc = makeTestGoldCells();
    State s = makeStartState();  // posicion (2,2)

    State sN = moveNorth(s, gc);
    reportTest("test1a: moveNorth -> row=1",
               sN.row == 1 && sN.col == 2);

    State sS = moveSouth(s, gc);
    reportTest("test1b: moveSouth -> row=3",
               sS.row == 3 && sS.col == 2);

    State sE = moveEast(s, gc);
    reportTest("test1c: moveEast -> col=3",
               sE.row == 2 && sE.col == 3);

    State sW = moveWest(s, gc);
    reportTest("test1d: moveWest -> col=1",
               sW.row == 2 && sW.col == 1);

    // Verificar que el estado original no fue modificado
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

    GoldCells gc = makeTestGoldCells();
    State s = makeStartState();  // Cube(0,1,2,3,4,5) en (2,2)

    // moveNorth -> rollNorth
    // rollNorth: nuevo_top=sur=3, norte=top=0, bottom=norte=2, sur=bottom=1
    State sN = moveNorth(s, gc);
    reportTest("test2a: moveNorth -> bottom=2",
               sN.cube.getBottom() == 2);
    reportTest("test2b: moveNorth -> top=3",
               sN.cube.getTop() == 3);
    reportTest("test2c: moveNorth -> north=0, south=1",
               sN.cube.getNorth() == 0 && sN.cube.getSouth() == 1);

    // moveSouth -> rollSouth
    // rollSouth: nuevo_top=norte=2, sur=top=0, bottom=sur=3, norte=bottom=1
    State sS = moveSouth(s, gc);
    reportTest("test2d: moveSouth -> bottom=3",
               sS.cube.getBottom() == 3);
    reportTest("test2e: moveSouth -> top=2",
               sS.cube.getTop() == 2);

    // moveEast -> rollEast
    // rollEast: nuevo_top=oeste=5, este=top=0, bottom=este=4, oeste=bottom=1
    State sE = moveEast(s, gc);
    reportTest("test2f: moveEast -> bottom=4",
               sE.cube.getBottom() == 4);
    reportTest("test2g: moveEast -> top=5",
               sE.cube.getTop() == 5);

    // moveWest -> rollWest
    // rollWest: nuevo_top=este=4, oeste=top=0, bottom=oeste=5, este=bottom=1
    State sW = moveWest(s, gc);
    reportTest("test2h: moveWest -> bottom=5",
               sW.cube.getBottom() == 5);
    reportTest("test2i: moveWest -> top=4",
               sW.cube.getTop() == 4);
}

// ============================================================
// TEST 3 - CASO 1: recoger oro cuando bottom NO tiene oro
//
// Setup:
//   Cubo fresco en (2,1). Gold cell index 2 en (2,0).
//   moveWest -> (2,0) = celda de oro.
//
// Tras rollWest: bottom = cara 5 (face 5 no tiene oro).
//   -> CASO 1: cubo recoge oro de la celda.
//   -> face 5 debe tener oro.
//   -> remainingGold[2] debe ser false.
// ============================================================
void test3_recogerOro() {
    std::cout << "\n--- TEST 3: CASO 1 - recoger oro ---\n";

    GoldCells gc = makeTestGoldCells();
    // Inicio en (2,1): un paso al oeste hay oro en (2,0) = gold[2]
    std::array<bool,6> allRG = {true, true, true, true, true, true};
    State s(2, 1, makeFreshCube(), allRG);

    State after = moveWest(s, gc);

    // Verificar posicion
    reportTest("test3a: llega a (2,0)",
               after.row == 2 && after.col == 0);

    // Tras rollWest: bottom = face 5
    reportTest("test3b: bottom es cara 5 tras rollWest",
               after.cube.getBottom() == 5);

    // La cara que quedo abajo debe haber recogido el oro
    reportTest("test3c: cara 5 tiene oro (bottomHasGold)",
               after.cube.bottomHasGold());

    reportTest("test3d: faceHasGold(5) == true",
               after.cube.faceHasGold(5));

    // La celda perdio su oro
    reportTest("test3e: remainingGold[2] == false (celda recogida)",
               !after.remainingGold[2]);

    // El resto de las celdas sigue intacto
    reportTest("test3f: el resto de celdas siguen con oro",
               after.remainingGold[0] && after.remainingGold[1] &&
               after.remainingGold[3] && after.remainingGold[4] &&
               after.remainingGold[5]);

    // Exactamente 1 cara del cubo tiene oro
    reportTest("test3g: countGoldFaces == 1",
               after.cube.countGoldFaces() == 1);
}

// ============================================================
// TEST 4 - CASO 2: intercambio cuando bottom YA tiene oro
//
// Setup:
//   Cubo en (2,1) con gold cell index 2 en (2,0).
//   La cara 5 (que quedara abajo tras rollWest) ya tiene oro.
//
//   Para pre-cargar oro en cara 5 sin mover el cubo de posicion:
//     rollWest  -> bottom = cara 5
//     putGoldOnBottom -> cara 5 tiene oro
//     rollEast  -> deshace la rotacion (cara 5 sigue con oro)
//
//   remainingGold[2] = true (la celda tiene oro).
//   -> CASO 2: intercambio. El estado booleano no cambia.
// ============================================================
void test4_intercambio() {
    std::cout << "\n--- TEST 4: CASO 2 - intercambio ---\n";

    GoldCells gc = makeTestGoldCells();

    // Pre-cargar oro en cara 5 sin modificar la orientacion final
    Cube c = makeFreshCube();     // top=0, bottom=1, ..., west=5
    c.rollWest();                 // bottom = cara 5
    c.putGoldOnBottom();          // cara 5 tiene oro
    c.rollEast();                 // deshace rollWest (orientacion original, cara 5 sigue con oro)

    // Verificar pre-condicion del setup
    // (rollEast es la inversa de rollWest: volvemos al Cube(0,1,2,3,4,5))
    // Cara 5 debe tener oro pero estar en posicion "west"
    // remainingGold[2] = true: la celda (2,0) aun tiene oro
    std::array<bool,6> rg = {true, true, true, true, true, true};
    State s(2, 1, c, rg);

    // Precondicion: cara 5 ya tiene oro
    reportTest("test4a: precondicion - cara 5 tiene oro antes del movimiento",
               s.cube.faceHasGold(5));
    reportTest("test4b: precondicion - celda (2,0) tiene oro",
               s.remainingGold[2]);

    State after = moveWest(s, gc);

    // Tras rollWest: bottom = cara 5 (que ya tiene oro) -> CASO 2

    // La celda SIGUE teniendo oro (intercambio, no se consume)
    reportTest("test4c: remainingGold[2] sigue true (celda conserva oro)",
               after.remainingGold[2]);

    // La cara del cubo SIGUE teniendo oro
    reportTest("test4d: cara 5 sigue con oro tras intercambio",
               after.cube.faceHasGold(5));

    // La cantidad total de caras con oro no cambia
    reportTest("test4e: countGoldFaces no cambia (sigue en 1)",
               after.cube.countGoldFaces() == 1);

    // Oro total del sistema = caras_cubo + celdas_con_oro
    // Antes: 1 (cara 5) + 6 (todas las celdas) = 7
    // Despues: 1 + 6 = 7 (sin cambio)
    reportTest("test4f: oro total del sistema no cambia",
               after.cube.countGoldFaces() + after.remainingGoldCount() ==
               s.cube.countGoldFaces()     + s.remainingGoldCount());
}

// ============================================================
// TEST 5 - El oro total se conserva en todo momento
//
// Invariante: cube.countGoldFaces() + remainingGoldCount() == 6
//
// Se verifica paso a paso en una secuencia de movimientos
// que incluye celdas sin oro, CASO 1 y verificacion continua.
//
// Ruta (desde (3,1), gold cells en (2,0) y (4,2)):
//   moveWest  -> (3,0)  : no es celda de oro
//   moveNorth -> (2,0)  : gold[2] -> CASO 1 (bottom = cara 2)
//   moveNorth -> (1,0)  : no es celda de oro
//   moveEast  -> (1,1)  : no es celda de oro
//   moveSouth -> (2,1)  : no es celda de oro
//
// Traza de la cara bottom en cada paso:
//   inicial          : bottom=1
//   tras rollWest    : bottom=5
//   tras rollNorth   : bottom=2   <- recoge en (2,0)
//   tras rollNorth   : bottom=4
//   tras rollEast    : bottom=1
//   tras rollSouth   : bottom=4
// ============================================================
void test5_conservacionOro() {
    std::cout << "\n--- TEST 5: Conservacion del oro total ---\n";

    GoldCells gc = makeTestGoldCells();
    std::array<bool,6> allRG = {true, true, true, true, true, true};
    State s(3, 1, makeFreshCube(), allRG);

    const int TOTAL = 6;
    reportTest("test5a: invariante en estado inicial",
               goldConserved(s, TOTAL));

    s = moveWest(s, gc);   // (3,0): no oro
    reportTest("test5b: invariante tras moveWest a (3,0)",
               goldConserved(s, TOTAL));

    // (2,0) = gold[2] -> CASO 1: cara bottom recoge oro
    s = moveNorth(s, gc);
    reportTest("test5c: invariante tras moveNorth a (2,0) [CASO 1]",
               goldConserved(s, TOTAL));
    reportTest("test5d: celda (2,0) ya recogida",
               !s.remainingGold[2]);
    reportTest("test5e: cube ahora tiene 1 cara con oro",
               s.cube.countGoldFaces() == 1);

    s = moveNorth(s, gc);  // (1,0): no oro
    reportTest("test5f: invariante tras moveNorth a (1,0)",
               goldConserved(s, TOTAL));

    s = moveEast(s, gc);   // (1,1): no oro
    reportTest("test5g: invariante tras moveEast a (1,1)",
               goldConserved(s, TOTAL));

    s = moveSouth(s, gc);  // (2,1): no oro
    reportTest("test5h: invariante tras moveSouth a (2,1)",
               goldConserved(s, TOTAL));

    // Confirmar estado final coherente
    reportTest("test5i: posicion final es (2,1)",
               s.row == 2 && s.col == 1);
    reportTest("test5j: countGoldFaces sigue siendo 1",
               s.cube.countGoldFaces() == 1);
    reportTest("test5k: remainingGoldCount es 5",
               s.remainingGoldCount() == 5);
}

// ============================================================
// TEST 6 - Secuencias de movimientos mantienen invariantes
//
// Se aplica una secuencia larga de movimientos y se verifica
// en cada paso:
//   A) orientacion del cubo valida (6 IDs distintos, en [0,5])
//   B) conservacion del oro
//   C) que la posicion es exactamente la esperada
//
// La secuencia da una vuelta alrededor del centro de la grilla
// y pasa por varias celdas de oro.
// ============================================================
void test6_invariantesSecuencia() {
    std::cout << "\n--- TEST 6: Invariantes en secuencia de movimientos ---\n";

    GoldCells gc = makeTestGoldCells();
    std::array<bool,6> allRG = {true, true, true, true, true, true};
    State s(1, 1, makeFreshCube(), allRG);

    const int TOTAL = 6;
    bool orientationOk = true;
    bool goldOk        = true;

    // Secuencia: se aplican 16 movimientos que forman un recorrido
    // pasando por celdas de oro en (2,0), (2,4), (4,2), (4,4).
    //
    // Paso a paso:
    //   (1,1) ->W-> (1,0) ->S-> (2,0)[g2] ->S-> (3,0) ->S-> (4,0)
    //         ->E-> (4,1) ->E-> (4,2)[g4] ->E-> (4,3) ->E-> (4,4)[g5]
    //         ->N-> (3,4) ->N-> (2,4)[g3] ->N-> (1,4) ->N-> (0,4)[g1]
    //         ->W-> (0,3) ->W-> (0,2) ->W-> (0,1) ->W-> (0,0)[g0]

    using MoveFunc = State(*)(const State&, const GoldCells&);
    MoveFunc moves[] = {
        moveWest, moveSouth, moveSouth, moveSouth,
        moveEast, moveEast,  moveEast,  moveEast,
        moveNorth,moveNorth, moveNorth, moveNorth,
        moveWest, moveWest,  moveWest,  moveWest
    };

    for (int i = 0; i < 16; ++i) {
        s = moves[i](s, gc);
        if (!isValidOrientation(s.cube)) orientationOk = false;
        if (!goldConserved(s, TOTAL))    goldOk        = false;
    }

    reportTest("test6a: orientacion valida en todos los pasos",
               orientationOk);
    reportTest("test6b: oro conservado en todos los pasos",
               goldOk);

    // Al final del recorrido estamos en (0,0) = gold[0]
    // (0,0) fue la ultima celda visitada -> recogida
    reportTest("test6c: posicion final es (0,0)",
               s.row == 0 && s.col == 0);

    // Se visitaron gold[2]:(2,0), gold[4]:(4,2), gold[5]:(4,4),
    // gold[3]:(2,4), gold[1]:(0,4), gold[0]:(0,0)
    // (dependiendo de si la cara bottom tenia oro o no en cada visita)
    // La conservacion garantiza que el resultado es coherente.
    reportTest("test6d: countGoldFaces + remainingGoldCount == 6",
               s.cube.countGoldFaces() + s.remainingGoldCount() == TOTAL);
}

// ============================================================
// TEST 7 - Los movimientos no modifican el estado original
//
// Verifica value semantics: cada funcion de movimiento trabaja
// sobre una copia interna y retorna el nuevo estado.
// El estado pasado como parametro debe quedar identico.
// ============================================================
void test7_valueSemantics() {
    std::cout << "\n--- TEST 7: Value semantics (original sin cambios) ---\n";

    GoldCells gc = makeTestGoldCells();

    // Setup: cubo en (2,1), un paso al oeste hay gold en (2,0)
    std::array<bool,6> allRG = {true, true, true, true, true, true};
    State original(2, 1, makeFreshCube(), allRG);

    // Aplicar todos los movimientos usando el estado original
    State afterN = moveNorth(original, gc);
    State afterS = moveSouth(original, gc);
    State afterE = moveEast(original, gc);
    State afterW = moveWest(original, gc);  // este recoge oro en (2,0)

    // afterW recogio oro: la cara 5 tiene oro y remainingGold[2]=false
    // Pero el original NO debe haber cambiado

    reportTest("test7a: original.row sin cambios",
               original.row == 2);
    reportTest("test7b: original.col sin cambios",
               original.col == 1);
    reportTest("test7c: orientacion original intacta (bottom=1)",
               original.cube.getBottom() == 1);
    reportTest("test7d: original sin oro en caras (countGoldFaces=0)",
               original.cube.countGoldFaces() == 0);
    reportTest("test7e: original.remainingGold[2] intacto (true)",
               original.remainingGold[2]);
    reportTest("test7f: original.remainingGoldCount() == 6",
               original.remainingGoldCount() == 6);

    // Verificar que el resultado de moveWest SÍ cambio
    reportTest("test7g: afterW tiene oro en cara 5 (recogio)",
               afterW.cube.faceHasGold(5));
    reportTest("test7h: afterW.remainingGold[2] == false",
               !afterW.remainingGold[2]);

    // Verificar que los otros movimientos tampoco modificaron el original
    reportTest("test7i: afterN.row == 1 (moveNorth funciono)",
               afterN.row == 1);
    reportTest("test7j: afterS.row == 3 (moveSouth funciono)",
               afterS.row == 3);
    reportTest("test7k: afterE.col == 2 (moveEast funciono)",
               afterE.col == 2);
    reportTest("test7l: afterW.col == 0 (moveWest funciono)",
               afterW.col == 0);
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
    test3_recogerOro();
    test4_intercambio();
    test5_conservacionOro();
    test6_invariantesSecuencia();
    test7_valueSemantics();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
