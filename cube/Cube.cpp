// ============================================================
// Cube.cpp
// Implementacion de la clase Cube.
// ============================================================

#include "Cube.h"

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
Cube::Cube(int top, int bottom, int north, int south, int east, int west)
    : top_(top), bottom_(bottom),
      north_(north), south_(south),
      east_(east), west_(west),
      gold_{false, false, false, false, false, false}
{}

// ------------------------------------------------------------
// Getters
// ------------------------------------------------------------
int Cube::getTop()    const { return top_;    }
int Cube::getBottom() const { return bottom_; }
int Cube::getNorth()  const { return north_;  }
int Cube::getSouth()  const { return south_;  }
int Cube::getEast()   const { return east_;   }
int Cube::getWest()   const { return west_;   }

// ------------------------------------------------------------
// rollNorth
// El cubo avanza hacia el norte (se vuelca hacia adelante).
//
// Caras afectadas (ciclo de 4):
//   south -> top -> north -> bottom -> south
//
// En otras palabras:
//   nuevo top    = antiguo south
//   nuevo north  = antiguo top
//   nuevo bottom = antiguo north
//   nuevo south  = antiguo bottom
//
// East y west NO cambian.
// ------------------------------------------------------------
void Cube::rollNorth() {
    int oldTop    = top_;
    int oldBottom = bottom_;
    int oldNorth  = north_;
    int oldSouth  = south_;

    top_    = oldSouth;
    north_  = oldTop;
    bottom_ = oldNorth;
    south_  = oldBottom;
    // east_ y west_ permanecen iguales
}

// ------------------------------------------------------------
// rollSouth
// El cubo avanza hacia el sur (se vuelca hacia atras).
//
// Ciclo inverso al de rollNorth:
//   nuevo top    = antiguo north
//   nuevo south  = antiguo top
//   nuevo bottom = antiguo south
//   nuevo north  = antiguo bottom
//
// East y west NO cambian.
// ------------------------------------------------------------
void Cube::rollSouth() {
    int oldTop    = top_;
    int oldBottom = bottom_;
    int oldNorth  = north_;
    int oldSouth  = south_;

    top_    = oldNorth;
    south_  = oldTop;
    bottom_ = oldSouth;
    north_  = oldBottom;
    // east_ y west_ permanecen iguales
}

// ------------------------------------------------------------
// rollEast
// El cubo avanza hacia el este (se vuelca hacia la derecha).
//
// Ciclo de 4:
//   west -> top -> east -> bottom -> west
//
// En otras palabras:
//   nuevo top    = antiguo west
//   nuevo east   = antiguo top
//   nuevo bottom = antiguo east
//   nuevo west   = antiguo bottom
//
// North y south NO cambian.
// ------------------------------------------------------------
void Cube::rollEast() {
    int oldTop    = top_;
    int oldBottom = bottom_;
    int oldEast   = east_;
    int oldWest   = west_;

    top_    = oldWest;
    east_   = oldTop;
    bottom_ = oldEast;
    west_   = oldBottom;
    // north_ y south_ permanecen iguales
}

// ------------------------------------------------------------
// rollWest
// El cubo avanza hacia el oeste (se vuelca hacia la izquierda).
//
// Ciclo inverso al de rollEast:
//   nuevo top    = antiguo east
//   nuevo west   = antiguo top
//   nuevo bottom = antiguo west
//   nuevo east   = antiguo bottom
//
// North y south NO cambian.
// ------------------------------------------------------------
void Cube::rollWest() {
    int oldTop    = top_;
    int oldBottom = bottom_;
    int oldEast   = east_;
    int oldWest   = west_;

    top_    = oldEast;
    west_   = oldTop;
    bottom_ = oldWest;
    east_   = oldBottom;
    // north_ y south_ permanecen iguales
}

// ------------------------------------------------------------
// print
// Muestra el estado actual del cubo. Util para debug.
// ------------------------------------------------------------
void Cube::print() const {
    std::cout << "---------------------\n";
    std::cout << "  top    = " << top_    << "\n";
    std::cout << "  bottom = " << bottom_ << "\n";
    std::cout << "  north  = " << north_  << "\n";
    std::cout << "  south  = " << south_  << "\n";
    std::cout << "  east   = " << east_   << "\n";
    std::cout << "  west   = " << west_   << "\n";
    std::cout << "---------------------\n";
}

// ============================================================
// Sistema de oro
// Cada metodo accede a gold_[faceID] donde faceID esta en [0,5].
// El oro pertenece a la cara fisica — rota con ella.
// ============================================================

// ------------------------------------------------------------
// bottomHasGold
// Retorna true si la cara fisica actualmente abajo tiene oro.
// bottom_ almacena el ID de esa cara, gold_[bottom_] da el estado.
// ------------------------------------------------------------
bool Cube::bottomHasGold() const {
    return gold_[bottom_];
}

// ------------------------------------------------------------
// topHasGold
// Retorna true si la cara fisica actualmente arriba tiene oro.
// ------------------------------------------------------------
bool Cube::topHasGold() const {
    return gold_[top_];
}

// ------------------------------------------------------------
// putGoldOnBottom
// Pone oro en la cara fisica que actualmente esta abajo.
// Si ya tenia oro, no hace nada (idempotente).
// ------------------------------------------------------------
void Cube::putGoldOnBottom() {
    gold_[bottom_] = true;
}

// ------------------------------------------------------------
// removeGoldFromBottom
// Quita el oro de la cara fisica que actualmente esta abajo.
// Si no tenia oro, no hace nada (idempotente).
// ------------------------------------------------------------
void Cube::removeGoldFromBottom() {
    gold_[bottom_] = false;
}

// ------------------------------------------------------------
// countGoldFaces
// Cuenta cuantas caras fisicas tienen oro.
// Siempre debe retornar el mismo valor antes y despues de
// cualquier rotacion (el oro viaja con las caras).
// ------------------------------------------------------------
int Cube::countGoldFaces() const {
    int count = 0;
    for (int i = 0; i < 6; ++i) {
        if (gold_[i]) ++count;
    }
    return count;
}

// ------------------------------------------------------------
// printGoldState
// Muestra:
//   - que caras fisicas tienen oro
//   - que cara fisica ocupa cada posicion espacial
// Util para trazar bugs en secuencias de rotacion.
// ------------------------------------------------------------
void Cube::printGoldState() const {
    std::cout << "--- Gold State ---\n";

    // Caras con oro
    std::cout << "  Caras con oro: ";
    bool any = false;
    for (int i = 0; i < 6; ++i) {
        if (gold_[i]) {
            std::cout << i << " ";
            any = true;
        }
    }
    if (!any) std::cout << "(ninguna)";
    std::cout << "\n";

    // Orientacion actual: posicion -> cara fisica
    std::cout << "  Orientacion actual:\n";
    std::cout << "    top    -> cara " << top_    << (gold_[top_]    ? " [ORO]" : "") << "\n";
    std::cout << "    bottom -> cara " << bottom_ << (gold_[bottom_] ? " [ORO]" : "") << "\n";
    std::cout << "    north  -> cara " << north_  << (gold_[north_]  ? " [ORO]" : "") << "\n";
    std::cout << "    south  -> cara " << south_  << (gold_[south_]  ? " [ORO]" : "") << "\n";
    std::cout << "    east   -> cara " << east_   << (gold_[east_]   ? " [ORO]" : "") << "\n";
    std::cout << "    west   -> cara " << west_   << (gold_[west_]   ? " [ORO]" : "") << "\n";
    std::cout << "-----------------\n";
}
