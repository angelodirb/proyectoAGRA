#ifndef GRID_H
#define GRID_H

// ============================================================
// Grid.h
// Representa la grilla sobre la que se mueve el cubo.
//
// Responsabilidades:
//   - Conocer dimensiones y estructura de la grilla
//   - Verificar si una celda es accesible (dentro + no pared)
//   - Guardar posicion inicial y posiciones de oro
//
// Lo que NO hace este modulo:
//   - No conoce la fisica del cubo (rotaciones, orientacion)
//   - No implementa Dijkstra ni costos
//   - No aplica reglas de oro (eso es de Transition)
//
// Separacion de responsabilidades:
//   Grid       -> legalidad de movimientos, estructura de la grilla
//   Transition -> reglas fisicas del cubo y del oro
//   Solver     -> busqueda (todavia no implementado)
// ============================================================

#include "../transition/Transition.h"
#include <vector>
#include <string>

class Grid {
public:
    // ----------------------------------------------------------
    // Constructor
    //
    // Construye la grilla a partir de una representacion textual.
    //
    // Caracteres validos:
    //   '.' = celda libre
    //   '#' = pared / bloqueada
    //   'S' = posicion inicial del cubo
    //   'G' = celda con oro
    //
    // Las celdas de oro se numeran de arriba a abajo y de
    // izquierda a derecha, coincidiendo con el indice de
    // State::remainingGold y con GoldCells de Transition.h.
    //
    // Precondiciones (validadas con assert):
    //   - Exactamente 1 celda 'S'
    //   - Exactamente 6 celdas 'G'
    //   - Todas las filas tienen el mismo ancho
    // ----------------------------------------------------------
    explicit Grid(const std::vector<std::string>& layout);

    // ----------------------------------------------------------
    // Dimensiones
    // ----------------------------------------------------------
    int rows() const;
    int cols() const;

    // ----------------------------------------------------------
    // isInside
    //
    // Retorna true si (row, col) esta dentro de los limites de
    // la grilla (0 <= row < rows, 0 <= col < cols).
    // ----------------------------------------------------------
    bool isInside(int row, int col) const;

    // ----------------------------------------------------------
    // isBlocked
    //
    // Retorna true si la celda (row, col) es una pared ('#').
    // Precondicion: isInside(row, col) (validado con assert).
    // ----------------------------------------------------------
    bool isBlocked(int row, int col) const;

    // ----------------------------------------------------------
    // isValidMove
    //
    // Retorna true si mover el cubo a (row, col) es legal:
    //   - La celda esta dentro de la grilla
    //   - La celda no es una pared
    //
    // Esta es la consulta que usa generateNextStates antes de
    // llamar a cualquier funcion de Transition.
    // ----------------------------------------------------------
    bool isValidMove(int row, int col) const;

    // ----------------------------------------------------------
    // Posicion inicial del cubo (celda 'S').
    // ----------------------------------------------------------
    int startRow() const;
    int startCol() const;

    // ----------------------------------------------------------
    // getGoldCells
    //
    // Retorna las 6 posiciones (row, col) donde habia oro
    // originalmente, en orden de arriba a abajo y de izquierda
    // a derecha.
    //
    // Compatible directamente con GoldCells de Transition.h:
    //   goldCells[i] corresponde a remainingGold[i] en State.
    // ----------------------------------------------------------
    GoldCells getGoldCells() const;

    // ----------------------------------------------------------
    // printGrid
    //
    // Imprime la grilla con sus caracteres originales.
    // Muestra ademas la posicion inicial y las celdas de oro.
    // Util para verificar que el layout fue parseado bien.
    // ----------------------------------------------------------
    void printGrid() const;

private:
    std::vector<std::string> layout_;
    int rows_;
    int cols_;
    int startRow_;
    int startCol_;
    GoldCells goldCells_;
};

// ============================================================
// generateNextStates
//
// Genera todos los estados alcanzables desde current en un solo
// movimiento valido sobre la grilla grid.
//
// Para cada direccion (N, S, E, W):
//   1. Calcula la celda destino
//   2. Pregunta a Grid si el movimiento es valido
//   3. Si es valido, delega en la funcion de Transition
//   4. Agrega el Estado resultante al vector
//
// Esta funcion es una capa de coordinacion:
//   - No conoce la fisica del cubo (eso es de Transition)
//   - No conoce paredes ni limites (eso es de Grid)
//   - Solo conecta ambas capas
//
// Prerequisito del caller: current.row/col estan dentro de la
// grilla y en una celda valida (no es '#').
// ============================================================
std::vector<State> generateNextStates(
    const State& current,
    const Grid&  grid
);

#endif // GRID_H
