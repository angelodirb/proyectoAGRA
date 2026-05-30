#ifndef STATE_H
#define STATE_H

// ============================================================
// State.h
// Estado completo del problema en un instante dado.
//
// Combina tres piezas de informacion:
//   1. posicion del cubo en la grilla (row, col)
//   2. estado del cubo: orientacion + oro por cara fisica
//   3. cuales de las 6 celdas originales aun tienen oro
//
// Separacion de responsabilidades:
//   Cube  -> orientacion, rotaciones, oro en caras fisicas
//   State -> posicion en grilla, estado global del problema
//
// Fuente de verdad del oro en caras fisicas:
//   Siempre cube.faceHasGold(i). No hay campo espejo en State.
//   Esto garantiza consistencia: no hay estado duplicado que
//   pueda desincronizarse.
// ============================================================

#include "../cube/Cube.h"
#include <array>
#include <iostream>

struct State {

    // ----------------------------------------------------------
    // Posicion del cubo en la grilla.
    //   row: fila    (0-indexed, crece hacia el sur)
    //   col: columna (0-indexed, crece hacia el este)
    // ----------------------------------------------------------
    int row;
    int col;

    // ----------------------------------------------------------
    // El cubo: orientacion y sistema de oro interno.
    // Usos principales:
    //   - saber que cara queda abajo al llegar a una celda
    //   - recoger/devolver oro (putGoldOnBottom / removeGoldFromBottom)
    //   - consultar oro por cara (faceHasGold)
    //   - contar cuantas caras tienen oro (countGoldFaces)
    // ----------------------------------------------------------
    Cube cube;

    // ----------------------------------------------------------
    // Estado del oro en las 6 celdas originales de la grilla.
    //
    //   remainingGold[i] == true  =>  la celda i todavia tiene oro.
    //
    // El problema garantiza exactamente 6 celdas con oro.
    // El indice 0..5 corresponde al orden de aparicion en la grilla
    // (de arriba a abajo, de izquierda a derecha).
    // ----------------------------------------------------------
    std::array<bool, 6> remainingGold;

    // ----------------------------------------------------------
    // Constructor explicito.
    //
    // Todos los campos deben proporcionarse al construir un State.
    // No hay valores por defecto: el estado debe ser siempre valido.
    //
    // Parametros:
    //   row, col      : posicion del cubo en la grilla
    //   cube          : cubo con su orientacion y oro ya asignado
    //   remainingGold : cuales celdas originales tienen oro aun
    // ----------------------------------------------------------
    State(int row, int col,
          const Cube& cube,
          const std::array<bool, 6>& remainingGold);

    // ----------------------------------------------------------
    // Helpers de estado global
    // ----------------------------------------------------------

    // Cuantas de las 6 celdas originales todavia tienen oro.
    int remainingGoldCount() const;

    // El objetivo del problema: las 6 caras fisicas del cubo
    // tienen oro simultaneamente (cube.countGoldFaces() == 6).
    bool allGoldCollected() const;

    // ----------------------------------------------------------
    // Debug: imprime el estado completo de forma legible.
    // Muestra posicion, orientacion del cubo, oro en caras
    // y oro restante en celdas. Muy util para trazar Dijkstra.
    // ----------------------------------------------------------
    void printState() const;

    // ----------------------------------------------------------
    // Comparacion de estados.
    //
    // Dos estados son iguales si coinciden en:
    //   - posicion (row, col)
    //   - orientacion del cubo (todas las 6 caras)
    //   - oro en caras fisicas (cube.faceHasGold para i = 0..5)
    //   - oro restante en celdas (remainingGold)
    //
    // Necesario para el conjunto de estados visitados en Dijkstra.
    // ----------------------------------------------------------
    bool operator==(const State& other) const;
    bool operator!=(const State& other) const;
};

#endif // STATE_H
