#pragma once

#include "Modelos.h"

#include <utility>
#include <vector>

namespace recomendador {

// Contiene las colecciones de usuarios y películas, las operaciones CRUD
// sobre ellas, y el cálculo de la matriz R (afinidad usuario-película)
// descrito en la sección 4.0 del diseño: R = U x G.
class Sistema {
public:
    void cargarDatosIniciales();

    // --- Usuarios ---
    bool existeUsuario(int id_usuario) const;
    Usuario* buscarUsuario(int id_usuario);
    bool emailExiste(const std::string& email, int idExcluir = -1) const;
    void insertarUsuario(const Usuario& usuario);
    void eliminarUsuario(int id_usuario);

    // --- Películas ---
    bool existePelicula(int id_pelicula) const;
    Pelicula* buscarPelicula(int id_pelicula);
    void insertarPelicula(const Pelicula& pelicula);
    void eliminarPelicula(int id_pelicula);

    // --- Cálculo de la matriz R ---
    // Subproblema menor: producto punto entre preferencias y géneros (5 términos).
    long long calcularAfinidad(const std::vector<int>& preferenciasUsuario,
                                const std::vector<int>& generosPelicula) const;

    // Subproblema mayor: fila completa de un usuario contra todas las películas.
    std::vector<std::pair<Pelicula*, long long>> calcularFilaUsuario(Usuario& usuario);

private:
    std::vector<Usuario> usuarios_;
    std::vector<Pelicula> peliculas_;

    // Algoritmo de Divide y Vencerás
    void calcularFilaRecursiva(const std::vector<int>& preferencias,
                               int inicio,
                               int fin,
                               std::vector<std::pair<Pelicula*, long long>>& fila);
};

} // namespace recomendador