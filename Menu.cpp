#include "Menu.h"
#include "Entrada.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace recomendador {

// Secuencias de escape ANSI para negrita
const std::string BOLD = "\033[1m";
const std::string RESET = "\033[0m";

Menu::Menu(Sistema& sistema) : sistema_(sistema) {}

void Menu::pausar() const {
    std::cout << "\nPresione ENTER para continuar...";
    std::string temp;
    std::getline(std::cin, temp);
}

void Menu::ejecutar() {
    while (true) {
        std::cout << "\n" << BOLD << "==== SISTEMA DE RECOMENDACIÓN DE PELÍCULAS ====" << RESET << "\n";
        std::cout << "1) Insertar información\n";
        std::cout << "2) Consultar información\n";
        std::cout << "3) Editar información\n";
        std::cout << "4) Eliminar información\n";
        std::cout << "5) Salir\n";
        
        int opcion = pedirEntero("\nSeleccione una opción: ");

        switch (opcion) {
            case 1: menuInsertar(); pausar(); break;
            case 2: menuConsultar(); pausar(); break;
            case 3: menuEditar(); pausar(); break;
            case 4: menuEliminar(); pausar(); break;
            case 5:
                std::cout << "\nGracias por usar el sistema. ¡Hasta pronto!\n";
                return;
            default:
                std::cout << "\nOpción inválida, intente de nuevo.\n";
                pausar();
        }
    }
}

// ---------------------------------------------------------------------
// 4.1 Insertar
// ---------------------------------------------------------------------

void Menu::menuInsertar() {
    std::cout << "\n" << BOLD << "¿Qué desea insertar?" << RESET << "\n";
    std::cout << "1) Usuario\n2) Película\n";
    int opcion = pedirEntero("\nSeleccione una opción: ");
    
    if (opcion == 1) insertarUsuario();
    else if (opcion == 2) insertarPelicula();
    else std::cout << "\nOpción inválida.\n";
}

void Menu::insertarUsuario() {
    int id = pedirEntero("\nid_usuario: ");
    if (sistema_.existeUsuario(id)) {
        std::cout << "El id_usuario ya existe. No se puede insertar un usuario duplicado.\n";
        return;
    }

    std::string nombre = pedirTextoNoVacio("nombre: ");

    std::string email = pedirTextoNoVacio("email: ");
    if (sistema_.emailExiste(email)) {
        std::cout << "El email ya está registrado.\n";
        return;
    }

    std::cout << "\nIngrese las preferencias del usuario (escala 1-10):\n";
    std::vector<int> preferencias = pedirVectorGeneros(1, 10);

    sistema_.insertarUsuario(Usuario{id, nombre, email, preferencias});
    std::cout << "\nUsuario insertado correctamente.\n";
}

void Menu::insertarPelicula() {
    int id = pedirEntero("\nid_pelicula: ");
    if (sistema_.existePelicula(id)) {
        std::cout << "El id_pelicula ya existe. No se puede insertar una película duplicada.\n";
        return;
    }

    std::string titulo = pedirTextoNoVacio("titulo: ");
    int anio = pedirEntero("anio: ");

    std::cout << "\nIngrese el grado de pertenencia a cada género (escala 0-10):\n";
    std::vector<int> generos = pedirVectorGeneros(0, 10);

    sistema_.insertarPelicula(Pelicula{id, titulo, anio, generos});
    std::cout << "\nPelícula insertada correctamente.\n";
}

// ---------------------------------------------------------------------
// 4.2 Consultar
// ---------------------------------------------------------------------

void Menu::menuConsultar() {
    std::cout << "\n" << BOLD << "¿Qué desea consultar?" << RESET << "\n";
    std::cout << "1) Usuario por id\n2) Película por id\n3) Recomendaciones de un usuario\n";
    
    int opcion = pedirEntero("\nSeleccione una opción: ");
    
    if (opcion == 1) consultarUsuarioPorId();
    else if (opcion == 2) consultarPeliculaPorId();
    else if (opcion == 3) consultarRecomendaciones();
    else std::cout << "\nOpción inválida.\n";
}

void Menu::consultarUsuarioPorId() {
    int id = pedirEntero("\nid_usuario a consultar: ");
    Usuario* usuario = sistema_.buscarUsuario(id);
    if (!usuario) {
        std::cout << "\nNo se encontró ningún registro con el id ingresado.\n";
        return;
    }
    std::cout << "\n";
    mostrarUsuario(*usuario);
}

