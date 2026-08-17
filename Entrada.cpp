#include "Entrada.h"
#include "Modelos.h"

#include <cctype>
#include <iostream>
#include <sstream>

namespace recomendador {

namespace {

// Intenta convertir texto a entero de forma estricta (sin espacios ni basura).
bool intentarConvertirAEntero(const std::string& texto, int& resultado) {
    if (texto.empty()) {
        return false;
    }

    size_t inicio = 0;
    if (texto[0] == '-' || texto[0] == '+') {
        inicio = 1;
    }
    if (inicio == texto.size()) {
        return false;
    }
    for (size_t i = inicio; i < texto.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(texto[i]))) {
            return false;
        }
    }

    try {
        resultado = std::stoi(texto);
    } catch (...) {
        return false;
    }
    return true;
}

std::string leerLinea(const std::string& mensaje) {
    std::cout << mensaje;
    std::string linea;
    std::getline(std::cin, linea);
    return linea;
}

} // namespace

std::string pedirTextoNoVacio(const std::string& mensaje) {
    while (true) {
        std::string valor = leerLinea(mensaje);

        size_t inicio = valor.find_first_not_of(" \t");
        if (inicio == std::string::npos) {
            std::cout << "Por favor ingrese un valor\n";
            continue;
        }
        size_t fin = valor.find_last_not_of(" \t");
        return valor.substr(inicio, fin - inicio + 1);
    }
}

int pedirEntero(const std::string& mensaje) {
    while (true) {
        std::string linea = leerLinea(mensaje);
        if (linea.empty()) {
            std::cout << "Por favor ingrese un valor\n";
            continue;
        }

        int valor;
        if (!intentarConvertirAEntero(linea, valor)) {
            std::cout << "Valor inválido, debe ser un número entero.\n";
            continue;
        }
        return valor;
    }
}

int pedirEnteroEnRango(const std::string& mensaje, int minimo, int maximo) {
    while (true) {
        std::string linea = leerLinea(mensaje);
        if (linea.empty()) {
            std::cout << "Por favor ingrese un valor\n";
            continue;
        }

        int valor;
        if (!intentarConvertirAEntero(linea, valor) || valor < minimo || valor > maximo) {
            std::cout << "Valor inválido, debe ser un entero entre " << minimo
                       << " y " << maximo << ".\n";
            continue;
        }
        return valor;
    }
}

std::vector<int> pedirVectorGeneros(int minimo, int maximo) {
    std::vector<int> valores;
    valores.reserve(NUM_GENEROS);
    for (const auto& genero : GENEROS) {
        std::ostringstream mensaje;
        mensaje << genero << " (" << minimo << "-" << maximo << "): ";
        valores.push_back(pedirEnteroEnRango(mensaje.str(), minimo, maximo));
    }
    return valores;
}

bool pedirConfirmacionSN(const std::string& mensaje) {
    while (true) {
        std::string linea = leerLinea(mensaje);
        if (linea.size() == 1) {
            char c = static_cast<char>(std::toupper(static_cast<unsigned char>(linea[0])));
            if (c == 'S') return true;
            if (c == 'N') return false;
        }
        std::cout << "Respuesta inválida, ingrese S o N.\n";
    }
}

} // namespace recomendador