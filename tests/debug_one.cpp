#include "../solver/Solver.h"
#include "../grid/Grid.h"
#include <iostream>
#include <string>
#include <vector>
int main() {
    std::vector<std::string> layout = {"..G","GGG","GGS"};
    Grid g(layout);
    int result = solve(g, 2, 9, true);
    std::cout << "Result: " << result << "\n";
    return 0;
}
