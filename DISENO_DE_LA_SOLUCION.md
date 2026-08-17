# Diseño de la Solución

**Isabella Cadavid Posada**
**Sistema de Recomendación de Películas mediante Multiplicación de Matrices**

---

## 1. Problema escogido

Una plataforma de streaming desea mejorar su sistema de recomendación de películas para los usuarios. Para ello, cuenta con una matriz **U** de 100x5 que representa las preferencias de 100 usuarios sobre 5 géneros de películas, donde cada elemento `U[i][j]` indica la afinidad del usuario `i` por el género `j`, valorada en una escala del 1 al 10.

Por otro lado, la plataforma dispone de una matriz **G** de 5x50 que contiene el índice de coincidencia entre 5 géneros de películas y 50 películas disponibles en la plataforma. Cada elemento `G[j][k]` indica el grado en que la película `k` pertenece al género `j`.

El objetivo es calcular una matriz **R** de 100x50 que represente las recomendaciones personalizadas para cada usuario. Cada elemento `R[i][k]` indicará la afinidad del usuario `i` con la película `k`, obtenida multiplicando las matrices U y G.

**Objetivo:** Calcular la matriz R multiplicando las matrices U y G, para obtener las afinidades de los usuarios con respecto a todas las películas disponibles en la plataforma. Este cálculo ayudará a determinar cuáles son las películas más adecuadas para recomendar a cada usuario basado en sus preferencias de género.

---

## 2. Supuestos

Dado que el enunciado no especifica ciertos detalles operativos, se asumen los siguientes puntos para poder construir un sistema funcional:

- **Tipo de interfaz:** consola (menú interactivo numerado, sin interfaz gráfica).
- **Géneros de película (5, orden fijo):** j0 = Acción, j1 = Comedia, j2 = Drama, j3 = Terror, j4 = Romance. El orden de este vector es el mismo tanto en la matriz U como en la matriz G, y no puede alterarse.
- **Atributos de un usuario:**
  - `id_usuario` (numérico, único, autogenerado o ingresado por el operador)
  - `nombre` (texto)
  - `email` (texto, único)
  - `preferencias` (vector de 5 enteros, escala 1–10, uno por género — esto es la fila i de la matriz U)
- **Atributos de una película:**
  - `id_pelicula` (numérico, único)
  - `titulo` (texto)
  - `anio` (numérico)
  - `generos` (vector de 5 enteros, escala 0–10, grado de pertenencia a cada género — esto es la columna k de la matriz G)
- **Escala:** todos los valores de afinidad y pertenencia son números enteros entre 1 y 10 (0 se permite en `generos` cuando la película no pertenece nada a ese género).
- **Tamaño real vs. tamaño de ejemplo:** el sistema en producción maneja 100 usuarios x 5 géneros x 50 películas, pero para efectos de este documento se trabaja con una muestra reducida de 5 usuarios y 5 películas, suficiente para ilustrar el comportamiento del sistema sin perder generalidad.
- La matriz R **no se guarda de forma persistente** como archivo aparte: se recalcula bajo demanda (o se actualiza incrementalmente) cada vez que cambian los datos de usuarios o de películas, para garantizar que las recomendaciones estén siempre actualizadas.
- **Gestión de memoria y volatilidad:** El sistema opera exclusivamente en la memoria principal (RAM) mediante estructuras de datos dinámicas (`std::vector`). No se implementa persistencia de datos en disco (archivos o bases de datos). Por tanto, cualquier modificación en el CRUD (inserciones, ediciones, eliminaciones) es temporal y se reiniciará a los datos de ejemplo predefinidos cada vez que finalice la ejecución del programa. Esto permite enfocar la complejidad en la evaluación lógica y la eficiencia del algoritmo recursivo de multiplicación.

### Registros iniciales de ejemplo (usuarios) — filas de U

| id_usuario | nombre      | email                  | Acción | Comedia | Drama | Terror | Romance |
|-----------:|-------------|-------------------------|:------:|:-------:|:-----:|:------:|:-------:|
| 1          | Ana Torres  | ana.torres@mail.com     | 8      | 3       | 5     | 2      | 6       |
| 2          | Luis Gómez  | luis.gomez@mail.com     | 4      | 9       | 3     | 1      | 5       |
| 3          | Marta Ruiz  | marta.ruiz@mail.com     | 2      | 6       | 8     | 3      | 7       |
| 4          | Carlos Peña | carlos.pena@mail.com    | 9      | 2       | 4     | 8      | 1       |
| 5          | Sofía León  | sofia.leon@mail.com     | 3      | 7       | 6     | 2      | 9       |

