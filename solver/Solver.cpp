// ============================================================
// Solver.cpp
// Implementacion de Dijkstra para el problema del cubo y el oro.
// ============================================================

#include "Solver.h"
#include "StateKey.h"
#include "../state/State.h"
#include "../cube/Cube.h"
#include "../cost/Cost.h"
#include <cstdint>
#include <unordered_map>
#include <queue>
#include <vector>
#include <utility>
#include <array>
#include <iostream>

// ============================================================
// Hash para StateKey — necesario para unordered_map.
// Empaqueta todos los campos en dos uint64_t y los combina.
// ============================================================
struct StateKeyHash {
    size_t operator()(const StateKey& k) const {
        // Campos pequeños (0-7 o 0-5) caben en 1 byte cada uno
        uint64_t h1 = ((uint64_t)(uint8_t)k.row)
                    | ((uint64_t)(uint8_t)k.col)    << 8
                    | ((uint64_t)(uint8_t)k.top)    << 16
                    | ((uint64_t)(uint8_t)k.bottom) << 24
                    | ((uint64_t)(uint8_t)k.north)  << 32
                    | ((uint64_t)(uint8_t)k.south)  << 40
                    | ((uint64_t)(uint8_t)k.east)   << 48
                    | ((uint64_t)(uint8_t)k.west)   << 56;

        uint8_t cg = 0;
        for (int i = 0; i < 6; ++i)
            cg |= (k.cubeGold[i] ? 1u : 0u) << i;

        // FNV-1a de 64 bits
        size_t h = 14695981039346656037ULL;
        auto mix = [&h](uint64_t v) {
            for (int b = 0; b < 8; ++b) {
                h ^= (v >> (b * 8)) & 0xFF;
                h *= 1099511628211ULL;
            }
        };
        mix(h1);
        mix((uint64_t)cg);
        mix(k.cellGold);
        return h;
    }
};

// ============================================================
// solve
//
// Dijkstra con costos reales A/B delegados a Cost::movementCost.
//
// Estado inicial:
//   posicion   -> grid.startRow(), grid.startCol()
//   cubo       -> Cube(0,1,2,3,4,5)  (orientacion canonica)
//   cellGold      -> bitmask con bit (r*cols+c) encendido para cada 'G'
//
// Objetivo: state.allGoldCollected() == true
//           (cube.countGoldFaces() == 6)
//
// Estructura:
//   dist : unordered_map<StateKey, int>  distancia minima conocida
//   pq   : min-heap de (distancia, State)
//
// Stale entries:
//   La priority_queue no soporta decrease-key.
//   Cuando se actualiza la distancia de un estado, el entry
//   anterior queda obsoleto en la cola.
//   Al hacer pop, se verifica si la distancia coincide con dist.
//   Si no coincide -> entry obsoleto -> continuar.
// ============================================================
int solve(const Grid& grid, int A, int B, bool debug) {
    // Construir bitmask inicial de celdas con oro
    GoldCells celdasOro = grid.getGoldCells();
    uint64_t cellGoldInicial = 0;
    for (int i = 0; i < 6; ++i)
        cellGoldInicial |= 1ULL << (celdasOro[i].first * grid.cols() + celdasOro[i].second);

    State estadoInicial(grid.startRow(), grid.startCol(),
                        Cube(0, 1, 2, 3, 4, 5),
                        cellGoldInicial);

    // Min-heap: el estado con menor distancia sale primero.
    // Comparador: a.first > b.first invierte el orden natural
    // de priority_queue (que es max-heap) para obtener min-heap.
    using Entry = std::pair<int, State>;
    auto cmp = [](const Entry& a, const Entry& b) {
        return a.first > b.first;
    };
    std::priority_queue<Entry, std::vector<Entry>, decltype(cmp)> pq(cmp);

    std::unordered_map<StateKey, int, StateKeyHash> dist;

    dist[StateKey(estadoInicial)] = 0;
    pq.push({0, estadoInicial});

    if (debug) {
        std::cout << "===========================================\n";
        std::cout << "  Dijkstra Solver [debug=true]\n";
        std::cout << "===========================================\n";
        std::cout << "[INIT] pos=(" << estadoInicial.row << "," << estadoInicial.col
                  << ") goldFaces=" << estadoInicial.cube.countGoldFaces() << "\n";
    }

    while (!pq.empty()) {
        auto [currentDist, current] = pq.top();
        pq.pop();

        StateKey currentKey(current);

        // Stale entry: una distancia menor ya fue encontrada y procesada.
        auto it = dist.find(currentKey);
        if (it == dist.end() || currentDist != it->second) {
            if (debug) {
                std::cout << "[STALE] dist=" << currentDist
                          << " pos=(" << current.row << ","
                          << current.col << ")\n";
            }
            continue;
        }

        if (debug) {
            std::cout << "[EXPAND] dist=" << currentDist
                      << " pos=(" << current.row << "," << current.col << ")"
                      << " goldFaces=" << current.cube.countGoldFaces()
                      << "\n";
        }

        if (current.allGoldCollected()) {
            if (debug) {
                std::cout << "[GOAL] dist=" << currentDist << "\n";
                std::cout << "===========================================\n";
            }
            return currentDist;
        }

        // Generar transiciones con evento de oro (nextState + recogioOro)
        std::vector<TransitionResult> transitions = generateTransitions(current, grid);

        if (debug) {
            std::cout << "  vecinos=" << transitions.size() << "\n";
        }

        for (const TransitionResult& tr : transitions) {
            const State& next = tr.nextState;
            int newDist = currentDist + movementCost(tr.recogioOro, A, B);
            StateKey nextKey(next);

            auto nextIt = dist.find(nextKey);
            if (nextIt == dist.end() || newDist < nextIt->second) {
                dist[nextKey] = newDist;
                pq.push({newDist, next});

                if (debug) {
                    std::cout << "  [UPDATE] pos=(" << next.row << ","
                              << next.col << ")"
                              << " goldFaces=" << next.cube.countGoldFaces()
                              << " dist=" << newDist << "\n";
                }
            }
        }
    }

    if (debug) {
        std::cout << "[SIN SOLUCION]\n";
        std::cout << "===========================================\n";
    }

    return -1;
}
