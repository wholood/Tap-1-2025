#include <iostream>
#include <chrono>
#include <iomanip>
using namespace std;

const int N = 16; 
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
    for(int i = 0; i < N; i++) columnas[i] = i;
    
    auto inicio = chrono::high_resolution_clock::now();
    permutacion(0, columnas);
    auto fin = chrono::high_resolution_clock::now();
    
    auto duracion = chrono::duration_cast<chrono::microseconds>(fin - inicio);
    double tiempo = duracion.count() / 1000.0; 
    
    if(encontrado) {
        imprimirSolucion(solucion);
    }
    
    cout << fixed << setprecision(6);
    cout << "Tiempo de ejecucion: " << tiempo << " ms" << endl;
    
    return 0;
}