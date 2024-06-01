#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> // Para inet_ntop

const int FILAS = 6; // Número de filas del tablero
const int COLUMNAS = 7; // Número de columnas del tablero
const char PIEZA_SERVIDOR = 'S'; // Pieza del servidor
const char PIEZA_CLIENTE = 'C'; // Pieza del cliente

int contadorClientes = 0; // Contador global de clientes

struct EstadoJuego {
    char tablero[FILAS][COLUMNAS]; // Tablero del juego
    bool turnoServidor; // Indica si es el turno del servidor
    bool juegoTerminado; // Indica si el juego ha terminado
    char ganador; // Ganador del juego (' ' para empate)
};

// Inicializar el tablero
void inicializarTablero(EstadoJuego &juego) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            juego.tablero[i][j] = ' ';
        }
    }
    juego.turnoServidor = rand() % 2 == 0;
    juego.juegoTerminado = false;
    juego.ganador = ' ';
}

// Soltar una pieza en una columna
bool soltarPieza(EstadoJuego &juego, int columna, char pieza) {
    if (columna < 0 || columna >= COLUMNAS || juego.tablero[0][columna] != ' ') return false;
    for (int i = FILAS - 1; i >= 0; --i) {
        if (juego.tablero[i][columna] == ' ') {
            juego.tablero[i][columna] = pieza;
            return true;
        }
    }
    return false;
}

// Verificar si hay un ganador
bool verificarGanador(EstadoJuego &juego, char pieza) {
    // Verificar victorias horizontales, verticales y diagonales
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (juego.tablero[i][j] == pieza) {
                // Horizontal
                if (j + 3 < COLUMNAS &&
                    juego.tablero[i][j + 1] == pieza &&
                    juego.tablero[i][j + 2] == pieza &&
                    juego.tablero[i][j + 3] == pieza)
                    return true;
                // Vertical
                if (i + 3 < FILAS &&
                    juego.tablero[i + 1][j] == pieza &&
                    juego.tablero[i + 2][j] == pieza &&
                    juego.tablero[i + 3][j] == pieza)
                    return true;
                // Diagonal
                if (i + 3 < FILAS && j + 3 < COLUMNAS &&
                    juego.tablero[i + 1][j + 1] == pieza &&
                    juego.tablero[i + 2][j + 2] == pieza &&
                    juego.tablero[i + 3][j + 3] == pieza)
                    return true;
                if (i - 3 >= 0 && j + 3 < COLUMNAS &&
                    juego.tablero[i - 1][j + 1] == pieza &&
                    juego.tablero[i - 2][j + 2] == pieza &&
                    juego.tablero[i - 3][j + 3] == pieza)
                    return true;
            }
        }
    }
    return false;
}

// Verificar si el tablero está lleno (empate)
bool tableroLleno(const EstadoJuego &juego) {
    for (int j = 0; j < COLUMNAS; ++j) {
        if (juego.tablero[0][j] == ' ') {
            return false;
        }
    }
    return true;
}

// Manejar la conexión con el cliente
void manejarCliente(int socketCliente, sockaddr_in direccionCliente, bool esPrimerCliente) {
    char IPCliente[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &direccionCliente.sin_addr, IPCliente, INET_ADDRSTRLEN);
    int puertoCliente = ntohs(direccionCliente.sin_port);

    int idCliente = ++contadorClientes; // Asignar un ID único al cliente

    if (esPrimerCliente) {
        std::cout << "Juego nuevo servidor [" << IPCliente << ":" << puertoCliente << "]" << std::endl;
        std::cout << "Juego nuevo cliente [" << IPCliente << ":" << puertoCliente << "]" << std::endl;
    } else {
        std::cout << "Juego nuevo cliente [" << IPCliente << ":" << puertoCliente << "]" << std::endl;
    }

    EstadoJuego juego;
    inicializarTablero(juego);

    if (juego.turnoServidor) {
        std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: inicia juego el servidor." << std::endl;
    } else {
        std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: inicia juego el cliente." << std::endl;
    }

    while (!juego.juegoTerminado) {
        // Enviar tablero y turno al cliente
        if (send(socketCliente, &juego, sizeof(EstadoJuego), 0) <= 0) {
            std::cout << "Error enviando datos al cliente [" << IPCliente << ":" << puertoCliente << "]." << std::endl;
            break;
        }

        if (!juego.turnoServidor) {
            // Obtener movimiento del cliente
            int columna;
            if (recv(socketCliente, &columna, sizeof(int), 0) <= 0) {
                std::cout << "Cliente [" << IPCliente << ":" << puertoCliente << "] desconectado." << std::endl;
                close(socketCliente);
                return;
            }

            char pieza = PIEZA_CLIENTE;
            if (soltarPieza(juego, columna, pieza)) {
                std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: cliente juega columna " << columna + 1 << "." << std::endl;
                if (verificarGanador(juego, pieza)) {
                    juego.juegoTerminado = true;
                    juego.ganador = pieza;
                    std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: gana el cliente." << std::endl;
                } else if (tableroLleno(juego)) {
                    juego.juegoTerminado = true;
                    juego.ganador = ' ';
                    std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: empate." << std::endl;
                }
                juego.turnoServidor = !juego.turnoServidor;
            }
        } else {
            // Turno del servidor
            int columna = rand() % COLUMNAS;

            char pieza = PIEZA_SERVIDOR;
            if (soltarPieza(juego, columna, pieza)) {
                std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: servidor juega columna " << columna + 1 << "." << std::endl;
                if (verificarGanador(juego, pieza)) {
                    juego.juegoTerminado = true;
                    juego.ganador = pieza;
                    std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: gana el servidor." << std::endl;
                } else if (tableroLleno(juego)) {
                    juego.juegoTerminado = true;
                    juego.ganador = ' ';
                    std::cout << "Juego [" << IPCliente << ":" << puertoCliente << "]: empate." << std::endl;
                }
                juego.turnoServidor = !juego.turnoServidor;
            }
        }
    }

    // Enviar estado final del juego al cliente
    send(socketCliente, &juego, sizeof(EstadoJuego), 0);
    close(socketCliente);
    std::cout << "Cerrando conexión con cliente [" << IPCliente << ":" << puertoCliente << "]." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>\n";
        return 1;
    }

    int puerto = std::stoi(argv[1]);
    int socketServidor = socket(AF_INET, SOCK_STREAM, 0);

    // Configurar el socket para reutilizar direcciones
    int opt = 1;
    if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error configurando opciones del socket\n";
        return 1;
    }

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    if (bind(socketServidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) {
        std::cerr << "Error en bind\n";
        return 1;
    }
    if (listen(socketServidor, 5) < 0) {
        std::cerr << "Error en listen\n";
        return 1;
    }

    std::cout << "Esperando conexiones en el puerto " << puerto << " ..." << std::endl;

    bool esPrimerCliente = true;
    while (true) {
        sockaddr_in direccionCliente;
        socklen_t longitudCliente = sizeof(direccionCliente);
        int socketCliente = accept(socketServidor, (struct sockaddr*)&direccionCliente, &longitudCliente);
        if (socketCliente < 0) {
            std::cerr << "Error en accept\n";
            continue;
        }
        std::thread(manejarCliente, socketCliente, direccionCliente, esPrimerCliente).detach();
        esPrimerCliente = false;  // Después del primer cliente, todos serán clientes adicionales
    }

    close(socketServidor);
    return 0;
}
