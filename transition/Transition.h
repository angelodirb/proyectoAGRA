#ifndef TRANSITION_H
#define TRANSITION_H

// ============================================================
// Transition.h
// Funciones de transicion de estado: modelan como el cubo
// se mueve sobre la grilla y como interactua con el oro.
//
// Responsabilidad de este modulo:
//   Dado un State actual, producir el State resultante de
//   aplicar un movimiento valido (norte / sur / este / oeste).
//
// Lo que NO hace este modulo:
//   - No verifica limites de la grilla (eso es del solver)
//   - No implementa Dijkstra ni costos
//   - No genera todos los movimientos posibles
//
// Separacion de responsabilidades:
//   Cube        -> orientacion + oro fisico
//   State       -> snapshot del estado global
//   Transition  -> reglas de movimiento y de oro
//   Solver      -> busqueda (todavia no implementado)
// ============================================================

#include "../state/State.h"
#include <array>
#include <utility>  // std::pair

// ------------------------------------------------------------
// GoldCells
//
// Las 6 posiciones (row, col) donde habia oro originalmente
// en la grilla. El indice coincide con el de State::remainingGold.
//
//   goldCells[i] = {row, col} de la celda de oro i.
//
// Se pasa como parametro a las funciones de movimiento para
// que puedan saber si la celda destino es una celda de oro.
// ------------------------------------------------------------
using GoldCells = std::array<std::pair<int,int>, 6>;

// ------------------------------------------------------------
// findGoldIndex
//
// Busca si (row, col) es alguna de las 6 celdas de oro.
// Retorna el indice (0..5) si lo es, o -1 si no lo es.
//
// Usado internamente por las funciones de movimiento para
// determinar si hay interaccion de oro en la celda destino.
// ------------------------------------------------------------
int findGoldIndex(int row, int col, const GoldCells& goldCells);

// ============================================================
// Funciones de movimiento individuales
//
// Cada funcion:
//   1. Copia el state actual (sin modificar el original)
//   2. Actualiza row/col segun la direccion
//   3. Rota el cubo en la direccion indicada
//   4. Aplica las reglas de oro en la celda destino
//   5. Retorna el nuevo state
//
// PRECONDICION: el movimiento es valido (dentro de la grilla).
// La validacion de limites es responsabilidad del solver.
//
// Convencion de coordenadas:
//   - row crece hacia el sur  (moveNorth -> row -= 1)
//   - col crece hacia el este (moveEast  -> col += 1)
// ============================================================

// El cubo avanza hacia el norte: row -= 1, rollNorth()
State moveNorth(const State& state, const GoldCells& goldCells);

// El cubo avanza hacia el sur: row += 1, rollSouth()
State moveSouth(const State& state, const GoldCells& goldCells);

// El cubo avanza hacia el este: col += 1, rollEast()
State moveEast(const State& state, const GoldCells& goldCells);

// El cubo avanza hacia el oeste: col -= 1, rollWest()
State moveWest(const State& state, const GoldCells& goldCells);

#endif // TRANSITION_H