### Registros iniciales de ejemplo (películas) — columnas de G

| id_pelicula | titulo             | anio | Acción | Comedia | Drama | Terror | Romance |
|------------:|---------------------|-----:|:------:|:-------:|:-----:|:------:|:-------:|
| 101         | Furia Extrema        | 2022 | 9      | 1       | 2     | 4      | 0       |
| 102         | Risas de Verano       | 2021 | 1      | 9       | 2     | 0      | 4       |
| 103         | Lágrimas del Alma      | 2020 | 0      | 2       | 9     | 1      | 6       |
| 104         | Noche sin Fin           | 2023 | 3      | 0       | 3     | 9      | 1       |
| 105         | Bajo la Misma Luna       | 2019 | 1      | 3       | 6     | 0      | 9       |

---

## 3. Estructura de datos

La información se organiza en tres colecciones: `usuarios`, `peliculas` y, de forma derivada (no editable directamente), la matriz de resultados R.

```json
{
  "usuarios": [
    {
      "id_usuario": 1,
      "nombre": "Ana Torres",
      "email": "ana.torres@mail.com",
      "preferencias": [8, 3, 5, 2, 6]
    },
    {
      "id_usuario": 2,
      "nombre": "Luis Gómez",
      "email": "luis.gomez@mail.com",
      "preferencias": [4, 9, 3, 1, 5]
    }
  ],
  "peliculas": [
    {
      "id_pelicula": 101,
      "titulo": "Furia Extrema",
      "anio": 2022,
      "generos": [9, 1, 2, 4, 0]
    },
    {
      "id_pelicula": 102,
      "titulo": "Risas de Verano",
      "anio": 2021,
      "generos": [1, 9, 2, 0, 4]
    }
  ]
}
```

La matriz R (calculada, no almacenada como entrada del usuario) se representa así en memoria:

```json
{
  "matriz_R": [
    { "id_usuario": 1, "afinidades": { "101": 80, "102": 43, "103": 60 } },
    { "id_usuario": 2, "afinidades": { "101": 65, "102": 96, "103": 51 } }
  ]
}
```

Cada clave dentro de `afinidades` corresponde a un `id_pelicula`, y el valor es el resultado de `R[i][k]`.

---

## 4. Diseño de la solución

### 4.0 Algoritmo de cálculo de la matriz R (lenguaje natural)

**Solución propuesta:** La matriz R se obtiene multiplicando U (100x5) por G (5x50). Cada elemento `R[i][k]` no es más que el "producto punto" (suma de productos elemento a elemento) entre la fila i de U (las preferencias del usuario i sobre los 5 géneros) y la columna k de G (la pertenencia de la película k a esos mismos 5 géneros). En otras palabras: se recorre cada género j, se multiplica la afinidad del usuario por ese género por el grado de pertenencia de la película a ese género, y se suman esos 5 productos.

**Cómo se divide el problema:** El cálculo completo de R (100x50 = 5.000 valores) se descompone en subproblemas independientes de dos niveles:

- **Subproblema mayor (por usuario):** calcular la fila completa de recomendaciones de un usuario i contra las 50 películas. Este subproblema es independiente para cada uno de los 100 usuarios, por lo que puede resolverse uno a la vez o en paralelo.
- **Subproblema menor (por par usuario-película):** dentro de cada fila, calcular un único valor `R[i][k]`, que consiste en un producto punto de 5 términos entre el vector de preferencias del usuario i y la columna de género de la película k.

**Qué debe devolver cada subproblema:**

- El subproblema menor (par usuario-película) devuelve un único número escalar: la afinidad del usuario i con la película k.
- El subproblema mayor (por usuario) devuelve un vector de 50 escalares: la fila completa de afinidades del usuario i con todas las películas.

**Cómo se combinan los resultados:** Los 50 escalares calculados para un mismo usuario se colocan en orden (según el id de la película) para formar su fila completa dentro de R. Luego, las 100 filas (una por usuario) se apilan en el mismo orden en que aparecen los usuarios, formando así la matriz R de 100x50 completa. Para generar recomendaciones a un usuario específico, basta con tomar su fila y ordenar las películas de mayor a menor afinidad.

