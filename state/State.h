#ifndef STATE_H
#define STATE_H

// ============================================================
// State.h
// Estado completo del problema en un instante dado.
//
// Combina tres piezas de informacion:
//   1. posicion del cubo en la grilla (row, col)
//   2. estado del cubo: orientacion + oro por cara fisica
//   3. distribucion actual del oro en celdas de la grilla (cellGold)
//
// Invariante de conservacion del oro (vale en todo instante):
//   cube.countGoldFaces() + cellGoldCount() == 6
//   El oro no se crea ni se destruye; solo circula entre celdas
//   y caras del cubo a traves de los intercambios de Transition.
//
// Separacion de responsabilidades:
//   Cube  -> orientacion, rotaciones, oro en caras fisicas
//   State -> posicion en grilla, estado global del problema
//
// Fuente de verdad del oro en caras fisicas:
//   Siempre cube.faceHasGold(i). No hay campo espejo en State.
// ============================================================

#include "../cube/Cube.h"
#include <cstdint>
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
    // Oro en celdas de la grilla.
    //
    // Bitmask de 64 bits: el bit (row * cols + col) esta encendido
    // si esa celda tiene oro actualmente.
    //
    // Usamos bitmask en lugar de bool[6] porque CASO 3 permite
    // depositar oro en cualquier celda, no solo en las 6 posiciones
    // 'G' originales. Con bool[6] esos depositos serian irrepresentables.
    //
    // El ancho de la grilla (cols) se necesita para convertir
    // (row, col) a indice y viceversa.
    // ----------------------------------------------------------
    uint64_t cellGold;

    // ----------------------------------------------------------
    // Constructor explicito.
    //
    // Todos los campos deben proporcionarse al construir un State.
    // No hay valores por defecto: el estado debe ser siempre valido.
    //
    // Parametros:
    //   row, col      : posicion del cubo en la grilla
    //   cube          : cubo con su orientacion y oro ya asignado
    //   cellGold      : bitmask de celdas que tienen oro actualmente
    // ----------------------------------------------------------
    State(int row, int col,
          const Cube& cube,
          uint64_t cellGold);

    // ----------------------------------------------------------
    // Helpers de estado global
    // ----------------------------------------------------------

    // Cuantas celdas de la grilla tienen oro actualmente.
    int cellGoldCount() const;

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
    //   - oro en celdas (cellGold)
    //
    // Necesario para el conjunto de estados visitados en Dijkstra.
    // ----------------------------------------------------------
    bool operator==(const State& other) const;
    bool operator!=(const State& other) const;
};

#endif // STATE_H
