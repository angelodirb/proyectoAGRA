#ifndef SOLVER_H
#define SOLVER_H

// ============================================================
// Solver.h
// Busqueda de camino minimo mediante Dijkstra.
//
// Responsabilidad de este modulo:
//   Coordinar la busqueda: estado inicial, cola de prioridad,
//   mapa de distancias, generacion de vecinos y deteccion del
//   estado objetivo.
//
// Lo que NO hace este modulo:
//   - No rota cubos manualmente
//   - No aplica reglas de oro ni modifica cellGold directamente
//   - No conoce las reglas de oro
//
//   Toda la fisica esta en Cube, Transition y Grid.
//
// Objetivo:
//   Encontrar el numero minimo de movimientos para que el cubo
//   tenga oro en sus 6 caras fisicas simultaneamente.
//
// Retorna:
//   Distancia minima si existe solucion.
//   -1 si no existe solucion.
// ============================================================

#include "../grid/Grid.h"

// debug=true imprime cada expansion, actualizacion y goal.
int solve(const Grid& grid, int A, int B, bool debug = false);

#endif // SOLVER_H