**Ejemplo reducido:** Supongamos solo 2 usuarios, 3 géneros y 2 películas.

U (2x3):

|       | Acción | Comedia | Drama |
|-------|:------:|:-------:|:-----:|
| Ana   | 8      | 3       | 5     |
| Luis  | 4      | 9       | 3     |

G (3x2):

|          | Furia Extrema | Risas de Verano |
|----------|:-------------:|:----------------:|
| Acción   | 9              | 1                |
| Comedia  | 1              | 9                |
| Drama    | 2              | 2                |

Cálculo de:

```
R[Ana][Furia Extrema]     = (8×9) + (3×1) + (5×2) = 72 + 3 + 10 = 85
R[Ana][Risas de Verano]   = (8×1) + (3×9) + (5×2) = 8 + 27 + 10 = 45
R[Luis][Furia Extrema]    = (4×9) + (9×1) + (3×2) = 36 + 9 + 6  = 51
R[Luis][Risas de Verano]  = (4×1) + (9×9) + (3×2) = 4 + 81 + 6  = 91
```

R resultante (2x2), y porque 2x3 : 3x2, tomamos los valores de las esquinas y nos da dicha matriz de 2x2:

|       | Furia Extrema | Risas de Verano |
|-------|:-------------:|:----------------:|
| Ana   | 85             | 45               |
| Luis  | 51             | 91               |

Con este resultado, a Ana se le recomendaría primero "Furia Extrema" (85 > 45) y a Luis primero "Risas de Verano" (91 > 51).

### 4.0 Algoritmo de cálculo de la matriz R (Enfoque Divide y Vencerás)

**Solución propuesta:** La matriz R se obtiene multiplicando U (100x5) por G (5x50). Para optimizar y estructurar el cálculo de las recomendaciones bajo demanda, se aplica el paradigma algorítmico de Divide y Vencerás (Divide and Conquer) sobre el catálogo de películas. En lugar de iterar secuencialmente, el algoritmo opera de la siguiente manera:

- **Dividir (Partición del catálogo):** Para calcular la fila completa de recomendaciones de un usuario i, se toma el arreglo total de películas disponibles y se divide recursivamente por la mitad (calculando el índice central). Esto fragmenta el espacio de búsqueda en subconjuntos cada vez más pequeños.
- **Vencer (Caso Base):** La recursividad se detiene cuando el rango se reduce a una sola película. En este punto, se calcula un único escalar `R[i][k]` mediante el "producto punto" (suma de productos elemento a elemento) entre el vector de preferencias del usuario i y el vector de géneros de la película k.
- **Combinar:** A medida que las llamadas recursivas finalizan, las afinidades individuales calculadas se integran en el vector resultante que conforma la fila del usuario. Para maximizar la eficiencia y evitar el consumo excesivo de memoria, la combinación se realiza directamente sobre la estructura de datos original mediante el paso por referencia.

**Qué debe devolver el algoritmo:** El proceso devuelve un vector compuesto por escalares, representando la fila completa de afinidades del usuario i con todas las películas. Para generar las sugerencias finales, basta con tomar esta fila resultante y ordenar las películas de mayor a menor afinidad.

**Ejemplo reducido:**

Matriz de preferencias de Ana:

|      | Acción | Comedia | Drama |
|------|:------:|:-------:|:-----:|
| Ana  | 8      | 3       | 5     |

Matriz G (3×2):

| Género   | Furia Extrema | Risas de Verano |
|----------|:-------------:|:----------------:|
| Acción   | 9              | 1                |
| Comedia  | 1              | 9                |
| Drama    | 2              | 2                |

### 4.1 Inserción de información

El menú principal ofrece insertar un usuario o una película. Se describe el caso de "insertar usuario"; el de "insertar película" sigue la misma lógica sobre la colección `peliculas`.

#### a) Instrucciones al programador