void Menu::consultarPeliculaPorId() {
    int id = pedirEntero("\nid_pelicula a consultar: ");
    Pelicula* pelicula = sistema_.buscarPelicula(id);
    if (!pelicula) {
        std::cout << "\nNo se encontró ningún registro con el id ingresado.\n";
        return;
    }
    std::cout << "\n";
    mostrarPelicula(*pelicula);
}

void Menu::consultarRecomendaciones() {
    int id = pedirEntero("\nid_usuario: ");
    Usuario* usuario = sistema_.buscarUsuario(id);
    if (!usuario) {
        std::cout << "\nUsuario no encontrado.\n";
        return;
    }

    auto fila = sistema_.calcularFilaUsuario(*usuario);
    std::sort(fila.begin(), fila.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "\n" << BOLD << "Recomendaciones para " << usuario->nombre << ":" << RESET << "\n";
    size_t tope = std::min<size_t>(5, fila.size());
    for (size_t i = 0; i < tope; ++i) {
        std::cout << (i + 1) << ") " << std::left << std::setw(25) << fila[i].first->titulo
                   << " -> afinidad " << fila[i].second << "\n";
    }
}

// ---------------------------------------------------------------------
// 4.3 Editar
// ---------------------------------------------------------------------

void Menu::menuEditar() {
    std::cout << "\n" << BOLD << "¿Qué desea editar?" << RESET << "\n";
    std::cout << "1) Usuario\n2) Película\n";
    
    int opcion = pedirEntero("\nSeleccione una opción: ");
    
    if (opcion == 1) editarUsuario();
    else if (opcion == 2) editarPelicula();
    else std::cout << "\nOpción inválida.\n";
}

void Menu::editarUsuario() {
    int id = pedirEntero("\nid_usuario a editar: ");
    Usuario* usuario = sistema_.buscarUsuario(id);
    if (!usuario) {
        std::cout << "\nNo se encontró ningún registro con ese id. No es posible editar.\n";
        return;
    }

    std::cout << "\n" << BOLD << "¿Qué campo desea editar?" << RESET << "\n";
    std::cout << "1) Nombre\n2) Email\n";
    std::cout << "3) Preferencia de género (Acción/Comedia/Drama/Terror/Romance)\n";
    std::cout << "4) Cancelar\n";
    
    int opcion = pedirEntero("\nSeleccione una opción: ");

    switch (opcion) {
        case 1:
            usuario->nombre = pedirTextoNoVacio("\nNuevo nombre: ");
            std::cout << "\nRegistro actualizado correctamente.\n";
            break;
        case 2: {
            std::string nuevoEmail = pedirTextoNoVacio("\nNuevo email: ");
            if (sistema_.emailExiste(nuevoEmail, id)) {
                std::cout << "\nEl email ya está registrado.\n";
                return;
            }
            usuario->email = nuevoEmail;
            std::cout << "\nRegistro actualizado correctamente.\n";
            break;
        }
        case 3: {
            std::cout << "\n" << BOLD << "¿Qué género desea modificar?" << RESET << "\n";
            for (size_t i = 0; i < GENEROS.size(); ++i) {
                std::cout << (i + 1) << ") " << GENEROS[i] << "\n";
            }
            int indice = pedirEnteroEnRango("\nSeleccione (1-5): ", 1, 5) - 1;
            int nuevoValor =
                pedirEnteroEnRango("Nuevo valor para " + GENEROS[indice] + " (1-10): ", 1, 10);
            usuario->preferencias[indice] = nuevoValor;
            std::cout << "\nRegistro actualizado correctamente.\n";
            break;
        }
        case 4:
            return;
        default:
            std::cout << "\nOpción inválida.\n";
    }
}

void Menu::editarPelicula() {
    int id = pedirEntero("\nid_pelicula a editar: ");
    Pelicula* pelicula = sistema_.buscarPelicula(id);
    if (!pelicula) {
        std::cout << "\nNo se encontró ningún registro con ese id. No es posible editar.\n";
        return;
    }

    std::cout << "\n" << BOLD << "¿Qué campo desea editar?" << RESET << "\n";
    std::cout << "1) Título\n2) Año\n3) Grado de pertenencia a un género\n4) Cancelar\n";
    
    int opcion = pedirEntero("\nSeleccione una opción: ");

    switch (opcion) {
        case 1:
            pelicula->titulo = pedirTextoNoVacio("\nNuevo título: ");
            std::cout << "\nRegistro actualizado correctamente.\n";
            break;
        case 2:
            pelicula->anio = pedirEntero("\nNuevo año: ");
            std::cout << "\nRegistro actualizado correctamente.\n";
            break;
        case 3: {
            std::cout << "\n" << BOLD << "¿Qué género desea modificar?" << RESET << "\n";
            for (size_t i = 0; i < GENEROS.size(); ++i) {
                std::cout << (i + 1) << ") " << GENEROS[i] << "\n";
            }
            int indice = pedirEnteroEnRango("\nSeleccione (1-5): ", 1, 5) - 1;
            int nuevoValor =
                pedirEnteroEnRango("Nuevo valor para " + GENEROS[indice] + " (0-10): ", 0, 10);
            pelicula->generos[indice] = nuevoValor;
            std::cout << "\nRegistro actualizado correctamente.\n";
            break;
        }
        case 4:
            return;
        default:
            std::cout << "\nOpción inválida.\n";
    }
}

// ---------------------------------------------------------------------
// 4.4 Eliminar
// ---------------------------------------------------------------------

void Menu::menuEliminar() {
    std::cout << "\n" << BOLD << "¿Qué desea eliminar?" << RESET << "\n";
    std::cout << "1) Usuario\n2) Película\n";
    
    int opcion = pedirEntero("\nSeleccione una opción: ");
    
    if (opcion == 1) eliminarUsuario();
    else if (opcion == 2) eliminarPelicula();
    else std::cout << "\nOpción inválida.\n";
}

void Menu::eliminarUsuario() {
    int id = pedirEntero("\nid_usuario a eliminar: ");
    Usuario* usuario = sistema_.buscarUsuario(id);
    if (!usuario) {
        std::cout << "\nNo se encontró ningún registro con ese id. No es posible eliminar.\n";
        return;
    }

    std::cout << "\n";
    mostrarUsuario(*usuario);
    std::cout << "\n";
    
    if (pedirConfirmacionSN("¿Está seguro que desea eliminar este registro? (S/N): ")) {
        sistema_.eliminarUsuario(id);
        std::cout << "\nRegistro eliminado correctamente.\n";
    } else {
        std::cout << "\nEliminación cancelada.\n";
    }
}

void Menu::eliminarPelicula() {
    int id = pedirEntero("\nid_pelicula a eliminar: ");
    Pelicula* pelicula = sistema_.buscarPelicula(id);
    if (!pelicula) {
        std::cout << "\nNo se encontró ningún registro con ese id. No es posible eliminar.\n";
        return;
    }

    std::cout << "\n";
    mostrarPelicula(*pelicula);
    std::cout << "\n";
    
    if (pedirConfirmacionSN("¿Está seguro que desea eliminar este registro? (S/N): ")) {
        sistema_.eliminarPelicula(id);
        std::cout << "\nRegistro eliminado correctamente.\n";
    } else {
        std::cout << "\nEliminación cancelada.\n";
    }
}

// ---------------------------------------------------------------------
// Presentación
// ---------------------------------------------------------------------

void Menu::mostrarUsuario(const Usuario& usuario) const {
    std::cout << "id_usuario: " << usuario.id_usuario << "\n";
    std::cout << "nombre: " << usuario.nombre << "\n";
    std::cout << "email: " << usuario.email << "\n";
    for (size_t i = 0; i < GENEROS.size(); ++i) {
        std::cout << "  " << GENEROS[i] << ": " << usuario.preferencias[i] << "\n";
    }
}

void Menu::mostrarPelicula(const Pelicula& pelicula) const {
    std::cout << "id_pelicula: " << pelicula.id_pelicula << "\n";
    std::cout << "titulo: " << pelicula.titulo << "\n";
    std::cout << "anio: " << pelicula.anio << "\n";
    for (size_t i = 0; i < GENEROS.size(); ++i) {
        std::cout << "  " << GENEROS[i] << ": " << pelicula.generos[i] << "\n";
    }
}

} // namespace recomendador