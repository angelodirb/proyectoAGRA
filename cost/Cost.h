#ifndef COST_H
#define COST_H

// ============================================================
// Cost.h
// Funcion objetivo del problema: traduccion evento -> costo.
//
// Responsabilidad de este modulo:
//   Dado el evento fisico reportado por Transition (pickedUpGold)
//   y los parametros A y B del caso de prueba, retornar el costo
//   entero del movimiento segun las reglas del enunciado.
//
// Lo que NO hace este modulo:
//   - No implementa Dijkstra ni toma decisiones de busqueda.
//   - No rota cubos ni modifica estados.
//   - No conoce la grilla ni sus coordenadas de oro.
//   - No infiere el evento: lo recibe directamente de Transition.
//
// Separacion de responsabilidades:
//   Transition -> produce TransitionResult con evento fisico
//   Cost       -> traduce evento fisico a costo entero
//   Solver     -> orquesta la busqueda, usa Cost como caja negra
//
// Reglas de costo (del enunciado):
//
//   Costo B  (CASO 1 - "pickup real"):
//     pickedUpGold == true:
//     la celda tenia oro Y la cara inferior estaba vacia ANTES
//     del intercambio. Transition detecta esto en el momento
//     fisico exacto, sin inferencia post-hoc.
//
//   Costo A  (cualquier otro caso):
//     pickedUpGold == false:
//     - Movimiento a celda sin oro.
//     - CASO 2: cara inferior ya tenia oro (swap silencioso).
//     - Segunda visita a celda ya recogida.
// ============================================================

// ------------------------------------------------------------
// movementCost
//
// Traduce el evento fisico al costo del movimiento.
//
// Parametros:
//   pickedUpGold : true si Transition reporto CASO 1.
//   A            : costo de un movimiento normal.
//   B            : costo de un pickup real.
//
// Retorna:
//   B si pickedUpGold == true.
//   A en cualquier otro caso.
// ------------------------------------------------------------
int movementCost(bool pickedUpGold, int A, int B);

#endif // COST_H
