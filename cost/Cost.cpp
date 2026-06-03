// ============================================================
// Cost.cpp
// Implementacion de movementCost.
// ============================================================

#include "Cost.h"

// ============================================================
// movementCost
//
// Traduce el evento fisico al costo del movimiento.
// recogioOro viene directamente de TransitionResult::recogioOro,
// que aplicarReglasOro detecta antes de cualquier mutacion.
//
// No hay inferencia post-hoc: Transition ya hizo el trabajo de
// determinar si ocurrio CASO 1. Cost solo mapea bool -> int.
// ============================================================
int movementCost(bool recogioOro, int A, int B) {
    return recogioOro ? B : A;
}
