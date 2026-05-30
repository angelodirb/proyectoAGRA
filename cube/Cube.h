#ifndef CUBE_H
#define CUBE_H

// ============================================================
// Cube.h
// Representa la orientacion de un cubo de 6 caras.
// Cada cara almacena un identificador entero.
//
// Convencion de ejes:
//   - North/South : eje Y (norte = Y positivo)
//   - East/West   : eje X (este  = X positivo)
//   - Top/Bottom  : eje Z (arriba = Z positivo)
//
// Rotaciones (el cubo rueda en la direccion indicada):
//   rollNorth : el cubo avanza hacia el norte
//   rollSouth : el cubo avanza hacia el sur
//   rollEast  : el cubo avanza hacia el este
//   rollWest  : el cubo avanza hacia el oeste
// ============================================================

#include <iostream>

class Cube {
public:
    // ----------------------------------------------------------
    // Constructor: inicializa cada cara con un ID distinto.
    //
    // IMPORTANTE: para usar el sistema de oro, los IDs deben
    // estar en el rango [0, 5], ya que gold_[id] indexa
    // directamente por cara fisica.
    //
    // Valores por defecto sugeridos:
    //   top=0, bottom=1, north=2, south=3, east=4, west=5
    //
    // Todas las caras arrancan sin oro (gold_[i] = false).
    // ----------------------------------------------------------
    Cube(int top, int bottom, int north, int south, int east, int west);

    // Accesores (getters) para cada cara
    int getTop()    const;
    int getBottom() const;
    int getNorth()  const;
    int getSouth()  const;
    int getEast()   const;
    int getWest()   const;

    // ----------------------------------------------------------
    // Rotaciones
    // Cada funcion actualiza las 4 caras afectadas;
    // las otras 2 permanecen iguales.
    // ----------------------------------------------------------

    // El cubo rueda hacia el norte:
    //   top -> north, north -> bottom, bottom -> south, south -> top
    void rollNorth();

    // El cubo rueda hacia el sur:
    //   top -> south, south -> bottom, bottom -> north, north -> top
    void rollSouth();

    // El cubo rueda hacia el este:
    //   top -> east, east -> bottom, bottom -> west, west -> top
    void rollEast();

    // El cubo rueda hacia el oeste:
    //   top -> west, west -> bottom, bottom -> east, east -> top
    void rollWest();

    // Imprime el estado actual (util para debug)
    void print() const;

    // ----------------------------------------------------------
    // Sistema de oro
    //
    // El oro pertenece a las CARAS FISICAS, no a las posiciones
    // espaciales. Cuando el cubo rota, el oro viaja con la cara.
    //
    // gold_[i] == true  =>  la cara fisica i tiene oro.
    // Requiere que los face IDs esten en el rango [0, 5].
    // ----------------------------------------------------------

    // Consulta si la cara fisica actualmente abajo tiene oro
    bool bottomHasGold() const;

    // Consulta si la cara fisica actualmente arriba tiene oro
    bool topHasGold() const;

    // Pone oro en la cara fisica actualmente abajo
    void putGoldOnBottom();

    // Quita oro de la cara fisica actualmente abajo
    void removeGoldFromBottom();

    // Retorna si la cara fisica con el ID indicado tiene oro.
    // Permite inspeccionar cualquier cara sin exponer gold_[].
    // Precondicion: 0 <= faceID < 6  (validado con assert).
    bool faceHasGold(int faceID) const;

    // Retorna cuantas caras fisicas tienen oro (0..6)
    int countGoldFaces() const;

    // Imprime que caras tienen oro y que cara fisica ocupa
    // cada posicion espacial. Util para debug.
    void printGoldState() const;

private:
    int top_;
    int bottom_;
    int north_;
    int south_;
    int east_;
    int west_;

    // gold_[i] == true si la cara fisica i tiene oro.
    // Indexado directamente por face ID (debe estar en [0,5]).
    bool gold_[6];
};

#endif // CUBE_H
