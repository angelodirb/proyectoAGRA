// ============================================================
// Solver.cpp
// Implementacion de Dijkstra para el problema del cubo y el oro.
// ============================================================

#include "Solver.h"
#include "StateKey.h"
#include "../state/State.h"
#include "../cube/Cube.h"
#include "../cost/Cost.h"
#include <map>
#include <queue>
#include <vector>
#include <utility>
#include <array>
#include <iostream>

// ============================================================
// solve
//
// Dijkstra con costos reales A/B delegados a Cost::movementCost.
//
// Estado inicial:
//   posicion   -> grid.startRow(), grid.startCol()
//   cubo       -> Cube(0,1,2,3,4,5)  (orientacion canonica)
//   remainingGold -> {true x6}       (ninguna celda recogida)
//
// Objetivo: state.allGoldCollected() == true
//           (cube.countGoldFaces() == 6)
//
// Estructura:
//   dist : std::map<StateKey, int>   distancia minima conocida
//   pq   : min-heap de (dist, State)
//
// Stale entries:
//   La priority_queue no soporta decrease-key.
//   Cuando se actualiza la distancia de un estado, el entry
//   anterior queda obsoleto en la cola.
//   Al hacer pop, se verifica si la distancia coincide con dist.
//   Si no coincide -> entry obsoleto -> continuar.
// ============================================================
int solve(const Grid& grid, int A, int B, bool debug) {
    std::array<bool, 6> allGold = {true, true, true, true, true, true};
    State init(grid.startRow(), grid.startCol(),
               Cube(0, 1, 2, 3, 4, 5),
               allGold);

    // Min-heap: el estado con menor distancia sale primero.
    // Comparador: a.first > b.first invierte el orden natural
    // de priority_queue (que es max-heap) para obtener min-heap.
    using Entry = std::pair<int, State>;
    auto cmp = [](const Entry& a, const Entry& b) {
        return a.first > b.first;
    };
    std::priority_queue<Entry, std::vector<Entry>, decltype(cmp)> pq(cmp);

    std::map<StateKey, int> dist;

    dist[StateKey(init)] = 0;
    pq.push({0, init});

    if (debug) {
        std::cout << "===========================================\n";
        std::cout << "  Dijkstra Solver [debug=true]\n";
        std::cout << "===========================================\n";
        std::cout << "[INIT] pos=(" << init.row << "," << init.col
                  << ") goldFaces=" << init.cube.countGoldFaces() << "\n";
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

        // Verificacion del objetivo
        if (current.allGoldCollected()) {
            if (debug) {
                std::cout << "[GOAL] dist=" << currentDist << "\n";
                std::cout << "===========================================\n";
            }
            return currentDist;
        }

        // Generar estados sucesores
        std::vector<State> neighbors = generateNextStates(current, grid);

        if (debug) {
            std::cout << "  vecinos=" << neighbors.size() << "\n";
        }

        for (const State& next : neighbors) {
            int newDist = currentDist + movementCost(current, next, A, B);
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
