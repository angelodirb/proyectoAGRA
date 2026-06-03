// ============================================================
// Transition.cpp
// Implementacion de las funciones de transicion de estado.
// ============================================================

#include "Transition.h"

// ============================================================
// aplicarReglasOro  (funcion interna, no expuesta en el header)
//
// Centralizamos aqui el intercambio entre la cara inferior del
// cubo y la celda destino. Leer bottomHadGold ANTES de mutar
// garantiza deteccion exacta al instante fisico del intercambio.
//
//   CASO 1: celda con oro, cara inferior VACIA
//     -> cara recoge oro (putGoldOnBottom), celda lo pierde
//     -> retorna true  (recogioOro = true, costo B)
//
//   CASO 2: celda con oro, cara inferior CON ORO
//     -> intercambio entre piezas identicas: sin efecto neto
//     -> retorna false (costo A)
//
//   CASO 3: celda VACIA, cara inferior CON ORO
//     -> cara deposita oro en celda (removeGoldFromBottom)
//     -> retorna false (costo A)
//
//   CASO 4: ambos vacios — sin efecto
//     -> retorna false (costo A)
// ============================================================
static bool aplicarReglasOro(State& newState, int row, int col, int cols) {
    bool bottomHadGold = newState.cube.bottomHasGold();

    uint64_t mask     = 1ULL << (row * cols + col);
    bool cellHasGold  = (newState.cellGold & mask) != 0;

    if (cellHasGold && !bottomHadGold) {
        // CASO 1: pickup real
        newState.cube.putGoldOnBottom();
        newState.cellGold &= ~mask;
        return true;
    }
    if (!cellHasGold && bottomHadGold) {
        // CASO 3: la cara deposita oro en la celda vacia
        newState.cube.removeGoldFromBottom();
        newState.cellGold |= mask;
        return false;
    }
    // CASO 2 o CASO 4: sin efecto observable
    return false;
}

// ============================================================
// moveNorth
// ============================================================
TransitionResult moveNorth(const State& state, int cols) {
    State next = state;
    next.row -= 1;
    next.cube.rollNorth();
    bool recogio = aplicarReglasOro(next, next.row, next.col, cols);
    return {next, recogio};
}

// ============================================================
// moveSouth
// ============================================================
TransitionResult moveSouth(const State& state, int cols) {
    State next = state;
    next.row += 1;
    next.cube.rollSouth();
    bool recogio = aplicarReglasOro(next, next.row, next.col, cols);
    return {next, recogio};
}

// ============================================================
// moveEast
// ============================================================
TransitionResult moveEast(const State& state, int cols) {
    State next = state;
    next.col += 1;
    next.cube.rollEast();
    bool recogio = aplicarReglasOro(next, next.row, next.col, cols);
    return {next, recogio};
}

// ============================================================
// moveWest
// ============================================================
TransitionResult moveWest(const State& state, int cols) {
    State next = state;
    next.col -= 1;
    next.cube.rollWest();
    bool recogio = aplicarReglasOro(next, next.row, next.col, cols);
    return {next, recogio};
}
