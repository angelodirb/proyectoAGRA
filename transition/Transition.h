#ifndef TRANSITION_H
#define TRANSITION_H

// ============================================================
// Transition.h
// Funciones de transicion de estado: modelan como el cubo
// se mueve sobre la grilla y como interactua con el oro.
//
// Responsabilidad de este modulo:
//   Dado un State actual, producir el State resultante de
//   aplicar un movimiento valido (norte / sur / este / oeste),
//   junto con metadata fisica del evento de oro ocurrido.
//
// Lo que NO hace este modulo:
//   - No verifica limites de la grilla (eso es del solver)
//   - No implementa Dijkstra ni costos
//   - No genera todos los movimientos posibles
//
// Separacion de responsabilidades:
//   Cube        -> orientacion + oro fisico
//   State       -> snapshot del estado global
//   Transition  -> reglas de movimiento y eventos fisicos de oro
//   Cost        -> traduccion evento -> costo
//   Solver      -> busqueda
// ============================================================

#include "../state/State.h"
#include <array>
#include <utility>  // std::pair

// ------------------------------------------------------------
// GoldCells
//
// Las 6 posiciones (row, col) originales con oro.
// Sigue siendo usada por Grid para inicializar el bitmask
// de oro de la grilla (cellGold en State).
// ------------------------------------------------------------
using GoldCells = std::array<std::pair<int,int>, 6>;

// ------------------------------------------------------------
// TransitionResult
//
// Resultado de aplicar un movimiento: nuevo estado + metadata
// fisica del evento de oro que ocurrio (si ocurrio alguno).
//
// recogioOro == true  =>  CASO 1 (pickup):
//   celda tiene oro, cara inferior vacia.
//   El cubo recoge el oro de la celda. Costo: B.
//
// recogioOro == false =>  CASO 2, CASO 3 o CASO 4:
//   CASO 2: ambos tienen oro (sin efecto neto). Costo: A.
//   CASO 3: cara inferior con oro, celda vacia. El cubo
//     deposita su oro en la celda. Costo: A.
//   CASO 4: ambos vacios. Sin efecto. Costo: A.
//
// La deteccion ocurre en aplicarReglasOro, antes de cualquier
// mutacion, con acceso al estado fisico completo.
// ------------------------------------------------------------
struct TransitionResult {
    State nextState;
    bool  recogioOro;
};


// ============================================================
// Funciones de movimiento individuales
//
// Cada funcion:
//   1. Copia el state actual (sin modificar el original)
//   2. Actualiza row/col segun la direccion
//   3. Rota el cubo en la direccion indicada
//   4. Aplica las reglas de oro en la celda destino
//   5. Retorna TransitionResult con el nuevo state y el evento
//
// PRECONDICION: el movimiento es valido (dentro de la grilla).
// La validacion de limites es responsabilidad del solver.
//
// Convencion de coordenadas:
//   - row crece hacia el sur  (moveNorth -> row -= 1)
//   - col crece hacia el este (moveEast  -> col += 1)
// ============================================================

// El cubo avanza hacia el norte: row -= 1, rollNorth()
TransitionResult moveNorth(const State& state, int cols);

// El cubo avanza hacia el sur: row += 1, rollSouth()
TransitionResult moveSouth(const State& state, int cols);

// El cubo avanza hacia el este: col += 1, rollEast()
TransitionResult moveEast(const State& state, int cols);

// El cubo avanza hacia el oeste: col -= 1, rollWest()
TransitionResult moveWest(const State& state, int cols);

#endif // TRANSITION_H
