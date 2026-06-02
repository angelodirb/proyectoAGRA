// ============================================================
// StateKey.cpp
// Implementacion de StateKey.
// ============================================================

#include "StateKey.h"

// ============================================================
// Constructor
// Extrae todos los campos necesarios del State dado.
// ============================================================
StateKey::StateKey(const State& state)
    : row(state.row),
      col(state.col),
      top   (state.cube.getTop()),
      bottom(state.cube.getBottom()),
      north (state.cube.getNorth()),
      south (state.cube.getSouth()),
      east  (state.cube.getEast()),
      west  (state.cube.getWest()),
      remainingGold(state.remainingGold)
{
    for (int i = 0; i < 6; ++i)
        cubeGold[i] = state.cube.faceHasGold(i);
}

// ============================================================
// operator<
//
// Comparacion lexicografica campo a campo.
// El orden es deterministico y define un orden total estricto.
// ============================================================
bool StateKey::operator<(const StateKey& other) const {
    // Posicion
    if (row != other.row) return row < other.row;
    if (col != other.col) return col < other.col;

    // Orientacion del cubo
    if (top    != other.top)    return top    < other.top;
    if (bottom != other.bottom) return bottom < other.bottom;
    if (north  != other.north)  return north  < other.north;
    if (south  != other.south)  return south  < other.south;
    if (east   != other.east)   return east   < other.east;
    if (west   != other.west)   return west   < other.west;

    // Oro en caras fisicas (std::array soporta operator<)
    if (cubeGold != other.cubeGold) return cubeGold < other.cubeGold;

    // Oro restante en celdas
    if (remainingGold != other.remainingGold)
        return remainingGold < other.remainingGold;

    return false;  // iguales -> no es menor
}

// ============================================================
// operator==
// ============================================================
bool StateKey::operator==(const StateKey& other) const {
    return row    == other.row
        && col    == other.col
        && top    == other.top
        && bottom == other.bottom
        && north  == other.north
        && south  == other.south
        && east   == other.east
        && west   == other.west
        && cubeGold      == other.cubeGold
        && remainingGold == other.remainingGold;
}

bool StateKey::operator!=(const StateKey& other) const {
    return !(*this == other);
}

// ============================================================
// printKey
// ============================================================
void StateKey::printKey() const {
    std::cout << "===========================\n";
    std::cout << "STATE KEY\n";
    std::cout << "===========================\n";

    std::cout << "Posicion : row=" << row
              << "  col=" << col << "\n";

    std::cout << "\nOrientacion del cubo:\n";
    std::cout << "  top    = " << top    << "\n";
    std::cout << "  bottom = " << bottom << "\n";
    std::cout << "  north  = " << north  << "\n";
    std::cout << "  south  = " << south  << "\n";
    std::cout << "  east   = " << east   << "\n";
    std::cout << "  west   = " << west   << "\n";

    std::cout << "\nOro en caras fisicas:\n";
    for (int i = 0; i < 6; ++i) {
        std::cout << "  cara " << i << " : "
                  << (cubeGold[i] ? "[ORO]" : "vacia") << "\n";
    }

    std::cout << "\nOro en celdas originales:\n";
    for (int i = 0; i < 6; ++i) {
        std::cout << "  celda " << i << " : "
                  << (remainingGold[i] ? "tiene oro" : "recogido") << "\n";
    }

    std::cout << "===========================\n";
}
