#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <random>
using namespace std;
using namespace std::chrono;

struct Nodo {
    int valor;
    Nodo() : valor(0) {}
    Nodo(int v) : valor(v) {}
};

// Funciones modificadas para min-heap
void flotar(vector<Nodo>& heap, int indice) {
    while (indice > 0) {
        int padre = (indice - 1) / 2;
        if (heap[padre].valor <= heap[indice].valor) break;
        swap(heap[padre], heap[indice]);
        indice = padre;
    }
}

void hundir(vector<Nodo>& heap, int indice, int tamaño) {
    while (2 * indice + 1 < tamaño) {
        int hijo_izq = 2 * indice + 1;
        int hijo_der = 2 * indice + 2;
        int menor = hijo_izq;

        if (hijo_der < tamaño && heap[hijo_der].valor < heap[hijo_izq].valor) {
            menor = hijo_der;
        }

        if (heap[indice].valor <= heap[menor].valor) break;
        swap(heap[indice], heap[menor]);
        indice = menor;
    }
}

void insertar(vector<Nodo>& heap, int valor, int& tamaño) {
    if (tamaño >= heap.size()) {
        heap.push_back(Nodo(valor));
    } else {
        heap[tamaño] = Nodo(valor);
    }
    flotar(heap, tamaño);
    tamaño++;
}

int obtenerMinimo(const vector<Nodo>& heap, int tamaño) {
    if (tamaño == 0) return -1;
    return heap[0].valor;
}

void extraerMinimo(vector<Nodo>& heap, int& tamaño) {
    if (tamaño == 0) return;
    swap(heap[0], heap[tamaño - 1]);
    tamaño--;
    hundir(heap, 0, tamaño);
}

void experimento(const unsigned long long N, unsigned seed) {
    vector<Nodo> heap;
    int tamaño = 0;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);

    // Mediciones de tiempo
    auto inicio_insert = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        insertar(heap, dist(gen), tamaño);
    }
    auto fin_insert = high_resolution_clock::now();

    auto inicio_consulta = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        volatile int minimo = obtenerMinimo(heap, tamaño);
    }
    auto fin_consulta = high_resolution_clock::now();

    auto inicio_extraccion = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        if (tamaño > 0) extraerMinimo(heap, tamaño);
    }
    auto fin_extraccion = high_resolution_clock::now();

    // Resultados
    cout << "=== Resultados para N = " << N << " ===" << endl;
    cout << "Insercion: \t"
         << duration_cast<microseconds>(fin_insert - inicio_insert).count()
         << " microseg\t  en promedio :"<< (duration_cast<nanoseconds>(fin_insert - inicio_insert).count())/N
         << " nanoseg/und" << endl;
    cout << "Consulta minimo: \t"
         << duration_cast<microseconds>(fin_consulta - inicio_consulta).count()
         << " microseg\t  en promedio :"<< (duration_cast<nanoseconds>(fin_consulta - inicio_consulta).count())/N
         << " nanoseg/und" << endl;
    cout << "Extraccion minimo: \t"
         << duration_cast<microseconds>(fin_extraccion - inicio_extraccion).count()
         << " microseg\t  en promedio :"<< (duration_cast<nanoseconds>(fin_extraccion - inicio_extraccion).count())/N
         << " nanoseg/und" << endl;
    cout << "----------------------------------" << endl;
}

int main() {
    const unsigned seed = 123456789;
    const unsigned long long valores_N[] = {1000, 10000, 100000, 1000000};

    for (auto N : valores_N) {
        experimento(N, seed);
    }

    return 0;
}