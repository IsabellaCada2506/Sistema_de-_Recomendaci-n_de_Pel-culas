#pragma once

#include <string>
#include <vector>

namespace recomendador {

// Cantidad fija de géneros y orden fijo, compartido entre usuarios y películas.
constexpr int NUM_GENEROS = 5;

inline const std::vector<std::string> GENEROS = {
    "Acción", "Comedia", "Drama", "Terror", "Romance"
};

// Fila i de la matriz U: preferencias del usuario por cada género (1-10).
struct Usuario {
    int id_usuario;
    std::string nombre;
    std::string email;
    std::vector<int> preferencias; // tamaño NUM_GENEROS, escala 1-10
};

// Columna k de la matriz G: pertenencia de la película a cada género (0-10).
struct Pelicula {
    int id_pelicula;
    std::string titulo;
    int anio;
    std::vector<int> generos; // tamaño NUM_GENEROS, escala 0-10
};

} // namespace recomendador