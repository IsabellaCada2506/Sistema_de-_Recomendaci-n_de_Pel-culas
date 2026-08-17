#pragma once

#include <string>
#include <vector>

namespace recomendador {

// Pide texto por consola; repite si el usuario ingresa una cadena vacía.
std::string pedirTextoNoVacio(const std::string& mensaje);

// Pide un número entero cualquiera; repite si la entrada no es numérica.
int pedirEntero(const std::string& mensaje);

// Pide un número entero dentro de [minimo, maximo]; repite si es inválido.
int pedirEnteroEnRango(const std::string& mensaje, int minimo, int maximo);

// Pide, uno por uno, los NUM_GENEROS valores (preferencias o pertenencia a género).
std::vector<int> pedirVectorGeneros(int minimo, int maximo);

// Pide confirmación S/N (acepta mayúsculas o minúsculas); repite si no es válida.
bool pedirConfirmacionSN(const std::string& mensaje);

} // namespace recomendador