// ============================================================
// Cost.cpp
// Implementacion de movementCost.
// ============================================================

#include "Cost.h"

// ============================================================
// movementCost
//
// Traduce el evento fisico al costo del movimiento.
// pickedUpGold viene directamente de TransitionResult::pickedUpGold,
// que Transition detecta en el momento fisico exacto (dentro de
// applyGoldRules, antes de cualquier mutacion de estado).
//
// No hay inferencia post-hoc: Transition ya hizo el trabajo de
// determinar si ocurrio CASO 1. Cost solo mapea bool -> int.
// ============================================================
int movementCost(bool pickedUpGold, int A, int B) {
    return pickedUpGold ? B : A;
}
