#ifndef MENU_H
#define MENU_H

#include "Sistema.h"

namespace recomendador {

class Menu {
public:
    Menu(Sistema& sistema);
    void ejecutar();

private:
    Sistema& sistema_;

    // Utilidad de presentación
    void pausar() const;

    // Submenús principales
    void menuInsertar();
    void menuConsultar();
    void menuEditar();
    void menuEliminar();

    // Lógica detallada
    void insertarUsuario();
    void insertarPelicula();
    
    void consultarUsuarioPorId();
    void consultarPeliculaPorId();
    void consultarRecomendaciones();
    
    void editarUsuario();
    void editarPelicula();
    
    void eliminarUsuario();
    void eliminarPelicula();

    // Mostrado por pantalla
    void mostrarUsuario(const Usuario& usuario) const;
    void mostrarPelicula(const Pelicula& pelicula) const;
};

} // namespace recomendador

#endif