1. Mostrar submenú: "¿Qué desea insertar? 1) Usuario 2) Película".
2. Si se elige "Usuario":
   1. Solicitar `id_usuario`.
   2. Verificar si ese `id_usuario` ya existe en la colección `usuarios`.
      - Si ya existe: mostrar el mensaje de error **"El id_usuario ya existe. No se puede insertar un usuario duplicado."** y regresar al menú principal sin insertar nada.
   3. Solicitar `nombre` y `email`. Verificar que `email` no esté vacío y no esté ya registrado en otro usuario; si el email ya existe, mostrar error **"El email ya está registrado."** y cancelar la inserción.
   4. Solicitar, uno por uno, los 5 valores de `preferencias` (Acción, Comedia, Drama, Terror, Romance).
      - Validar que cada valor sea un número entero entre 1 y 10. Si el valor está fuera de rango o no es numérico, mostrar **"Valor inválido, debe ser un entero entre 1 y 10."** y volver a solicitar ese mismo dato (sin descartar los ya ingresados).
   5. Construir el nuevo registro y agregarlo a la colección `usuarios`.
   6. Marcar la matriz R como desactualizada (o recalcular la fila del nuevo usuario) para que sus recomendaciones estén disponibles de inmediato.
   7. Mostrar mensaje de confirmación: **"Usuario insertado correctamente."** y volver al menú principal.
3. Si se elige "Película": aplicar la misma secuencia de pasos, pero sobre `id_pelicula`, `titulo`, `anio` y el vector `generos` (escala 0–10), y agregar el registro a la colección `peliculas`.

#### b) Ejemplo concreto

El operador elige "Insertar usuario" e ingresa:

```
id_usuario: 6
nombre: Pedro Nieto
email: pedro.nieto@mail.com
Acción: 7
Comedia: 4
Drama: 2
Terror: 9
Romance: 3
```

Como `id_usuario = 6` y `email = pedro.nieto@mail.com` no existían previamente, el sistema los acepta y muestra **"Usuario insertado correctamente."**. La colección `usuarios` queda así:

```json
{
  "id_usuario": 6,
  "nombre": "Pedro Nieto",
  "email": "pedro.nieto@mail.com",
  "preferencias": [7, 4, 2, 9, 3]
}
```

agregado al final del arreglo `usuarios`. Muy importante: ningún campo debe quedar vacío; en caso de que falte algún campo por llenar, mostrar un mensaje de **"Por favor ingrese un valor"**.

### 4.2 Consulta de información

#### a) Instrucciones al programador

1. Mostrar submenú: "¿Qué desea consultar? 1) Usuario por id 2) Película por id 3) Recomendaciones de un usuario".
2. Si se elige "Usuario por id" o "Película por id":
   1. Solicitar el id correspondiente.
   2. Buscar linealmente en la colección respectiva.
      - Si no se encuentra ningún registro con ese id: mostrar **"No se encontró ningún registro con el id ingresado."** y volver al menú.
      - Si se encuentra, mostrar todos sus campos en pantalla.
3. Si se elige "Recomendaciones de un usuario":
   1. Solicitar `id_usuario`.
   2. Validar que exista; si no, mostrar **"Usuario no encontrado."** y volver al menú.
   3. Calcular (o recuperar si ya está calculada) la fila i de la matriz R para ese usuario, usando el algoritmo descrito en el punto 4.0.
   4. Ordenar las películas de mayor a menor afinidad y mostrar el listado (por ejemplo, las 5 mejores recomendaciones).

#### b) Ejemplo concreto

El operador consulta recomendaciones para `id_usuario = 1` (Ana Torres, preferencias `[8, 3, 5, 2, 6]`). El sistema calcula `R[1][k]` para cada película registrada usando el vector `generos` de cada una y devuelve, ordenado de mayor a menor:

```
1) Furia Extrema      -> afinidad 72
2) Bajo la Misma Luna  -> afinidad 65
3) Lágrimas del Alma   -> afinidad 44
4) Risas de Verano     -> afinidad 40
5) Noche sin Fin       -> afinidad 33
```

No se modifica ninguna estructura de datos; la consulta es de solo lectura.

### 4.3 Edición de información (con menú de campos)

#### a) Instrucciones al programador

1. Mostrar submenú: "¿Qué desea editar? 1) Usuario 2) Película".
2. Solicitar el id del registro a editar.
3. Buscar el registro en la colección correspondiente.
   - Si no se encuentra: mostrar **"No se encontró ningún registro con ese id. No es posible editar."** y volver al menú principal.
4. Si se encuentra, mostrar un submenú de campos editables. Para un usuario:
   ```
   ¿Qué campo desea editar?
   1) Nombre
   2) Email
   3) Preferencia de género (Acción/Comedia/Drama/Terror/Romance)
   4) Cancelar
   ```
   (Para una película, el submenú ofrece: Título, Año, Grado de pertenencia a un género, Cancelar.)
