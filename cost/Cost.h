#ifndef COST_H
#define COST_H

// ============================================================
// Cost.h
// Funcion objetivo del problema: traduccion evento -> costo.
//
// Responsabilidad de este modulo:
//   Dado el evento fisico reportado por Transition (recogioOro)
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
//   Costo B  (CASO 1 - pickup):
//     recogioOro == true:
//     celda tenia oro, cara inferior vacia. Transition detecta
//     esto antes de mutar el estado, sin inferencia post-hoc.
//
//   Costo A  (CASO 2 / CASO 3 / CASO 4):
//     recogioOro == false:
//     CASO 2: ambos tienen oro (sin efecto neto).
//     CASO 3: cara inferior con oro, celda vacia -> deposito.
//     CASO 4: ambos vacios.
// ============================================================

// ------------------------------------------------------------
// movementCost
//
// Traduce el evento fisico al costo del movimiento.
//
// Parametros:
//   recogioOro : true si Transition reporto CASO 1.
//   A            : costo de un movimiento normal.
//   B            : costo de un pickup real.
//
// Retorna:
//   B si recogioOro == true.
//   A en cualquier otro caso.
// ------------------------------------------------------------
int movementCost(bool recogioOro, int A, int B);

#endif // COST_H
