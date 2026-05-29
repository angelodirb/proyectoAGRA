// ============================================================
// cube_tests.cpp
// Pruebas unitarias para la clase Cube.
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cube/Cube.cpp tests/cube_tests.cpp -o cube_tests
//   ./cube_tests
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <random>
#include "../cube/Cube.h"

// ------------------------------------------------------------
// Utilidades de comparacion y reporte
// ------------------------------------------------------------

// Compara dos cubos cara a cara
bool cubesEqual(const Cube& a, const Cube& b) {
    return a.getTop()    == b.getTop()    &&
           a.getBottom() == b.getBottom() &&
           a.getNorth()  == b.getNorth()  &&
           a.getSouth()  == b.getSouth()  &&
           a.getEast()   == b.getEast()   &&
           a.getWest()   == b.getWest();
}

// Imprime el resultado de un test con nombre
void reportTest(const std::string& name, bool passed) {
    if (passed) {
        std::cout << "[PASS] " << name << "\n";
    } else {
        std::cout << "[FAIL] " << name << "\n";
    }
}

// Cubo de referencia con IDs en [0,5] (requerido por el sistema de oro).
//   top=0, bottom=1, north=2, south=3, east=4, west=5
Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// ============================================================
// TEST 1 — rollNorth() + rollSouth() = identidad
// Rodar hacia el norte y luego hacia el sur debe
// devolver el cubo exactamente al estado inicial.
// ============================================================
bool test1_northSouthIdentity() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    c.rollNorth();
    c.rollSouth();

    return cubesEqual(c, original);
}

// ============================================================
// TEST 2 — rollEast() + rollWest() = identidad
// Rodar hacia el este y luego hacia el oeste debe
// devolver el cubo exactamente al estado inicial.
// ============================================================
bool test2_eastWestIdentity() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    c.rollEast();
    c.rollWest();

    return cubesEqual(c, original);
}

// ============================================================
// TEST 3 — 4 x rollNorth() = orientacion original
// Un cubo que rueda 4 veces en la misma direccion da
// una vuelta completa y vuelve a su estado inicial.
// ============================================================
bool test3_fourRollsNorth() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    c.rollNorth();
    c.rollNorth();
    c.rollNorth();
    c.rollNorth();

    return cubesEqual(c, original);
}

// ============================================================
// TEST 4 — 4 x rollEast() = orientacion original
// Igual que el TEST 3 pero en el eje este-oeste.
// ============================================================
bool test4_fourRollsEast() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    c.rollEast();
    c.rollEast();
    c.rollEast();
    c.rollEast();

    return cubesEqual(c, original);
}

// ============================================================
// TEST 5 — Secuencias mezcladas
//
// Cada sub-test aplica una secuencia de movimientos que
// matematicamente debe terminar en el estado original.
// Son propiedades conocidas del grupo de rotaciones del cubo.
//
// 5a) North, East, South, West  (cuadrado en sentido horario)
//     Dar una vuelta completa alrededor del cubo regresa
//     al estado inicial.
//
// 5b) 4 x (North + East)
//     Secuencia de periodo 4 sobre dos ejes.
//
// 5c) North x2, East x2, South x2, West x2
//     Doble vuelta en cada direccion: periodo 2 por eje,
//     la combinacion tambien es identidad.
//
// 5d) North, East, North, West, South, West, South, East
//     Secuencia simetrica de 8 pasos que cancela exactamente.
//
// 5e) 12 x (North + East)
//     El orden del subgrupo generado por N y E divide a 12,
//     por lo que 12 aplicaciones dan la identidad.
// ============================================================

// 5a: (N E S W)^3 = identidad
// El orden del elemento NESW en el grupo de rotacion es 3,
// por lo que aplicarlo 3 veces devuelve el estado original.
bool test5a_NESW_times3() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    for (int i = 0; i < 3; ++i) {
        c.rollNorth();
        c.rollEast();
        c.rollSouth();
        c.rollWest();
    }

    return cubesEqual(c, original);
}

// 5b: (N E)^3 = identidad
// El orden del elemento NE en el grupo de rotacion es 3.
// Tres aplicaciones de (rollNorth + rollEast) devuelven el estado original.
bool test5b_NE_times3() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    for (int i = 0; i < 3; ++i) {
        c.rollNorth();
        c.rollEast();
    }

    return cubesEqual(c, original);
}

