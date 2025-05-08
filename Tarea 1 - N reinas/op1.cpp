#include <iostream>
#include "stopWatch.hpp"
#include <iomanip>
using namespace std;

const int N = 8;
const int iteraciones = 10;
bool encontrado = false; 
int solucion[N][N];

void procesarSolucion(int tablero[N][N]) {
    string sol;
    for (int i = 0; i < N; ++i) {
        sol = 'A' + i;
        for (int j = 0; j < N; ++j) {
            cout << (tablero[i][j] == 1 ? sol+' ' : ". ");
        }
        cout << endl;
    }
    cout << "------------" << endl;
}

void marcarColumna(int columna, int fila, int id, int tablero[N][N]) {
    for (int i = fila + 1; i < N; ++i) {
        if (tablero[i][columna] == 0) {
            tablero[i][columna] = id;
        }
    }
}

void marcarDiagonalIzquierda(int fila, int columna, int id, int tablero[N][N]) {
    int i = fila + 1;
    int j = columna - 1;
    while (i < N && j >= 0) {
        if (tablero[i][j] == 0) {
            tablero[i][j] = id;
        }
        i++;
        j--;
    }
}

void marcarDiagonalDerecha(int fila, int columna, int id, int tablero[N][N]) {
    int i = fila + 1;
    int j = columna + 1;
    while (i < N && j < N) {
        if (tablero[i][j] == 0) {
            tablero[i][j] = id;
        }
        i++;
        j++;
    }
}

void desmarcarColumna(int columna, int fila, int id, int tablero[N][N]) {
    for (int i = fila + 1; i < N; ++i) {
        if (tablero[i][columna] == id) {
            tablero[i][columna] = 0;
        }
    }
}

void desmarcarDiagonalIzquierda(int fila, int columna, int id, int tablero[N][N]) {
    int i = fila + 1;
    int j = columna - 1;
    while (i < N && j >= 0) {
        if (tablero[i][j] == id) {
            tablero[i][j] = 0;
        }
        i++;
        j--;
    }
}

void desmarcarDiagonalDerecha(int fila, int columna, int id, int tablero[N][N]) {
    int i = fila + 1;
    int j = columna + 1;
    while (i < N && j < N) {
        if (tablero[i][j] == id) {
            tablero[i][j] = 0;
        }
        i++;
        j++;
    }
}

void reinas(int fila, int tablero[N][N]) {
    if (encontrado) return;
    
    if (fila == N) {
        // Guarda solución
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                solucion[i][j] = tablero[i][j];
            }
        }
        encontrado = true;
        return;
    }

    for (int col = 0; col < N; ++col) {
        if (tablero[fila][col] == 0) {
            int id = fila + 2;
            
            // Marcar zonas
            marcarColumna(col, fila, id, tablero);
            marcarDiagonalIzquierda(fila, col, id, tablero);
            marcarDiagonalDerecha(fila, col, id, tablero);
            tablero[fila][col] = 1;
            
            reinas(fila + 1, tablero);
            
            // Desmarcar
            desmarcarColumna(col, fila, id, tablero);
            desmarcarDiagonalIzquierda(fila, col, id, tablero);
            desmarcarDiagonalDerecha(fila, col, id, tablero);
            tablero[fila][col] = 0;
        }
    }
}

int main() {
    int tablero[N][N] = {0};

    double tiemposMili=0;
    double tiemposMicro=0;

    for (int i = 0; i < iteraciones; ++i) {
        encontrado = false;
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                tablero[j][k] = 0;
            }
        }

        auto watchIter = StopWatch::start();
        reinas(0, tablero);
        auto timeIter = watchIter.stop();

        if (encontrado) procesarSolucion(solucion);
        tiemposMili += timeIter.getElapsedTimeMiliSeconds();
        tiemposMicro += timeIter.getElapsedTimeMicroSeconds();
    }

    cout << fixed << setprecision(9);
    cout << "Tiempo promedio en milisegundos:" << tiemposMili / iteraciones << endl;

    cout << "Tiempos promedio en microsegundos:" << tiemposMicro / iteraciones << endl;
    
    
    return 0;
}