#include "Menu.h"
#include "Sistema.h"

int main() {
    recomendador::Sistema sistema;
    sistema.cargarDatosIniciales();

    recomendador::Menu menu(sistema);
    menu.ejecutar();

    return 0;
}