# Sistema de Recomendación de Películas mediante Multiplicación de Matrices

Sistema de consola en C++ que calcula recomendaciones personalizadas de películas
para usuarios, multiplicando una matriz de preferencias de usuario (**U**) por una
matriz de pertenencia de género (**G**), para obtener la matriz de afinidades **R**.

> Diseño original: Isabella Cadavid Posada

---

## Tabla de contenido

- [Descripción del problema](#descripción-del-problema)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Requisitos](#requisitos)
- [Compilación](#compilación)
- [Ejecución](#ejecución)
- [Menú principal](#menú-principal)
- [Ejemplo de uso](#ejemplo-de-uso)
- [Algoritmo de cálculo (R = U × G)](#algoritmo-de-cálculo-r--u--g)
- [Supuestos del diseño](#supuestos-del-diseño)
- [Cómo probar que todo funciona](#cómo-probar-que-todo-funciona)
- [Limitaciones conocidas](#limitaciones-conocidas)

---

## Descripción del problema

Una plataforma de streaming quiere recomendar películas a sus usuarios según su
afinidad por géneros. Para eso usa dos matrices:

- **U** (usuarios × géneros): cada fila es el vector de preferencias de un
  usuario, con valores de 1 a 10 por género.
- **G** (géneros × películas): cada columna es el vector de pertenencia de una
  película a cada género, con valores de 0 a 10.

El sistema calcula **R = U × G**, donde `R[i][k]` es la afinidad del usuario `i`
con la película `k`. En este proyecto se trabaja con una muestra reducida de 5
usuarios y 5 películas (el diseño original contempla 100 usuarios × 5 géneros ×
50 películas, pero la lógica es idéntica sea cual sea el tamaño).

---

## Estructura del proyecto

```
recomendador_cpp/
├── Modelos.h      # Structs Usuario y Pelicula, constantes de género
├── Entrada.h       # Declaración de funciones de entrada validada por consola
├── Entrada.cpp      # Implementación de la validación de entrada
├── Sistema.h        # Declaración de la lógica de negocio (CRUD + cálculo de R)
├── Sistema.cpp       # Implementación del CRUD y el cálculo de afinidades
├── Menu.h            # Declaración de la interfaz de consola (menús)
├── Menu.cpp           # Implementación de los menús y submenús
├── main.cpp            # Punto de entrada del programa
├── Makefile             # Atajo opcional para compilar con `make`
└── README.md             # Este archivo
```

Cada archivo tiene una única responsabilidad:

| Archivo         | Responsabilidad                                                        |
|-----------------|--------------------------------------------------------------------------|
| `Modelos.h`     | Solo define los datos (`Usuario`, `Pelicula`), sin lógica.               |
| `Entrada.*`     | Lee y valida datos por consola (texto, enteros, rangos, S/N).            |
| `Sistema.*`     | Guarda usuarios y películas en memoria, hace CRUD y calcula la matriz R. |
| `Menu.*`        | Muestra los menús y llama a `Sistema` según lo que el usuario elige.     |
| `main.cpp`      | Arma todo: crea el `Sistema`, carga datos de ejemplo y lanza el `Menu`.  |

---

## Requisitos

- Un compilador de C++ compatible con **C++17** (en macOS: Clang, instalado con
  las Command Line Tools de Xcode; en Linux: `g++`).
- No requiere librerías externas ni conexión a internet: solo usa la biblioteca
  estándar de C++ (`<vector>`, `<string>`, `<iostream>`, `<algorithm>`).

---

## Compilación

### Opción 1 — Un solo comando (sin Makefile)

No necesitas el `Makefile` para nada, es solo una comodidad. Desde la carpeta
del proyecto:

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o recomendador main.cpp Entrada.cpp Sistema.cpp Menu.cpp
```

Esto genera un ejecutable llamado `recomendador` en la misma carpeta.

### Opción 2 — Con Makefile (más cómodo)

```bash
make          # compila
make clean    # borra los .o y el ejecutable
```

### En macOS, si no tienes compilador todavía

```bash
xcode-select --install
```

Acepta la instalación de las Command Line Tools y espera a que termine.
Verifica con `g++ --version`.

---

## Ejecución

```bash
./recomendador
```

Se muestra el menú principal y puedes navegar escribiendo el número de la
opción deseada y presionando Enter.

---

## Menú principal

```
==== SISTEMA DE RECOMENDACIÓN DE PELÍCULAS ====
1) Insertar información
2) Consultar información
3) Editar información
4) Eliminar información
5) Salir
Seleccione una opción:
```

| Opción                     | Qué hace                                                                 |
|-----------------------------|---------------------------------------------------------------------------|
| 1) Insertar información     | Agrega un usuario o una película nueva (con validación de datos).       |
| 2) Consultar información    | Busca un usuario/película por id, o pide las recomendaciones de un usuario. |
| 3) Editar información       | Modifica un campo de un usuario o película existente.                    |
| 4) Eliminar información     | Borra un usuario o película, previa confirmación (S/N).                  |
| 5) Salir                    | Termina el programa.                                                     |

---

## Ejemplo de uso

Consulta de recomendaciones para el usuario 1 (Ana Torres), usando los datos
de ejemplo con los que arranca el programa:

```
==== SISTEMA DE RECOMENDACIÓN DE PELÍCULAS ====
1) Insertar información
2) Consultar información
3) Editar información
4) Eliminar información
5) Salir
Seleccione una opción: 2
¿Qué desea consultar?
1) Usuario por id
2) Película por id
3) Recomendaciones de un usuario
Seleccione una opción: 3
id_usuario: 1