// 5c: NN EE SS WW = identidad
bool test5c_NN_EE_SS_WW() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    c.rollNorth(); c.rollNorth();
    c.rollEast();  c.rollEast();
    c.rollSouth(); c.rollSouth();
    c.rollWest();  c.rollWest();

    return cubesEqual(c, original);
}

// 5d: N E N W S W = identidad  (6 pasos)
// Secuencia simetrica verificada empiricamente:
// cada par de movimientos opuestos se cancela de forma no trivial
// gracias al orden de las rotaciones intermedias.
bool test5d_NENWSW() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    c.rollNorth();
    c.rollEast();
    c.rollNorth();
    c.rollWest();
    c.rollSouth();
    c.rollWest();

    return cubesEqual(c, original);
}

// 5e: (N E) x12 = identidad  (orden del subgrupo)
bool test5e_NE_times12() {
    Cube c = makeFreshCube();
    Cube original = makeFreshCube();

    for (int i = 0; i < 12; ++i) {
        c.rollNorth();
        c.rollEast();
    }

    return cubesEqual(c, original);
}

// ============================================================
// HERRAMIENTAS DE CONSISTENCIA ESTRUCTURAL
// ============================================================

// ------------------------------------------------------------
// isValidCube
// Verifica que el cubo tenga exactamente 6 IDs distintos.
// Detecta los bugs mas comunes de rotaciones incorrectas:
//   - una cara sobrescribio a otra (duplicado)
//   - una cara nunca se asigno (perdida)
//   - se usaron variables temporales incorrectas
// ------------------------------------------------------------
bool isValidCube(const Cube& c) {
    std::set<int> ids = {
        c.getTop(),
        c.getBottom(),
        c.getNorth(),
        c.getSouth(),
        c.getEast(),
        c.getWest()
    };
    // Si hay duplicados el set tendra menos de 6 elementos
    return ids.size() == 6;
}

// Imprime todas las caras del cubo (para diagnostico al fallar)
void printCubeState(const Cube& c) {
    std::cout << "    top="    << c.getTop()
              << " bot="    << c.getBottom()
              << " N="      << c.getNorth()
              << " S="      << c.getSouth()
              << " E="      << c.getEast()
              << " W="      << c.getWest() << "\n";
}

// Aplica un movimiento individual dado como char: 'N','S','E','W'
void applyMove(Cube& c, char move) {
    switch (move) {
        case 'N': c.rollNorth(); break;
        case 'S': c.rollSouth(); break;
        case 'E': c.rollEast();  break;
        case 'W': c.rollWest();  break;
    }
}

// Nombre legible para cada movimiento
std::string moveName(char m) {
    switch (m) {
        case 'N': return "rollNorth";
        case 'S': return "rollSouth";
        case 'E': return "rollEast";
        case 'W': return "rollWest";
        default:  return "?";
    }
}

// ------------------------------------------------------------
// runSequenceAndValidate
// Ejecuta una secuencia de movimientos sobre el cubo.
// Despues de CADA movimiento verifica isValidCube().
// Si falla, imprime:
//   - nombre de la secuencia
//   - paso exacto que fallo
//   - movimiento que causo el fallo
//   - estado actual del cubo
// Retorna true si todos los pasos son validos.
// ------------------------------------------------------------
bool runSequenceAndValidate(Cube& c,
                            const std::vector<char>& moves,
                            const std::string& seqName) {
    for (int i = 0; i < (int)moves.size(); ++i) {
        applyMove(c, moves[i]);

        if (!isValidCube(c)) {
            std::cout << "  [ERROR] Secuencia \"" << seqName << "\"\n";
            std::cout << "  Fallo en paso " << (i + 1)
                      << " de " << moves.size()
                      << " (movimiento: " << moveName(moves[i]) << ")\n";
            std::cout << "  Estado del cubo al fallar:\n";
            printCubeState(c);
            return false;
        }
    }
    return true;
}

// ============================================================
// TEST 6 — NESW repetido muchas veces
// Valida el cubo tras cada uno de los 80 movimientos.
// Detecta bugs que solo aparecen despues de varias rotaciones.
// ============================================================
bool test6_NESW_repeated() {
    Cube c = makeFreshCube();

    // Repetir NESW 20 veces = 80 movimientos en total
    std::vector<char> moves;
    for (int i = 0; i < 20; ++i) {
        moves.push_back('N');
        moves.push_back('E');
        moves.push_back('S');
        moves.push_back('W');
    }

    return runSequenceAndValidate(c, moves, "NESW x20");
}

