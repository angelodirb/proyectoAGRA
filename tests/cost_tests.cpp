// ============================================================
// cost_tests.cpp
// Pruebas unitarias para movementCost(bool recogioOro, int A, int B).
//
// La nueva firma recibe el evento fisico directamente desde
// TransitionResult::recogioOro — no necesita inspeccionar
// State ni Cube. Los tests reflejan esa simplificacion.
//
// Cubre:
//   TEST 1 — pickup real (recogioOro=true)   -> retorna B
//   TEST 2 — no pickup   (recogioOro=false)  -> retorna A
//   TEST 3 — A == B (edge case)                -> mismo valor siempre
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cost/Cost.cpp tests/cost_tests.cpp -o cost_tests
//   ./cost_tests
//
// Nota: esta suite NO incluye Cube, State, Grid ni Transition.
// Cost depende unicamente de bool + int. Separacion maxima.
// ============================================================

#include <iostream>
#include <string>
#include "../cost/Cost.h"

static int testsPassed = 0;
static int testsFailed = 0;

void reportTest(const std::string& name, bool passed) {
    if (passed) { std::cout << "[PASS] " << name << "\n"; ++testsPassed; }
    else         { std::cout << "[FAIL] " << name << "\n"; ++testsFailed; }
}

// ============================================================
// TEST 1 — Pickup real (recogioOro = true) -> retorna B
//
// Transition reporto CASO 1: la celda tenia oro y la cara
// inferior estaba vacia antes del intercambio.
// Cost debe retornar B sin importar el valor de A.
// ============================================================
void test1_pickupReal() {
    std::cout << "--- TEST 1: Pickup real (recogioOro=true) ---\n";

    reportTest("test1a: A=1,  B=5   -> retorna B=5",   movementCost(true, 1, 5)   == 5);
    reportTest("test1b: A=10, B=3   -> retorna B=3",   movementCost(true, 10, 3)  == 3);
    reportTest("test1c: A=0,  B=100 -> retorna B=100", movementCost(true, 0, 100) == 100);
    reportTest("test1d: A=18, B=1   -> retorna B=1",   movementCost(true, 18, 1)  == 1);
}

// ============================================================
// TEST 2 — No pickup (recogioOro = false) -> retorna A
//
// Cubre CASO 2, CASO 3 y CASO 4 (recogioOro=false en todos):
//   - CASO 2: ambos tienen oro (sin efecto neto).
//   - CASO 3: cara inferior con oro, celda vacia (deposito).
//   - CASO 4: movimiento a celda sin oro, cara inferior vacia.
// Cost debe retornar A.
// ============================================================
void test2_noPickup() {
    std::cout << "\n--- TEST 2: No pickup (recogioOro=false) ---\n";

    reportTest("test2a: A=1,  B=9  -> retorna A=1",  movementCost(false, 1, 9)   == 1);
    reportTest("test2b: A=3,  B=20 -> retorna A=3",  movementCost(false, 3, 20)  == 3);
    reportTest("test2c: A=2,  B=7  -> retorna A=2",  movementCost(false, 2, 7)   == 2);
    reportTest("test2d: A=18, B=1  -> retorna A=18", movementCost(false, 18, 1)  == 18);
}

// ============================================================
// TEST 3 — A == B (edge case)
//
// Si ambos costos son iguales, movementCost debe retornar
// ese valor para cualquier valor de recogioOro.
// Verifica que la funcion no tiene valores magicos ni calcula
// un costo distinto de A o B.
// ============================================================
void test3_aIgualB() {
    std::cout << "\n--- TEST 3: A == B ---\n";

    reportTest("test3a: pickup,    A==B==7  -> 7",  movementCost(true,  7, 7)  == 7);
    reportTest("test3b: no pickup, A==B==7  -> 7",  movementCost(false, 7, 7)  == 7);
    reportTest("test3c: pickup,    A==B==1  -> 1",  movementCost(true,  1, 1)  == 1);
    reportTest("test3d: no pickup, A==B==18 -> 18", movementCost(false, 18, 18) == 18);
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "===========================================\n";
    std::cout << "  Cost Tests\n";
    std::cout << "===========================================\n\n";

    test1_pickupReal();
    test2_noPickup();
    test3_aIgualB();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