Recomendaciones para Ana Torres:
1) Furia Extrema             -> afinidad 72
2) Bajo la Misma Luna        -> afinidad 65
3) Lágrimas del Alma         -> afinidad 44
4) Risas de Verano            -> afinidad 40
5) Noche sin Fin              -> afinidad 33
```

> **Nota:** estos números salen directamente de los datos de ejemplo cargados
> al iniciar el programa. Si antes insertaste, editaste o eliminaste registros
> **en esa misma ejecución**, las afinidades reflejarán esos cambios — pero
> apenas cierres y vuelvas a abrir el programa, todo vuelve a los datos
> originales (ver [Limitaciones conocidas](#limitaciones-conocidas)).

---

## Algoritmo de cálculo (R = U × G)

`R[i][k]` es el producto punto entre el vector de preferencias del usuario `i`
y el vector de géneros de la película `k`:

```
R[i][k] = Σ (U[i][j] × G[j][k])   para j = 0..4 (los 5 géneros)
```

El cálculo se descompone en dos niveles, tal como lo describe el diseño:

- **Subproblema menor** (`Sistema::calcularAfinidad`): calcula un solo
  `R[i][k]`, sumando los 5 productos preferencia × género.
- **Subproblema mayor** (`Sistema::calcularFilaUsuario`): recorre todas las
  películas y arma la fila completa de afinidades de un usuario.

La matriz **R** completa nunca se guarda como estructura aparte: cada fila se
recalcula al vuelo cuando se piden recomendaciones, así siempre refleja el
estado más reciente de usuarios y películas.

> El diseño también describe una variante que resuelve la fila de un usuario
> partiendo recursivamente el catálogo de películas por la mitad (Divide y
> Vencerás) hasta llegar a una sola película por rama. Matemáticamente da el
> mismo resultado que el recorrido directo que usa el código actual — si
> quieres que la implementación siga literalmente ese esquema recursivo en
> vez de la iteración simple, lo puedo ajustar.

---

## Supuestos del diseño

- Interfaz de consola, sin interfaz gráfica.
- 5 géneros en orden fijo: Acción, Comedia, Drama, Terror, Romance (mismo
  orden en usuarios y películas).
- Preferencias de usuario: enteros de 1 a 10.
- Pertenencia de género en películas: enteros de 0 a 10 (0 permitido).
- `id_usuario`, `email` e `id_pelicula` son únicos.
- La matriz R no se persiste como archivo; se recalcula bajo demanda.
- **El sistema trabaja solo en memoria (RAM)**, usando `std::vector`. No hay
  persistencia en disco ni base de datos: cualquier inserción, edición o
  eliminación es temporal y se pierde al cerrar el programa, volviendo a los
  5 usuarios y 5 películas de ejemplo en la próxima ejecución.

---

## Cómo probar que todo funciona

1. **Cálculo de afinidad**: consulta las recomendaciones del usuario 1 y
   verifica que coincidan con el ejemplo de arriba (72, 65, 44, 40, 33).
2. **Inserción duplicada**: intenta insertar un usuario con `id_usuario = 1`
   (ya existe) → debe rechazarlo.
3. **Validación de rango**: al insertar, ingresa `15` o `abc` como preferencia
   → debe pedirlo de nuevo sin perder los datos ya ingresados.
4. **Edición**: cambia el Terror de Marta Ruiz (id 3) a `7` y confirma que el
   cambio se refleje al consultarla.
5. **Eliminación con confirmación**: al eliminar, prueba una respuesta
   inválida (debe repetir la pregunta), luego `n` (debe cancelar), luego `s`
   (debe borrar).
6. **Email duplicado**: intenta usar un email que ya tenga otro usuario → debe
   rechazarlo.

Si recompilaste el código y los resultados no cambian, asegúrate de haber
vuelto a correr `make` (o el comando de `g++`) antes de probar de nuevo.

---

## Limitaciones conocidas

- **No hay persistencia**: todos los cambios (insertar, editar, eliminar) se
  pierden al salir del programa. Esto es intencional según el diseño, para
  enfocar la evaluación en la lógica del algoritmo y no en el manejo de
  archivos o bases de datos.
- El tamaño de ejemplo es de 5 usuarios y 5 películas; la lógica es la misma
  para 100 usuarios × 50 películas, solo cambia el volumen de datos.