5. Según la opción elegida:
   - Si es "Nombre" o "Título": solicitar el nuevo valor y verificar que no esté vacío.
   - Si es "Email": solicitar el nuevo valor, verificar que no esté vacío y que no pertenezca a otro usuario distinto (evitar duplicados). Si ya existe en otro registro, mostrar error y cancelar esa edición puntual.
   - Si es "Preferencia de género" o "Grado de pertenencia": primero preguntar a qué género corresponde (1–5), luego solicitar el nuevo valor y validar que sea un entero entre 1 y 10 (o 0–10 para películas). Si es inválido, mostrar **"Valor inválido."** y volver a solicitarlo.
   - Si es "Cancelar": regresar al menú principal sin modificar nada.
6. Aplicar el cambio sobre el registro en memoria.
7. Marcar la matriz R como desactualizada (o recalcular la fila/columna afectada), ya que un cambio en `preferencias` o en `generos` altera las afinidades calculadas.
8. Mostrar **"Registro actualizado correctamente."** y volver al menú principal.

#### b) Ejemplo concreto

El operador edita al usuario `id_usuario = 3` (Marta Ruiz) y elige el submenú "3) Preferencia de género". El sistema pregunta:

```
¿Qué género desea modificar?
1) Acción   2) Comedia   3) Drama   4) Terror   5) Romance
> 4
Nuevo valor para Terror (1-10): 7
```

Como el valor 7 está dentro del rango permitido, el sistema lo acepta. El registro de Marta Ruiz cambia de:

```json
{ "id_usuario": 3, "nombre": "Marta Ruiz", "email": "marta.ruiz@mail.com", "preferencias": [2, 6, 8, 3, 7] }
```

a:

```json
{ "id_usuario": 3, "nombre": "Marta Ruiz", "email": "marta.ruiz@mail.com", "preferencias": [2, 6, 8, 7, 7] }
```

y el sistema muestra **"Registro actualizado correctamente."**

### 4.4 Eliminación de información

#### a) Instrucciones al programador

1. Mostrar submenú: "¿Qué desea eliminar? 1) Usuario 2) Película".
2. Solicitar el id del registro a eliminar.
3. Buscar el registro en la colección correspondiente.
   - Si no se encuentra: mostrar **"No se encontró ningún registro con ese id. No es posible eliminar."** y volver al menú principal.
4. Si se encuentra, mostrar sus datos completos en pantalla y solicitar confirmación:
   ```
   ¿Está seguro que desea eliminar este registro? (S/N)
   ```
   Aquí se van a aceptar tanto mayúsculas como minúsculas de esos dos caracteres.
5. Si la respuesta es "N": mostrar **"Eliminación cancelada."** y volver al menú principal sin borrar nada.
6. Si la respuesta es "S": eliminar el registro de la colección correspondiente.
   - Si se elimina un usuario, descartar su fila en la matriz R; si se elimina una película, descartar la columna correspondiente en todas las filas de R.
   - Mostrar **"Registro eliminado correctamente."** y volver al menú principal.
7. Si la respuesta no es "S" ni "N": mostrar **"Respuesta inválida, ingrese S o N."** y volver a solicitar la confirmación (sin repetir la búsqueda del registro).

#### b) Ejemplo concreto

El operador elige eliminar la película `id_pelicula = 104` (Noche sin Fin). El sistema muestra:

```
Título: Noche sin Fin
Año: 2023
Géneros [Acción, Comedia, Drama, Terror, Romance]: [3, 0, 3, 9, 1]

¿Está seguro que desea eliminar este registro? (S/N)
> S
```

El sistema elimina el registro de la colección `peliculas` y descarta la columna "104" de todas las filas de la matriz R. Muestra **"Registro eliminado correctamente."** y regresa al menú principal.

---

## 5. Menú principal

```
==== SISTEMA DE RECOMENDACIÓN DE PELÍCULAS ====
1) Insertar información
2) Consultar información
3) Editar información
4) Eliminar información
5) Salir
Seleccione una opción:
```

Al elegir la opción 5) Salir, el sistema muestra el mensaje **"Gracias por usar el sistema. ¡Hasta pronto!"** y finaliza la ejecución del programa.
