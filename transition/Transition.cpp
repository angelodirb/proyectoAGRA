// ============================================================
// Transition.cpp
// Implementacion de las funciones de transicion de estado.
// ============================================================

#include "Transition.h"

// ============================================================
// findGoldIndex
// ============================================================
int findGoldIndex(int row, int col, const GoldCells& goldCells) {
    for (int i = 0; i < 6; ++i) {
        if (goldCells[i].first == row && goldCells[i].second == col)
            return i;
    }
    return -1;
}

// ============================================================
// applyGoldRules  (funcion interna, no expuesta en el header)
//
// Aplica las reglas de oro al llegar a la celda (row, col).
// Modifica newState directamente (ya es una copia del original).
//
// Reglas:
//   Si la celda destino NO tiene oro:
//     -> no ocurre nada
//
//   Si la celda destino tiene oro:
//     CASO 1: cara bottom sin oro
//       -> cubo recoge el oro (putGoldOnBottom)
//       -> celda pierde su oro (remainingGold[i] = false)
//
//     CASO 2: cara bottom ya tiene oro
//       -> ocurre intercambio
//       -> la cantidad total de oro no cambia
//       -> el estado booleano no cambia (ambos siguen con oro)
// ============================================================
static void applyGoldRules(State& newState, int row, int col,
                            const GoldCells& goldCells) {
    int goldIdx = findGoldIndex(row, col, goldCells);

    if (goldIdx == -1)                         return;  // no es celda de oro
    if (!newState.remainingGold[goldIdx])      return;  // celda ya recogida

    // La celda destino tiene oro
    if (!newState.cube.bottomHasGold()) {
        // CASO 1: cara bottom sin oro -> el cubo recoge
        newState.cube.putGoldOnBottom();
        newState.remainingGold[goldIdx] = false;
    }
    // CASO 2: cara bottom ya tiene oro -> intercambio
    // El estado booleano no cambia: celda y cara siguen con oro.
    // No se requiere ninguna accion adicional.
}

// ============================================================
// moveNorth
// El cubo avanza al norte: fila disminuye, rollNorth.
// ============================================================
State moveNorth(const State& state, const GoldCells& goldCells) {
    State next = state;        // copia completa, el original no se toca
    next.row -= 1;
    next.cube.rollNorth();
    applyGoldRules(next, next.row, next.col, goldCells);
    return next;
}

// ============================================================
// moveSouth
// El cubo avanza al sur: fila aumenta, rollSouth.
// ============================================================
State moveSouth(const State& state, const GoldCells& goldCells) {
    State next = state;
    next.row += 1;
    next.cube.rollSouth();
    applyGoldRules(next, next.row, next.col, goldCells);
    return next;
}

// ============================================================
// moveEast
// El cubo avanza al este: columna aumenta, rollEast.
// ============================================================
State moveEast(const State& state, const GoldCells& goldCells) {
    State next = state;
    next.col += 1;
    next.cube.rollEast();
    applyGoldRules(next, next.row, next.col, goldCells);
    return next;
}

// ============================================================
// moveWest
// El cubo avanza al oeste: columna disminuye, rollWest.
// ============================================================
State moveWest(const State& state, const GoldCells& goldCells) {
    State next = state;
    next.col -= 1;
    next.cube.rollWest();
    applyGoldRules(next, next.row, next.col, goldCells);
    return next;
}
