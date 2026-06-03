// ============================================================
// Grid.cpp
// Implementacion de Grid y generateNextStates.
// ============================================================

#include "Grid.h"
#include <cassert>
#include <iostream>

// ============================================================
// Constructor
// ============================================================
Grid::Grid(const std::vector<std::string>& layout)
    : layout_(layout),
      rows_(static_cast<int>(layout.size())),
      cols_(layout.empty() ? 0 : static_cast<int>(layout[0].size())),
      startRow_(-1),
      startCol_(-1)
{
    // Todas las filas deben tener el mismo ancho
    for (const auto& row : layout_) {
        assert(static_cast<int>(row.size()) == cols_
               && "Todas las filas deben tener el mismo ancho");
    }

    int startCount = 0;
    int goldIdx    = 0;

    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            char ch = layout_[r][c];

            if (ch == 'S') {
                ++startCount;
                startRow_ = r;
                startCol_ = c;

            } else if (ch == 'G') {
                assert(goldIdx < 6
                       && "Se encontraron mas de 6 celdas de oro 'G'");
                goldCells_[goldIdx] = {r, c};
                ++goldIdx;
            }
        }
    }

    assert(startCount == 1
           && "La grilla debe tener exactamente 1 celda de inicio 'S'");
    assert(goldIdx == 6
           && "La grilla debe tener exactamente 6 celdas de oro 'G'");
}

// ============================================================
// Dimensiones
// ============================================================
int Grid::rows() const { return rows_; }
int Grid::cols() const { return cols_; }

// ============================================================
// isInside
// ============================================================
bool Grid::isInside(int row, int col) const {
    return row >= 0 && row < rows_
        && col >= 0 && col < cols_;
}

// ============================================================
// isBlocked
// ============================================================
bool Grid::isBlocked(int row, int col) const {
    assert(isInside(row, col) && "isBlocked: celda fuera de limites");
    return layout_[row][col] == '#';
}

// ============================================================
// isValidMove
// ============================================================
bool Grid::isValidMove(int row, int col) const {
    if (!isInside(row, col)) return false;
    return layout_[row][col] != '#';
}

// ============================================================
// Posicion inicial
// ============================================================
int Grid::startRow() const { return startRow_; }
int Grid::startCol() const { return startCol_; }

// ============================================================
// getGoldCells
// ============================================================
GoldCells Grid::getGoldCells() const {
    return goldCells_;
}

// ============================================================
// printGrid
// ============================================================
void Grid::printGrid() const {
    std::cout << "Grid (" << rows_ << " filas x "
              << cols_ << " columnas):\n";
    for (int r = 0; r < rows_; ++r) {
        std::cout << "  " << layout_[r] << "\n";
    }
    std::cout << "Inicio : (" << startRow_ << ", "
              << startCol_ << ")\n";
    std::cout << "Celdas de oro:\n";
    for (int i = 0; i < 6; ++i) {
        std::cout << "  [" << i << "] -> ("
                  << goldCells_[i].first << ", "
                  << goldCells_[i].second << ")\n";
    }
}

// ============================================================
// generateTransitions
//
// Capa de coordinacion: consulta Grid para legalidad,
// delega la fisica en las funciones de Transition.
// Propaga TransitionResult completo (estado + evento de oro).
// ============================================================
std::vector<TransitionResult> generateTransitions(
    const State& current,
    const Grid&  grid)
{
    std::vector<TransitionResult> transiciones;
    const int cols = grid.cols();

    if (grid.isValidMove(current.row - 1, current.col))
        transiciones.push_back(moveNorth(current, cols));

    if (grid.isValidMove(current.row + 1, current.col))
        transiciones.push_back(moveSouth(current, cols));

    if (grid.isValidMove(current.row, current.col + 1))
        transiciones.push_back(moveEast(current, cols));

    if (grid.isValidMove(current.row, current.col - 1))
        transiciones.push_back(moveWest(current, cols));

    return transiciones;
}
