#include "solver/Solver.h"
#include "grid/Grid.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    int T;
    std::cin >> T;

    while (T--) {
        int R, C, A, B;
        std::cin >> R >> C >> A >> B;

        std::vector<std::string> layout(R);
        for (int i = 0; i < R; ++i)
            std::cin >> layout[i];

        Grid grid(layout);
        int result = solve(grid, A, B);

        if (result == -1)
            std::cout << "Oh my God, they killed Kenny!\n";
        else
            std::cout << "Screw you guys, I got all the gold for " << result << " cost!\n";
    }

    return 0;
}
