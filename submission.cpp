#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <utility>

// ============================================================
// Cube
// Orientacion + sistema de oro en caras fisicas.
// El oro pertenece a la cara fisica: viaja con ella al rotar.
// ============================================================
class Cube {
public:
    Cube(int top, int bottom, int north, int south, int east, int west)
        : top_(top), bottom_(bottom),
          north_(north), south_(south),
          east_(east), west_(west),
          gold_{false, false, false, false, false, false}
    {}

    int getTop()    const { return top_;    }
    int getBottom() const { return bottom_; }
    int getNorth()  const { return north_;  }
    int getSouth()  const { return south_;  }
    int getEast()   const { return east_;   }
    int getWest()   const { return west_;   }

    void rollNorth() {
        int t=top_, b=bottom_, n=north_, s=south_;
        top_=s; north_=t; bottom_=n; south_=b;
    }
    void rollSouth() {
        int t=top_, b=bottom_, n=north_, s=south_;
        top_=n; south_=t; bottom_=s; north_=b;
    }
    void rollEast() {
        int t=top_, b=bottom_, e=east_, w=west_;
        top_=w; east_=t; bottom_=e; west_=b;
    }
    void rollWest() {
        int t=top_, b=bottom_, e=east_, w=west_;
        top_=e; west_=t; bottom_=w; east_=b;
    }

    bool bottomHasGold()        const { return gold_[bottom_]; }
    void putGoldOnBottom()            { gold_[bottom_] = true;  }
    void removeGoldFromBottom()       { gold_[bottom_] = false; }
    bool faceHasGold(int id)    const { return gold_[id]; }
    int  countGoldFaces()       const {
        int c = 0;
        for (int i = 0; i < 6; ++i) if (gold_[i]) ++c;
        return c;
    }

private:
    int  top_, bottom_, north_, south_, east_, west_;
    bool gold_[6];
};

// ============================================================
// State
// Snapshot completo: posicion + orientacion + oro en celdas.
//
// cellGold: bitmask de 64 bits, bit (row*cols+col) encendido
// si esa celda tiene oro. Usamos bitmask (no bool[6]) porque
// CASO 3 puede depositar oro en cualquier celda libre,
// no solo en las 6 posiciones 'G' originales.
// ============================================================
struct State {
    int      row, col;
    Cube     cube;
    uint64_t cellGold;

    State(int row, int col, const Cube& cube, uint64_t cellGold)
        : row(row), col(col), cube(cube), cellGold(cellGold) {}

    bool allGoldCollected() const { return cube.countGoldFaces() == 6; }
};

// ============================================================
// Transition
// ============================================================
using GoldCells = std::array<std::pair<int,int>, 6>;

struct TransitionResult {
    State nextState;
    bool  recogioOro;
};

// Intercambio entre cara inferior y celda destino.
// bottomHadGold se lee ANTES de mutar — deteccion exacta
// al instante fisico del intercambio.
//   CASO 1: celda con oro, cara vacia  -> pickup    (true,  costo B)
//   CASO 2: ambos con oro              -> sin efecto (false, costo A)
//   CASO 3: celda vacia, cara con oro  -> deposito  (false, costo A)
//   CASO 4: ambos vacios               -> sin efecto (false, costo A)
static bool aplicarReglasOro(State& s, int row, int col, int cols) {
    bool     bottomHadGold = s.cube.bottomHasGold();
    uint64_t mask          = 1ULL << (row * cols + col);
    bool     cellHasGold   = (s.cellGold & mask) != 0;

    if (cellHasGold && !bottomHadGold) {
        s.cube.putGoldOnBottom();
        s.cellGold &= ~mask;
        return true;
    }
    if (!cellHasGold && bottomHadGold) {
        s.cube.removeGoldFromBottom();
        s.cellGold |= mask;
    }
    return false;
}

TransitionResult moveNorth(const State& s, int cols) {
    State nx = s; nx.row -= 1; nx.cube.rollNorth();
    return {nx, aplicarReglasOro(nx, nx.row, nx.col, cols)};
}
TransitionResult moveSouth(const State& s, int cols) {
    State nx = s; nx.row += 1; nx.cube.rollSouth();
    return {nx, aplicarReglasOro(nx, nx.row, nx.col, cols)};
}
TransitionResult moveEast(const State& s, int cols) {
    State nx = s; nx.col += 1; nx.cube.rollEast();
    return {nx, aplicarReglasOro(nx, nx.row, nx.col, cols)};
}
TransitionResult moveWest(const State& s, int cols) {
    State nx = s; nx.col -= 1; nx.cube.rollWest();
    return {nx, aplicarReglasOro(nx, nx.row, nx.col, cols)};
}

// ============================================================
// Grid
// ============================================================
class Grid {
public:
    explicit Grid(const std::vector<std::string>& layout)
        : layout_(layout),
          rows_(static_cast<int>(layout.size())),
          cols_(layout.empty() ? 0 : static_cast<int>(layout[0].size())),
          startRow_(-1), startCol_(-1)
    {
        int gi = 0;
        for (int r = 0; r < rows_; ++r)
            for (int c = 0; c < cols_; ++c) {
                char ch = layout_[r][c];
                if      (ch == 'S') { startRow_ = r; startCol_ = c; }
                else if (ch == 'G') { goldCells_[gi++] = {r, c};    }
            }
    }

    int rows()     const { return rows_;     }
    int cols()     const { return cols_;     }
    int startRow() const { return startRow_; }
    int startCol() const { return startCol_; }

    bool isValidMove(int row, int col) const {
        return row >= 0 && row < rows_
            && col >= 0 && col < cols_
            && layout_[row][col] != '#';
    }

