#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <random>
#include <tuple>
#include <numeric>
using namespace std;
using namespace std::chrono;

struct Nodo {
    int valor;
    Nodo() : valor(0) {}
    Nodo(int v) : valor(v) {}
};

// Funciones básicas del min-heap
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

        if (hijo_der < tamaño && heap[hijo_der].valor < heap[hijo_izq].valor)
            menor = hijo_der;

        if (heap[indice].valor <= heap[menor].valor) break;
        swap(heap[indice], heap[menor]);
        indice = menor;
    }
}

void insertar(vector<Nodo>& heap, int valor, int& tamaño) {
    if (tamaño >= heap.size()) heap.push_back(Nodo(valor));
    else heap[tamaño] = Nodo(valor);
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

void heapify(vector<Nodo>& heap, int tamaño) {
    for (int i = (tamaño - 1)/2; i >= 0; i--)
        hundir(heap, i, tamaño);
}

void unir(vector<Nodo>& heap1, int& tam1, const vector<Nodo>& heap2, int tam2) {
    //No está definida
}

void generarheap(unsigned seed, int N, vector<Nodo>& heap) {
    vector<int> numeros;
    mt19937 gen(123456789);
    uniform_int_distribution<int> dist(1, 1000000);
    
    int tam = 0;
    for (int i = 0; i < N; ++i)
        numeros.push_back(dist(gen));
    for (int num : numeros)
        insertar(heap, num, tam);
    
}
// Experimento 1: Rendimiento de inserciones
void experimento1(unsigned seed, int M) {
    vector<int> numeros;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);

    for (int i = 0; i < M; ++i)
        numeros.push_back(dist(gen));

    long long tiempo;
    vector<Nodo> heap;
    int tam = 0;
    
    auto inicio = high_resolution_clock::now();
    for (int num : numeros){
        insertar(heap, num, tam);
    }
    auto fin = high_resolution_clock::now();
    tiempo=duration_cast<microseconds>(fin - inicio).count();

    cout << "\nPara M=" << M 
         << "\nTiempo total= " << tiempo << " microsegundos\n"
         << "Tiempo promedio por insercion= " << (double)tiempo / M << " microsegundos.\n";
    cout << "----------------------------------\n";
}

// Experimento 2: Mezcla de operaciones
void experimento2(unsigned seed, int O) {
    vector<int> operaciones;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist_op(1, 3); // cambiar 1,4 para unión

    int ins = 0, cons = 0, ext = 0, uni=0;
    for (int i = 0; i < O; ++i) {
        int op = dist_op(gen);
        operaciones.push_back(op);
        if (op == 1) ins++;
        else if (op == 2) cons++;
        else if (op ==3) ext++;
        else uni++; 
    }

    long long tiempo;
    vector<Nodo> heap;
    int tam = 0, m;
    mt19937 gen_num(seed);
    uniform_int_distribution<int> dist_num(1, 1000000);
    generarheap(seed, 100, heap); //Crea un heap inicial de 100 nodos

    auto inicio = high_resolution_clock::now();
    for (int op : operaciones) {
        switch(op) {
            case 1: insertar(heap, dist_num(gen_num), tam); break;
            case 2: m = obtenerMinimo(heap, tam); break;
            case 3: if (tam > 0) extraerMinimo(heap, tam); break;
            /*case 4: {
                vector<Nodo> heap2;
                int tam2 = 0;
                generarheap(seed, dist_num(gen_num), heap2);
                unir(heap, tam, heap2, tam2);
                break;*/
        }
    }
    auto fin = high_resolution_clock::now();
    tiempo=duration_cast<microseconds>(fin - inicio).count();


    cout << "\nPara O=" << O 
         << "\nTiempo total= " << tiempo << " microsegundos\n"
         << "Tiempo promedio por operacion= " << (double)tiempo / O << " microsegundos.\n"
         << "Cantidad de inserciones=" << ins 
         << ", consulta tope=" << cons
         //<< ", uniones=" << uni
         << ", obtener tope=" << ext << ".\n";
    cout << "----------------------------------\n";
}

// Experimento 3: Secuencia completa
void experimento3(unsigned seed, int N) {
    vector<vector<long long>> tiempos(4); // 0:ins, 1:cons, 2:ext, 3:union
    vector<Nodo> heap;
    int tam = 0;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);

    // Inserciones
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        insertar(heap, dist(gen), tam);
    auto fin = high_resolution_clock::now();
    tiempos[0].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Consultas
    int m;
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        m = obtenerMinimo(heap, tam);
    fin = high_resolution_clock::now();
    tiempos[1].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Extracciones
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        if (tam > 0) extraerMinimo(heap, tam);
    fin = high_resolution_clock::now();
    tiempos[2].push_back(duration_cast<microseconds>(fin - inicio).count());

    // 
    tiempos[3].push_back(9999);
    

    string tipos[4] = {"Insercion", "Consulta", "Extraccion", "Union"};
    cout << "\nPara N=" << N << endl;
    for (int i = 0; i < 4; ++i) {
        auto [min, max] = minmax_element(tiempos[i].begin(), tiempos[i].end());
        double prom = accumulate(tiempos[i].begin(), tiempos[i].end(), 0.0) / N;
        cout << tipos[i] << ": min=" << *min << "microseg, max=" << *max 
             << "microseg, promedio=" << prom << "microseg\n";
    }
    cout << "----------------------------------\n";
}

int main() {
    unsigned seed;
    cin >> seed;
    vector<int> valores_N;
    int n;
    while (cin >> n && n != -1) {
        valores_N.push_back(n);
    }
    if (valores_N.empty()) {
        cout << "No se ingresaron valores, usando {100, 1000, 10000, 100000, 1000000} por defecto.\n";
        valores_N = {100, 1000, 10000, 100000, 1000000};
    }

    // Experimento 1
    cout << "======Experimento 1: Inserciones." << endl;
    for (auto M : valores_N) {
        experimento1(seed, M);;
    }
    
    cout << "======Experimento 2: Operaciones." << endl;
    for (auto O : valores_N) {
        experimento2(seed, O);
    }
    cout << "======Experimento 3: Operaciones en secuencia." << endl;
    for (auto N : valores_N) {
        experimento3(seed, N);
    }
    

    return 0;
}