// ============================================================
// TEST 7 — Secuencia fija mezclada
// Patron no trivial que ejercita los 4 ejes en orden irregular.
// Se repite 10 veces para aumentar la cobertura.
// ============================================================
bool test7_fixedMixedSequence() {
    Cube c = makeFreshCube();

    // Patron base: N N E W S E W N (8 pasos)
    // Repetido 10 veces = 80 movimientos
    std::vector<char> pattern = {'N','N','E','W','S','E','W','N'};
    std::vector<char> moves;
    for (int i = 0; i < 10; ++i) {
        moves.insert(moves.end(), pattern.begin(), pattern.end());
    }

    return runSequenceAndValidate(c, moves, "NNEWSEWN x10");
}

// ============================================================
// TEST 8 — 100 rotaciones aleatorias
// Usa una semilla fija para que el test sea reproducible.
// Si falla, la semilla permite reproducir exactamente el mismo
// orden de movimientos para debuggear.
// ============================================================
bool test8_randomSequence() {
    const unsigned int SEED = 42;   // semilla fija: reproducible
    const int NUM_MOVES = 100;

    std::mt19937 rng(SEED);
    std::uniform_int_distribution<int> dist(0, 3); // 0=N 1=S 2=E 3=W
    const char directions[] = {'N', 'S', 'E', 'W'};

    Cube c = makeFreshCube();
    std::vector<char> moves;
    moves.reserve(NUM_MOVES);

    for (int i = 0; i < NUM_MOVES; ++i) {
        moves.push_back(directions[dist(rng)]);
    }

    bool ok = runSequenceAndValidate(c, moves, "100 aleatorias (seed=42)");

    if (!ok) {
        // Imprimir la secuencia completa para reproducir el bug
        std::cout << "  Secuencia completa generada:\n  ";
        for (char m : moves) std::cout << m << " ";
        std::cout << "\n";
    }

    return ok;
}

// ============================================================
// TESTS DE ORO
// ============================================================

// ============================================================
// TEST 9 — El oro viaja con la cara fisica al rotar
//
// Poner oro abajo, rotar, verificar que el oro sigue a la
// cara fisica (no a la posicion espacial "bottom").
// ============================================================
bool test9_goldTravelsWithFace() {
    Cube c = makeFreshCube();
    // Estado inicial: bottom_ == 1 (cara fisica 1)

    c.putGoldOnBottom();   // oro en cara fisica 1

    // Verificar estado inicial
    if (!c.bottomHasGold()) {
        std::cout << "  [ERROR] putGoldOnBottom() no funciono\n";
        return false;
    }
    if (c.countGoldFaces() != 1) {
        std::cout << "  [ERROR] countGoldFaces() deberia ser 1, es "
                  << c.countGoldFaces() << "\n";
        return false;
    }

    // rollNorth: bottom(1) -> south
    // La cara fisica 1 ahora esta en south, no en bottom.
    // bottom ahora tiene la cara fisica que estaba en north (cara 2).
    c.rollNorth();

    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] Despues de rollNorth, bottom NO deberia tener oro\n";
        std::cout << "         (el oro viajo con la cara fisica al south)\n";
        return false;
    }

    // Rotar 3 veces mas hacia el norte para completar el ciclo
    // La cara fisica 1 pasa: south -> top -> north -> bottom
    c.rollNorth();  // south(1) -> top
    c.rollNorth();  // top(1) -> north
    c.rollNorth();  // north(1) -> bottom   <-- el oro vuelve abajo

    if (!c.bottomHasGold()) {
        std::cout << "  [ERROR] Despues de 4 rollNorth, la cara con oro\n";
        std::cout << "         deberia estar de nuevo en bottom\n";
        return false;
    }

    return true;
}

// ============================================================
// TEST 10 — Oro en multiples caras, el conteo se conserva
//
// Poner oro en varias caras, ejecutar secuencias largas,
// verificar que countGoldFaces() nunca cambia.
// ============================================================
bool test10_goldCountConserved() {
    Cube c = makeFreshCube();

    // Poner oro en bottom (cara 1) y luego rotar y poner en otro bottom
    c.putGoldOnBottom();   // cara fisica 1
    c.rollNorth();         // ahora bottom = cara 2
    c.putGoldOnBottom();   // cara fisica 2
    c.rollEast();          // ahora bottom = cara 4
    c.putGoldOnBottom();   // cara fisica 4

    int expectedGold = c.countGoldFaces();  // deberia ser 3
    if (expectedGold != 3) {
        std::cout << "  [ERROR] Se pusieron 3 oros pero countGoldFaces() = "
                  << expectedGold << "\n";
        return false;
    }

    // Ejecutar 200 rotaciones mezcladas y verificar que el conteo no cambia
    const char dirs[] = {'N', 'S', 'E', 'W'};
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, 3);

    for (int i = 0; i < 200; ++i) {
        applyMove(c, dirs[dist(rng)]);
        int currentGold = c.countGoldFaces();
        if (currentGold != expectedGold) {
            std::cout << "  [ERROR] Paso " << i << ": countGoldFaces() = "
                      << currentGold << ", esperado " << expectedGold << "\n";
            c.printGoldState();
            return false;
        }
    }

    return true;
}

