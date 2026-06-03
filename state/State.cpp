// ============================================================
// State.cpp
// Implementacion de State.
// ============================================================

#include "State.h"

// ------------------------------------------------------------
// Constructor
// Inicializa todos los campos directamente.
// No hay logica adicional: el State es un snapshot de valores.
// ------------------------------------------------------------
State::State(int row, int col,
             const Cube& cube,
             uint64_t cellGold)
    : row(row), col(col),
      cube(cube),
      cellGold(cellGold)
{}

// ------------------------------------------------------------
// cellGoldCount
// Popcount del bitmask: cada bit encendido es una celda con oro.
// ------------------------------------------------------------
int State::cellGoldCount() const {
    uint64_t v = cellGold;
    int count = 0;
    while (v) { v &= v - 1; ++count; }
    return count;
}

// ------------------------------------------------------------
// allGoldCollected
// El objetivo es que las 6 caras fisicas del cubo tengan oro.
// Delega en Cube::countGoldFaces() que usa su gold_[] interno.
// ------------------------------------------------------------
bool State::allGoldCollected() const {
    return cube.countGoldFaces() == 6;
}

// ------------------------------------------------------------
// printState
// Imprime el estado completo. Disenado para facilitar el
// debugging de la busqueda con Dijkstra.
// ------------------------------------------------------------
void State::printState() const {
    std::cout << "===========================\n";
    std::cout << "STATE\n";
    std::cout << "===========================\n";

    // Posicion
    std::cout << "Posicion : row=" << row
              << "  col=" << col << "\n";

    // Orientacion del cubo
    std::cout << "\nOrientacion del cubo:\n";
    std::cout << "  top    -> cara " << cube.getTop()    << "\n";
    std::cout << "  bottom -> cara " << cube.getBottom() << "\n";
    std::cout << "  north  -> cara " << cube.getNorth()  << "\n";
    std::cout << "  south  -> cara " << cube.getSouth()  << "\n";
    std::cout << "  east   -> cara " << cube.getEast()   << "\n";
    std::cout << "  west   -> cara " << cube.getWest()   << "\n";

    // Oro en cada cara fisica — fuente unica: cube.faceHasGold
    std::cout << "\nOro en caras fisicas del cubo:\n";
    for (int i = 0; i < 6; ++i) {
        std::cout << "  cara " << i << " : "
                  << (cube.faceHasGold(i) ? "[ORO]" : "vacia") << "\n";
    }
    std::cout << "  Total caras con oro : "
              << cube.countGoldFaces() << " / 6\n";

    // Oro en celdas de la grilla (bitmask)
    std::cout << "\nOro en celdas (bitmask): 0x"
              << std::hex << cellGold << std::dec
              << "  (" << cellGoldCount() << " celda(s))\n";

    // Estado del objetivo
    std::cout << "\nObjetivo (6 caras con oro) : "
              << (allGoldCollected() ? "LOGRADO" : "pendiente") << "\n";
    std::cout << "===========================\n";
}

// ------------------------------------------------------------
// operator==
// Compara campo a campo. El oro en caras fisicas se obtiene
// directamente de cube.faceHasGold(i): una sola fuente de
// verdad, sin riesgo de desincronizacion.
// ------------------------------------------------------------
bool State::operator==(const State& other) const {
    // Posicion
    if (row != other.row || col != other.col) return false;

    // Orientacion del cubo (las 6 caras)
    if (cube.getTop()    != other.cube.getTop()    ||
        cube.getBottom() != other.cube.getBottom() ||
        cube.getNorth()  != other.cube.getNorth()  ||
        cube.getSouth()  != other.cube.getSouth()  ||
        cube.getEast()   != other.cube.getEast()   ||
        cube.getWest()   != other.cube.getWest())  return false;

    // Oro en caras fisicas — consultado directamente en Cube
    for (int i = 0; i < 6; ++i) {
        if (cube.faceHasGold(i) != other.cube.faceHasGold(i)) return false;
    }

    // Oro en celdas
    if (cellGold != other.cellGold) return false;

    return true;
}

bool State::operator!=(const State& other) const {
    return !(*this == other);
}
