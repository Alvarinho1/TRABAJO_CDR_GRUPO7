#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits> // Para std::numeric_limits

const int FILAS = 6; // Número de filas del tablero
const int COLUMNAS = 7; // Número de columnas del tablero
const char PIEZA_SERVIDOR = 'S'; // Pieza del servidor
const char PIEZA_CLIENTE = 'C'; // Pieza del cliente

struct EstadoJuego {
    char tablero[FILAS][COLUMNAS]; // Tablero del juego
    bool turnoServidor; // Indica si es el turno del servidor
    bool juegoTerminado; // Indica si el juego ha terminado
    char ganador; // Ganador del juego (' ' para empate)
};

// Imprimir el tablero
void imprimirTablero(const EstadoJuego &juego) {
    for (int i = 0; i < FILAS; ++i) {
        std::cout << "  ";
        for (int j = 0; j < COLUMNAS; ++j) {
            std::cout << "----";
        }
        std::cout << "-" << std::endl;
        std::cout << " " << i + 1 << " |";
        for (int j = 0; j < COLUMNAS; ++j) {
            std::cout << " " << (juego.tablero[i][j] == ' ' ? ' ' : juego.tablero[i][j]) << " |";
        }
        std::cout << std::endl;
    }

    // Imprimir la línea de fondo del tablero
    std::cout << "  ";
    for (int j = 0; j < COLUMNAS; ++j) {
        std::cout << "----";
    }
    std::cout << "-" << std::endl;

    // Imprimir los números de las columnas
    std::cout << "   ";
    for (int j = 0; j < COLUMNAS; ++j) {
        std::cout << " " << j + 1 << "  ";
    }
    std::cout << std::endl;

    // Imprimir una línea en blanco para separar visualmente cada jugada
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <IP del servidor> <puerto>\n";
        return 1;
    }

    const char* IPServidor = argv[1]; // IP del servidor
    int puerto = std::stoi(argv[2]); // Puerto del servidor
    int socketCliente = socket(AF_INET, SOCK_STREAM, 0); // Crear socket del cliente

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    inet_pton(AF_INET, IPServidor, &direccionServidor.sin_addr);

    // Conectar al servidor
    if (connect(socketCliente, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) {
        std::cerr << "Error al conectar\n";
        return 1;
    }

    EstadoJuego juego;
    bool continuarJugando = true;
    while (continuarJugando) {
        // Recibir estado del juego
        recv(socketCliente, &juego, sizeof(EstadoJuego), 0);
        imprimirTablero(juego);

        if (juego.juegoTerminado) {
            if (juego.ganador == PIEZA_CLIENTE) {
                std::cout << "¡Gana cliente!" << std::endl;
            } else if (juego.ganador == PIEZA_SERVIDOR) {
                std::cout << "¡Gana servidor!" << std::endl;
            } else {
                std::cout << "¡Empate!" << std::endl;
            }
            std::cout << "Fin del juego." << std::endl;
            break;
        }

        if (!juego.turnoServidor) {
            int columna;
            bool entradaValida = false;
            while (!entradaValida) {
                std::cout << "Elige una columna (1-7): ";
                std::cin >> columna;

                if (std::cin.fail() || columna < 1 || columna > 7) {
                    std::cout << "Entrada inválida. Por favor, ingrese un número del 1 al 7." << std::endl;
                    std::cin.clear(); // Limpiar la bandera de error
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descartar la entrada inválida
                } else {
                    entradaValida = true;
                }
            }

            columna--; // Ajustar para índice basado en 0
            send(socketCliente, &columna, sizeof(int), 0);
        }
    }

    // Cerrar el socket del cliente
    close(socketCliente);

    return 0;
}