// ============================================================
// TEST 11 — bottomHasGold() despues de varias rotaciones
//
// Verificar que bottomHasGold() reporta correctamente
// despues de secuencias conocidas de rotacion.
// ============================================================
bool test11_bottomHasGoldAfterRotations() {
    Cube c = makeFreshCube();

    // Poner oro en la cara fisica que esta abajo inicialmente (cara 1)
    c.putGoldOnBottom();

    // Definir secuencia y resultado esperado para bottomHasGold()
    // despues de cada movimiento.
    //
    // Estado inicial: top=0, bottom=1[ORO], north=2, south=3, east=4, west=5
    //
    // rollEast:  bottom queda con cara que venia de east (4)  -> sin oro
    // rollEast:  bottom queda con cara que venia de east (0)  -> sin oro
    // rollNorth: bottom queda con cara que venia de north (?) -> depende
    // rollWest:  ...
    // Mejor rastrear manualmente:

    // Estado: T=0 B=1* N=2 S=3 E=4 W=5   (* = oro)
    c.rollEast();
    // T=5 B=4 N=2 S=3 E=0 W=1*
    // bottom = cara 4 -> sin oro
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] Paso 1 (rollEast): bottom no deberia tener oro\n";
        return false;
    }

    c.rollEast();
    // T=1* B=0 N=2 S=3 E=5 W=4
    // bottom = cara 0 -> sin oro
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] Paso 2 (rollEast): bottom no deberia tener oro\n";
        return false;
    }

    c.rollEast();
    // T=4 B=5 N=2 S=3 E=1* W=0
    // bottom = cara 5 -> sin oro
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] Paso 3 (rollEast): bottom no deberia tener oro\n";
        return false;
    }

    c.rollEast();
    // T=0 B=1* N=2 S=3 E=4 W=5  (vuelta completa)
    // bottom = cara 1 -> CON ORO
    if (!c.bottomHasGold()) {
        std::cout << "  [ERROR] Paso 4 (rollEast): bottom DEBERIA tener oro\n";
        std::cout << "         (4 rollEast = vuelta completa)\n";
        return false;
    }

    // Ahora rollNorth
    c.rollNorth();
    // T=3 B=2 N=0 S=1* E=4 W=5
    // bottom = cara 2 -> sin oro
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] Paso 5 (rollNorth): bottom no deberia tener oro\n";
        return false;
    }

    c.rollNorth();
    // T=1* B=3 N=3? ... let me recalculate
    // Antes: T=3 B=2 N=0 S=1* E=4 W=5
    // rollNorth: T=oldS=1*, N=oldT=3, B=oldN=0, S=oldB=2
    // T=1* B=0 N=3 S=2 E=4 W=5
    // bottom = cara 0 -> sin oro, top = cara 1 -> CON ORO
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] Paso 6 (rollNorth): bottom no deberia tener oro\n";
        return false;
    }
    if (!c.topHasGold()) {
        std::cout << "  [ERROR] Paso 6 (rollNorth): top DEBERIA tener oro\n";
        return false;
    }

    return true;
}

