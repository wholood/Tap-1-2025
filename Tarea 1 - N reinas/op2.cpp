#include <iostream>
#include "stopWatch.hpp"
#include <iomanip>

using namespace std;

const int N = 64; 
const int iteraciones = 1;
int solucion[N];   
bool encontrado = false;

bool mismaDiagonal(int fila1, int col1, int fila2, int col2) {
    return abs(fila1 - fila2) == abs(col1 - col2);
}

void imprimirSolucion(int arr[]) {
    string sol;
    for(int i = 0; i < N; i++) {
        sol = 'A' + i;
        for(int j = 0; j < N; j++) {
            cout << (arr[i] == j ? sol+' ' : ". ");
        }
        cout << endl;
    }
    cout << "------------" << endl;
}

void permutacion(int paso, int A[]) {
    if(encontrado) return;
    
    if(paso == N) {
        for(int i = 0; i < N; i++) solucion[i] = A[i];
        encontrado = true;
        return;
    }
    
    for(int i = paso; i < N; i++) {
        bool valido = true;
        
        for(int k = 0; k < paso; k++) {
            if(mismaDiagonal(k, A[k], paso, A[i])) {
                valido = false;
                break;
            }
        }
        
        if(valido) {
            swap(A[paso], A[i]);
            permutacion(paso + 1, A);
            swap(A[paso], A[i]);
            
            if(encontrado) return; 
        }
    }
}

int main() {
    int columnas[N];
    double tiemposMili=0;
    double tiemposMicro=0;

    for (int i = 0; i < iteraciones; ++i) {
        encontrado = false;
        for(int i = 0; i < N; i++) columnas[i] = i;

        auto watchIter = StopWatch::start();
        permutacion(0, columnas);
        auto timeIter = watchIter.stop();

        if (encontrado) imprimirSolucion(solucion);
        tiemposMili += timeIter.getElapsedTimeMiliSeconds();
        tiemposMicro += timeIter.getElapsedTimeMicroSeconds();
    }

    cout << fixed << setprecision(9);

    cout << "Cantidad de reinas: " << N << endl;
    cout << "Cantidad de iteraciones: " << iteraciones << endl;
    cout << "Tiempo promedio en milisegundos:" << tiemposMili / iteraciones << endl;
    cout << "Tiempos promedio en microsegundos:" << tiemposMicro / iteraciones << endl;
    
    
    return 0;
}