    GoldCells getGoldCells() const { return goldCells_; }

private:
    std::vector<std::string> layout_;
    int rows_, cols_, startRow_, startCol_;
    GoldCells goldCells_;
};

std::vector<TransitionResult> generateTransitions(const State& cur, const Grid& grid) {
    std::vector<TransitionResult> tr;
    const int cols = grid.cols();
    if (grid.isValidMove(cur.row - 1, cur.col)) tr.push_back(moveNorth(cur, cols));
    if (grid.isValidMove(cur.row + 1, cur.col)) tr.push_back(moveSouth(cur, cols));
    if (grid.isValidMove(cur.row, cur.col + 1)) tr.push_back(moveEast(cur, cols));
    if (grid.isValidMove(cur.row, cur.col - 1)) tr.push_back(moveWest(cur, cols));
    return tr;
}

// ============================================================
// Cost
// ============================================================
static int movementCost(bool recogioOro, int A, int B) {
    return recogioOro ? B : A;
}

// ============================================================
// StateKey
// Clave compacta y comparable para unordered_map en Dijkstra.
// ============================================================
struct StateKey {
    int row, col;
    int top, bottom, north, south, east, west;
    std::array<bool, 6> cubeGold;
    uint64_t cellGold;

    explicit StateKey(const State& s)
        : row(s.row), col(s.col),
          top   (s.cube.getTop()),    bottom(s.cube.getBottom()),
          north (s.cube.getNorth()),  south (s.cube.getSouth()),
          east  (s.cube.getEast()),   west  (s.cube.getWest()),
          cellGold(s.cellGold)
    {
        for (int i = 0; i < 6; ++i) cubeGold[i] = s.cube.faceHasGold(i);
    }

    bool operator==(const StateKey& o) const {
        return row    == o.row    && col    == o.col
            && top    == o.top    && bottom == o.bottom
            && north  == o.north  && south  == o.south
            && east   == o.east   && west   == o.west
            && cubeGold == o.cubeGold
            && cellGold == o.cellGold;
    }
};

// Hash FNV-1a de 64 bits: empaqueta los 8 campos enteros en
// un uint64_t, comprime cubeGold en 6 bits, mezcla cellGold.
struct StateKeyHash {
    size_t operator()(const StateKey& k) const {
        uint64_t h1 = ((uint64_t)(uint8_t)k.row)
                    | ((uint64_t)(uint8_t)k.col)    <<  8
                    | ((uint64_t)(uint8_t)k.top)    << 16
                    | ((uint64_t)(uint8_t)k.bottom) << 24
                    | ((uint64_t)(uint8_t)k.north)  << 32
                    | ((uint64_t)(uint8_t)k.south)  << 40
                    | ((uint64_t)(uint8_t)k.east)   << 48
                    | ((uint64_t)(uint8_t)k.west)   << 56;
        uint8_t cg = 0;
        for (int i = 0; i < 6; ++i)
            cg |= (k.cubeGold[i] ? 1u : 0u) << i;
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
// Dijkstra
//
// Estado inicial: pos=start, Cube(0,1,2,3,4,5), cellGold=bitmask G's
// Objetivo: allGoldCollected() == true  (countGoldFaces() == 6)
//
// Stale entries: priority_queue no soporta decrease-key.
// Al hacer pop, si currentDist != dist[key] -> entrada obsoleta.
// ============================================================
int solve(const Grid& grid, int A, int B) {
    GoldCells celdasOro = grid.getGoldCells();
    uint64_t cellGoldInicial = 0;
    for (int i = 0; i < 6; ++i)
        cellGoldInicial |= 1ULL << (celdasOro[i].first * grid.cols() + celdasOro[i].second);

    State estadoInicial(grid.startRow(), grid.startCol(),
                        Cube(0, 1, 2, 3, 4, 5),
                        cellGoldInicial);

    using Entry = std::pair<int, State>;
    auto cmp = [](const Entry& a, const Entry& b) { return a.first > b.first; };
    std::priority_queue<Entry, std::vector<Entry>, decltype(cmp)> pq(cmp);
    std::unordered_map<StateKey, int, StateKeyHash> dist;

    dist[StateKey(estadoInicial)] = 0;
    pq.push({0, estadoInicial});

    while (!pq.empty()) {
        auto [currentDist, current] = pq.top();
        pq.pop();

        StateKey currentKey(current);
        auto it = dist.find(currentKey);
        if (it == dist.end() || currentDist != it->second) continue;

        if (current.allGoldCollected()) return currentDist;

        for (const TransitionResult& tr : generateTransitions(current, grid)) {
            int newDist = currentDist + movementCost(tr.recogioOro, A, B);
            StateKey nextKey(tr.nextState);
            auto nextIt = dist.find(nextKey);
            if (nextIt == dist.end() || newDist < nextIt->second) {
                dist[nextKey] = newDist;
                pq.push({newDist, tr.nextState});
            }
        }
    }
    return -1;
}

// ============================================================
// main
// ============================================================
int main() {
    int T;
    std::cin >> T;
    while (T--) {
        int R, C, A, B;
        std::cin >> R >> C >> A >> B;
        std::vector<std::string> layout(R);
        for (int i = 0; i < R; ++i) std::cin >> layout[i];
        Grid grid(layout);
        int result = solve(grid, A, B);
        if (result == -1)
            std::cout << "Oh my God, they killed Kenny!\n";
        else
            std::cout << "Screw you guys, I got all the gold for " << result << " cost!\n";
    }
    return 0;
}