// ============================================================
// TEST 12 — removeGoldFromBottom() funciona correctamente
//
// Verificar que se puede poner y quitar oro, y que los conteos
// se actualizan correctamente.
// ============================================================
bool test12_removeGoldFromBottom() {
    Cube c = makeFreshCube();

    // Sin oro inicialmente
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] El cubo no deberia tener oro al inicio\n";
        return false;
    }
    if (c.countGoldFaces() != 0) {
        std::cout << "  [ERROR] countGoldFaces() deberia ser 0 al inicio\n";
        return false;
    }

    // Poner oro
    c.putGoldOnBottom();
    if (!c.bottomHasGold()) {
        std::cout << "  [ERROR] bottomHasGold() deberia ser true despues de put\n";
        return false;
    }
    if (c.countGoldFaces() != 1) {
        std::cout << "  [ERROR] countGoldFaces() deberia ser 1\n";
        return false;
    }

    // Quitar oro
    c.removeGoldFromBottom();
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] bottomHasGold() deberia ser false despues de remove\n";
        return false;
    }
    if (c.countGoldFaces() != 0) {
        std::cout << "  [ERROR] countGoldFaces() deberia ser 0 despues de remove\n";
        return false;
    }

    // Poner oro, rotar, quitar oro del nuevo bottom (diferente cara)
    c.putGoldOnBottom();   // oro en cara 1
    c.rollNorth();         // bottom ahora es cara 2, sin oro
    c.putGoldOnBottom();   // oro en cara 2  (total: 2 oros)
    if (c.countGoldFaces() != 2) {
        std::cout << "  [ERROR] Deberia haber 2 caras con oro\n";
        return false;
    }

    c.removeGoldFromBottom();  // quitar oro de cara 2 (total: 1 oro)
    if (c.countGoldFaces() != 1) {
        std::cout << "  [ERROR] Deberia haber 1 cara con oro despues de remove\n";
        return false;
    }
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] bottom no deberia tener oro despues de remove\n";
        return false;
    }

    // La cara 1 todavia deberia tener oro (esta en south ahora)
    // Verificar rotando de vuelta
    c.rollSouth();  // bottom vuelve a ser cara 1
    if (!c.bottomHasGold()) {
        std::cout << "  [ERROR] La otra cara (cara 1) deberia seguir con oro\n";
        return false;
    }

    // Doble remove es idempotente
    c.removeGoldFromBottom();
    c.removeGoldFromBottom();  // no deberia causar problemas
    if (c.bottomHasGold()) {
        std::cout << "  [ERROR] bottomHasGold() deberia ser false\n";
        return false;
    }
    if (c.countGoldFaces() != 0) {
        std::cout << "  [ERROR] countGoldFaces() deberia ser 0\n";
        return false;
    }

    return true;
}

// ============================================================
// TEST 13 — Stress test aleatorio para oro
//
// Ejecuta secuencias aleatorias y valida:
//   - nunca se pierde oro
//   - nunca aparece oro extra
//   - countGoldFaces() permanece consistente
//
// Usa std::mt19937 con seed fija para reproducibilidad.
// ============================================================
bool test13_goldRandomStress() {
    const unsigned int SEED = 99;
    const int NUM_ITERATIONS = 500;

    std::mt19937 rng(SEED);
    std::uniform_int_distribution<int> moveDist(0, 3);
    std::uniform_int_distribution<int> actionDist(0, 9);
    const char dirs[] = {'N', 'S', 'E', 'W'};

    Cube c = makeFreshCube();
    int expectedGold = 0;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int action = actionDist(rng);

        if (action <= 6) {
            // 70% del tiempo: rotar
            char dir = dirs[moveDist(rng)];
            applyMove(c, dir);
        } else if (action == 7) {
            // 10%: poner oro abajo
            if (!c.bottomHasGold()) {
                c.putGoldOnBottom();
                expectedGold++;
            }
        } else if (action == 8) {
            // 10%: quitar oro abajo
            if (c.bottomHasGold()) {
                c.removeGoldFromBottom();
                expectedGold--;
            }
        } else {
            // 10%: rotar dos veces
            applyMove(c, dirs[moveDist(rng)]);
            applyMove(c, dirs[moveDist(rng)]);
        }

        // Verificar invariante: countGoldFaces == expectedGold
        int currentGold = c.countGoldFaces();
        if (currentGold != expectedGold) {
            std::cout << "  [ERROR] Iteracion " << i
                      << ": countGoldFaces() = " << currentGold
                      << ", esperado " << expectedGold << "\n";
            std::cout << "  Accion: " << action << "\n";
            c.printGoldState();
            return false;
        }

        // Verificar que el cubo sigue siendo estructuralmente valido
        if (!isValidCube(c)) {
            std::cout << "  [ERROR] Iteracion " << i
                      << ": cubo invalido (IDs duplicados)\n";
            printCubeState(c);
            return false;
        }
    }

    return true;
}

