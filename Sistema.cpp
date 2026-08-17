#include "Sistema.h"

#include <algorithm>

namespace recomendador {

void Sistema::cargarDatosIniciales() {
    usuarios_ = {
        {1, "Ana Torres", "ana.torres@mail.com", {8, 3, 5, 2, 6}},
        {2, "Luis Gómez", "luis.gomez@mail.com", {4, 9, 3, 1, 5}},
        {3, "Marta Ruiz", "marta.ruiz@mail.com", {2, 6, 8, 3, 7}},
        {4, "Carlos Peña", "carlos.pena@mail.com", {9, 2, 4, 8, 1}},
        {5, "Sofía León", "sofia.leon@mail.com", {3, 7, 6, 2, 9}},
    };

    peliculas_ = {
        {101, "Furia Extrema", 2022, {9, 1, 2, 4, 0}},
        {102, "Risas de Verano", 2021, {1, 9, 2, 0, 4}},
        {103, "Lágrimas del Alma", 2020, {0, 2, 9, 1, 6}},
        {104, "Noche sin Fin", 2023, {3, 0, 3, 9, 1}},
        {105, "Bajo la Misma Luna", 2019, {1, 3, 6, 0, 9}},
    };
}

bool Sistema::existeUsuario(int id_usuario) const {
    return std::any_of(usuarios_.begin(), usuarios_.end(), [&](const Usuario& u) {
        return u.id_usuario == id_usuario;
    });
}

Usuario* Sistema::buscarUsuario(int id_usuario) {
    for (auto& usuario : usuarios_) {
        if (usuario.id_usuario == id_usuario) {
            return &usuario;
        }
    }
    return nullptr;
}

bool Sistema::emailExiste(const std::string& email, int idExcluir) const {
    return std::any_of(usuarios_.begin(), usuarios_.end(), [&](const Usuario& u) {
        return u.id_usuario != idExcluir && u.email == email;
    });
}

void Sistema::insertarUsuario(const Usuario& usuario) {
    usuarios_.push_back(usuario);
}

void Sistema::eliminarUsuario(int id_usuario) {
    usuarios_.erase(
        std::remove_if(usuarios_.begin(), usuarios_.end(),
                        [&](const Usuario& u) { return u.id_usuario == id_usuario; }),
        usuarios_.end());
}

bool Sistema::existePelicula(int id_pelicula) const {
    return std::any_of(peliculas_.begin(), peliculas_.end(), [&](const Pelicula& p) {
        return p.id_pelicula == id_pelicula;
    });
}

Pelicula* Sistema::buscarPelicula(int id_pelicula) {
    for (auto& pelicula : peliculas_) {
        if (pelicula.id_pelicula == id_pelicula) {
            return &pelicula;
        }
    }
    return nullptr;
}

void Sistema::insertarPelicula(const Pelicula& pelicula) {
    peliculas_.push_back(pelicula);
}

void Sistema::eliminarPelicula(int id_pelicula) {
    peliculas_.erase(
        std::remove_if(peliculas_.begin(), peliculas_.end(),
                        [&](const Pelicula& p) { return p.id_pelicula == id_pelicula; }),
        peliculas_.end());
}

long long Sistema::calcularAfinidad(const std::vector<int>& preferenciasUsuario,
                                     const std::vector<int>& generosPelicula) const {
    long long suma = 0;
    // Se utiliza el número fijo de géneros (5) según el diseño.
    for (int j = 0; j < 5; ++j) {
        suma += static_cast<long long>(preferenciasUsuario[j]) * generosPelicula[j];
    }
    return suma;
}

// -----------------------------------------------------------------------------
// IMPLEMENTACIÓN: DIVIDE Y VENCERÁS
// -----------------------------------------------------------------------------

void Sistema::calcularFilaRecursiva(const std::vector<int>& preferencias,
                                    int inicio,
                                    int fin,
                                    std::vector<std::pair<Pelicula*, long long>>& fila) {
    // 1. CASO BASE (Vencer): Si el rango se reduce a una sola película
    if (inicio == fin) {
        long long afinidad = calcularAfinidad(preferencias, peliculas_[inicio].generos);
        fila.emplace_back(&peliculas_[inicio], afinidad);
        return;
    }

    // 2. DIVIDIR: Partir el conjunto de películas por la mitad
    int medio = inicio + (fin - inicio) / 2;

    // 3. RECURSIVIDAD: Llamar al algoritmo para la mitad izquierda y derecha
    calcularFilaRecursiva(preferencias, inicio, medio, fila);
    calcularFilaRecursiva(preferencias, medio + 1, fin, fila);
}

std::vector<std::pair<Pelicula*, long long>> Sistema::calcularFilaUsuario(Usuario& usuario) {
    std::vector<std::pair<Pelicula*, long long>> fila;
    
    // Verificación de seguridad: si la lista está vacía, retornar fila vacía
    if (peliculas_.empty()) {
        return fila;
    }

    // Reservar memoria para mayor eficiencia
    fila.reserve(peliculas_.size());

    // Disparar el algoritmo Divide y Vencerás
    calcularFilaRecursiva(usuario.preferencias, 0, peliculas_.size() - 1, fila);

    return fila;
}

} // namespace recomendador