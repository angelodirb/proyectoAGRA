#ifndef STATEKEY_H
#define STATEKEY_H

// ============================================================
// StateKey.h
// Representacion compacta y comparable de un estado del problema.
//
// Proposito:
//   Permitir que estados sean usados como clave en std::map y
//   como elementos de un conjunto de visitados en Dijkstra.
//
// Separacion de State y StateKey:
//   State    -> objeto rico: logica, debug, metodos de conveniencia
//   StateKey -> dato puro: comparable, deterministico, sin logica
//
// Dos estados fisicamente equivalentes producen exactamente
// el mismo StateKey. Esta propiedad garantiza correctitud en
// la busqueda de estados.
//
// StateKey es conceptualmente inmutable: se construye una vez
// desde un State y no se modifica.
// ============================================================

#include "../state/State.h"
#include <array>
#include <iostream>

struct StateKey {

    // ----------------------------------------------------------
    // Posicion del cubo en la grilla
    // ----------------------------------------------------------
    int row;
    int col;

    // ----------------------------------------------------------
    // Orientacion del cubo: los 6 IDs de cara fisica en sus
    // posiciones espaciales actuales.
    //
    // Dos cubos con la misma orientacion tienen exactamente los
    // mismos valores de top/bottom/north/south/east/west.
    // ----------------------------------------------------------
    int top;
    int bottom;
    int north;
    int south;
    int east;
    int west;

    // ----------------------------------------------------------
    // Oro en caras fisicas del cubo.
    //
    //   cubeGold[i] == true  =>  la cara fisica i tiene oro.
    //
    // Obtenido directamente de cube.faceHasGold(i) para i=0..5.
    // ----------------------------------------------------------
    std::array<bool, 6> cubeGold;

    // ----------------------------------------------------------
    // Oro restante en las celdas originales de la grilla.
    //
    //   remainingGold[i] == true  =>  la celda i todavia tiene oro.
    //
    // Copia directa de State::remainingGold.
    // ----------------------------------------------------------
    std::array<bool, 6> remainingGold;

    // ----------------------------------------------------------
    // Constructor desde State.
    //
    // Extrae toda la informacion necesaria del State dado.
    // No guarda referencias: StateKey es completamente independiente
    // del State original despues de la construccion.
    // ----------------------------------------------------------
    explicit StateKey(const State& state);

    // ----------------------------------------------------------
    // operator<
    //
    // Comparacion lexicografica en orden:
    //   1. row
    //   2. col
    //   3. top, bottom, north, south, east, west
    //   4. cubeGold
    //   5. remainingGold
    //
    // Garantiza orden total estricto: permite usar StateKey como
    // clave en std::map<StateKey, int>.
    //
    // Deterministico: siempre produce el mismo orden para los
    // mismos datos. Facilita debugging reproducible.
    // ----------------------------------------------------------
    bool operator<(const StateKey& other) const;

    // ----------------------------------------------------------
    // operator==
    //
    // Dos StateKeys son iguales si y solo si todos sus campos
    // son iguales. Equivale a que los states originales sean
    // fisicamente indistinguibles.
    // ----------------------------------------------------------
    bool operator==(const StateKey& other) const;
    bool operator!=(const StateKey& other) const;

    // ----------------------------------------------------------
    // printKey
    //
    // Imprime el contenido completo de la clave.
    // Disenado para debugging durante el desarrollo del solver:
    // permite inspeccionar exactamente como se ve el estado
    // desde el punto de vista de la busqueda.
    // ----------------------------------------------------------
    void printKey() const;
};

#endif // STATEKEY_H