// ============================================================
// main — ejecuta todos los tests y muestra resumen
// ============================================================
int main() {
    std::cout << "========================================\n";
    std::cout << "  CUBE TESTS\n";
    std::cout << "  Estado inicial: top=0 bot=1 N=2 S=3 E=4 W=5\n";
    std::cout << "========================================\n\n";

    int passed = 0;
    int total  = 0;

    // --- TEST 1 ---
    std::cout << "-- TEST 1: rollNorth + rollSouth = identidad\n";
    {
        bool ok = test1_northSouthIdentity();
        reportTest("rollNorth -> rollSouth == identidad", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 2 ---
    std::cout << "-- TEST 2: rollEast + rollWest = identidad\n";
    {
        bool ok = test2_eastWestIdentity();
        reportTest("rollEast -> rollWest == identidad", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 3 ---
    std::cout << "-- TEST 3: 4 x rollNorth = orientacion original\n";
    {
        bool ok = test3_fourRollsNorth();
        reportTest("rollNorth x4 == identidad", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 4 ---
    std::cout << "-- TEST 4: 4 x rollEast = orientacion original\n";
    {
        bool ok = test4_fourRollsEast();
        reportTest("rollEast x4 == identidad", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 5 ---
    std::cout << "-- TEST 5: Secuencias mezcladas\n";
    {
        bool ok5a = test5a_NESW_times3();
        reportTest("5a) (N->E->S->W) x3 == identidad  [orden=3]", ok5a);
        passed += ok5a; total++;

        bool ok5b = test5b_NE_times3();
        reportTest("5b) (N->E) x3 == identidad  [orden=3]", ok5b);
        passed += ok5b; total++;

        bool ok5c = test5c_NN_EE_SS_WW();
        reportTest("5c) NN->EE->SS->WW == identidad", ok5c);
        passed += ok5c; total++;

        bool ok5d = test5d_NENWSW();
        reportTest("5d) N->E->N->W->S->W == identidad  [6 pasos]", ok5d);
        passed += ok5d; total++;

        bool ok5e = test5e_NE_times12();
        reportTest("5e) (N->E) x12 == identidad", ok5e);
        passed += ok5e; total++;
    }
    std::cout << "\n";

    // --- TEST 6 ---
    std::cout << "-- TEST 6: NESW repetido 20 veces (80 movimientos)\n";
    {
        bool ok = test6_NESW_repeated();
        reportTest("isValidCube() valido tras cada paso de NESW x20", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 7 ---
    std::cout << "-- TEST 7: Secuencia NNEWSEWN repetida 10 veces (80 movimientos)\n";
    {
        bool ok = test7_fixedMixedSequence();
        reportTest("isValidCube() valido tras cada paso de NNEWSEWN x10", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 8 ---
    std::cout << "-- TEST 8: 100 rotaciones aleatorias (seed=42)\n";
    {
        bool ok = test8_randomSequence();
        reportTest("isValidCube() valido tras cada uno de los 100 pasos", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // ========================================================
    // TESTS DE ORO
    // ========================================================
    std::cout << "========================================\n";
    std::cout << "  GOLD TESTS\n";
    std::cout << "========================================\n\n";

    // --- TEST 9 ---
    std::cout << "-- TEST 9: El oro viaja con la cara fisica\n";
    {
        bool ok = test9_goldTravelsWithFace();
        reportTest("Oro en bottom viaja con la cara al rotar", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 10 ---
    std::cout << "-- TEST 10: Oro en multiples caras, conteo se conserva\n";
    {
        bool ok = test10_goldCountConserved();
        reportTest("countGoldFaces() constante tras 200 rotaciones", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 11 ---
    std::cout << "-- TEST 11: bottomHasGold() despues de varias rotaciones\n";
    {
        bool ok = test11_bottomHasGoldAfterRotations();
        reportTest("bottomHasGold()/topHasGold() correctos tras rotaciones", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 12 ---
    std::cout << "-- TEST 12: removeGoldFromBottom() funciona correctamente\n";
    {
        bool ok = test12_removeGoldFromBottom();
        reportTest("put/remove/idempotencia de oro", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- TEST 13 ---
    std::cout << "-- TEST 13: Stress test aleatorio para oro (seed=99, 500 iters)\n";
    {
        bool ok = test13_goldRandomStress();
        reportTest("Invariantes de oro mantenidas en 500 iteraciones aleatorias", ok);
        passed += ok; total++;
    }
    std::cout << "\n";

    // --- Resumen ---
    std::cout << "========================================\n";
    std::cout << "  Resultado: " << passed << " / " << total << " tests pasaron\n";
    if (passed == total) {
        std::cout << "  *** TODOS LOS TESTS PASARON ***\n";
    } else {
        std::cout << "  *** HAY TESTS FALLIDOS — revisa la logica de rotacion ***\n";
    }
    std::cout << "========================================\n";

    return (passed == total) ? 0 : 1;
}
