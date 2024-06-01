# TRABAJO: Comunicación de Datos y Redes GRUPO7

# Conecta 4 - Juego de Cliente y Servidor

Este proyecto implementa un juego de Conecta 4 utilizando sockets para la comunicación entre un cliente y un servidor.

## Requisitos

- Sistema operativo Linux
- Compilador g++ (o cualquier otro compatible con C++)
- Biblioteca estándar de sockets (arpa/inet.h, netinet/in.h, etc.)
- `make` para la compilación

## Estructura del Proyecto

- `cliente.cpp`: Código del cliente que se conecta al servidor.
- `servidor.cpp`: Código del servidor que maneja el juego.
- `Makefile`: Archivo para la compilación de los ejecutables.
- `README.md`: Instrucciones de uso.

## Compilación

Para compilar los archivos fuente, ejecute el siguiente comando en la terminal:

```sh make
Esto generará los ejecutables cliente y servidor.

## Ejecución
# Servidor

Para iniciar el servidor, ejecute el siguiente comando:
./servidor <puerto>

Reemplace <puerto> con el número de puerto que desee usar. Por ejemplo:
./servidor 12345

## Cliente

Para iniciar el cliente y conectarlo al servidor, ejecute el siguiente comando:
./cliente <IP del servidor> <puerto>

Reemplace <IP del servidor> con la dirección IP del servidor y <puerto> con el número de puerto utilizado por el servidor. Por ejemplo:
./cliente 127.0.0.1 12345

## Uso del Juego

El servidor y el cliente se turnarán para colocar piezas en el tablero.
El cliente debe ingresar el número de columna (del 1 al 7) donde desea colocar su pieza.
El juego continúa hasta que haya un ganador o el tablero esté lleno (empate).

## Notas
Asegúrese de que el servidor esté ejecutándose antes de iniciar el cliente.
El cliente y el servidor deben usar el mismo número de puerto para comunicarse.
