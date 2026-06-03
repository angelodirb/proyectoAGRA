# Cartman and the Golden Cube

Solver de camino mínimo para un cubo rodante que recolecta oro sobre una grilla.  
Implementado en **C++17**. Algoritmo central: **Dijkstra** sobre un espacio de estados implícito.

---

## 1. Problema

Un cubo unitario rueda sobre una grilla rectangular. La grilla contiene exactamente **6 piezas de oro** en celdas marcadas como `G`. El objetivo es que el cubo recolecte las 6 piezas simultáneamente — una en cada cara física.

### La grilla

| Carácter | Significado              |
|----------|--------------------------|
| `.`      | celda libre              |
| `#`      | pared (no transitable)   |
| `S`      | posición inicial del cubo|
| `G`      | celda con una pieza de oro|

Ejemplo de entrada:

```
. . G
G G G
G G S
```

_3×3, costos A=2, B=9. El cubo parte desde `S` (fila 2, col 2)._

### Costos

Cada movimiento tiene un costo entero:

| Evento                               | Costo |
|--------------------------------------|-------|
| Movimiento que **recoge oro** (CASO 1) | **B** |
| Cualquier otro movimiento            | **A** |

### Objetivo

Encontrar la secuencia de movimientos de **costo total mínimo** hasta que las 6 caras físicas del cubo tengan oro simultáneamente.

---

## 2. Modelado del estado

> El estado debe contener toda la información necesaria para que el futuro del sistema sea determinístico. Dos estados idénticos deben tener exactamente el mismo conjunto de estados alcanzables.

El estado completo del problema requiere tres componentes:

| Campo        | Tipo       | Qué captura                                     |
|--------------|------------|-------------------------------------------------|
| `row`, `col` | `int`      | Posición del cubo en la grilla                  |
| `cube`       | `Cube`     | Orientación 3D + oro en caras físicas           |
| `cellGold`   | `uint64_t` | Qué celdas de la grilla tienen oro actualmente  |

### ¿Por qué los tres son necesarios?

- **Solo posición** — no alcanza. El mismo casillero con distinta orientación del cubo tiene distinta cara abajo: distintas posibilidades de recoger o depositar oro.
- **Posición + orientación** — no alcanza. La misma posición y orientación con distinta distribución de oro en la grilla lleva a futuros completamente distintos.
- **Los tres juntos** capturan exactamente lo que determina el siguiente estado.

### Coordenadas

```
(0,0) ──────────────▶  col  (crece al este)
  │
  ▼
 row  (crece al sur)
```

- `row -= 1` → movimiento al norte
- `col += 1` → movimiento al este

---

## 3. Rotaciones del cubo

El cubo tiene **6 caras físicas**, cada una con un identificador entero fijo (0..5). Al rodar, las caras cambian de posición espacial — pero cada cara mantiene su identidad y su contenido.

### Posiciones espaciales (net del cubo)

```
           ┌──────────┐
           │   TOP    │
  ┌────────┼──────────┼────────┐
  │  WEST  │  NORTH   │  EAST  │
  └────────┼──────────┼────────┘
           │  BOTTOM  │
           ├──────────┤
           │  SOUTH   │
           └──────────┘
```

_La cara que queda en posición BOTTOM al llegar a una celda es la que interactúa con el oro de esa celda._

### Efecto de cada rotación

| Movimiento  | Ciclo de posiciones espaciales              | Posiciones fijas |
|-------------|---------------------------------------------|------------------|
| `rollNorth` | top → north → bottom → south → top         | east, west       |
| `rollSouth` | top → south → bottom → north → top         | east, west       |
| `rollEast`  | top → east  → bottom → west  → top         | north, south     |
| `rollWest`  | top → west  → bottom → east  → top         | north, south     |

### El oro pertenece a las caras físicas, no a las posiciones espaciales

> **Cuando el cubo rota, el oro viaja con la cara física — no queda en la posición espacial.**

Ejemplo partiendo de la orientación canónica `(top=0, bottom=1, north=2, south=3, east=4, west=5)`:

```
Antes de rollNorth:          Después de rollNorth:
  top    = cara 0    →         top    = cara 3
  bottom = cara 1    →         bottom = cara 2
  north  = cara 2    →         north  = cara 0
  south  = cara 3    →         south  = cara 1
  east   = cara 4    →         east   = cara 4   (sin cambio)
  west   = cara 5    →         west   = cara 5   (sin cambio)
```

Si la cara física `2` tenía oro y estaba al norte, después de `rollNorth` esa misma cara pasa a estar en posición BOTTOM — **y su oro llega con ella** al intercambio con la celda destino.

---

## 4. Representación del oro en celdas: `uint64_t cellGold`

### El diseño original y su limitación

El primer diseño rastreaba el oro en celdas con `bool remainingGold[6]`: un booleano por cada una de las 6 posiciones `G` originales del mapa. La hipótesis implícita era que el oro, una vez recogido, nunca volvería a la grilla.

Esa hipótesis es incorrecta.

### El descubrimiento: CASO 3

Al modelar la interacción entre el cubo y una celda vacía, surge este caso:

> Si la cara inferior del cubo **tiene oro** y la celda destino está **vacía**, el cubo deposita esa pieza en la celda.

Ese depósito puede ocurrir en **cualquier celda libre de la grilla** — no solo en las 6 posiciones `G` originales. Con `bool[6]` ese estado es **irrepresentable**: no existe un índice para una celda que no sea una de las 6 originales.

### La solución: bitmask de 64 bits

```
cellGold : uint64_t
```

El bit en la posición `(row × cols + col)` está encendido si esa celda tiene oro actualmente.

*¿Por qué `uint64_t` es suficiente?*
Las restricciones del problema garantizan `R, C ≤ 8`, lo que da un máximo de `8 × 8 = 64` celdas — exactamente el número de bits disponibles en un `uint64_t`.

### Ejemplo concreto

Tomando el caso de la sección 1 (grilla 3×3, `cols = 3`):

```
     col0  col1  col2
     ┌─────┬─────┬─────┐
fila0│  .  │  .  │  G  │   →  bit  2   (0×3 + 2)
     ├─────┼─────┼─────┤
fila1│  G  │  G  │  G  │   →  bits 3, 4, 5
     ├─────┼─────┼─────┤
fila2│  G  │  G  │  S  │   →  bits 6, 7
     └─────┴─────┴─────┘

cellGold inicial = (1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7)
                 = 0b 1111 1100  =  0xFC

```

Después de un CASO 3 que deposite oro en `(fila0, col0)` — celda que nunca fue `G`:

```
cellGold |= (1ULL << (0×3 + 0))   // bit 0 se enciende
```

Con `bool[6]` ese estado habría sido irrepresentable. Con el bitmask, es solo un bit.

### caracteriztica del sistema de conservación del oro para que permanezca sin cambios o que siempre se cumpla

En todo instante del sistema se cumple:

```
cube.countGoldFaces() + cellGoldCount() == 6
```

El oro no se crea ni se destruye — solo circula entre celdas de la grilla y caras del cubo a través de los intercambios de `Transition`. Esta invariante es verificable en cualquier `State` y es la garantía formal de correctitud del modelo.
