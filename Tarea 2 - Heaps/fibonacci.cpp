#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <random>
#include <tuple>
#include <numeric>
#include <cassert>
#include <unordered_map>
#include <climits>
using namespace std;
using namespace std::chrono;

struct Nodo {
    int clave;
    int grado;
    bool marcado;
    Nodo* padre;
    Nodo* hijo;
    Nodo* siguiente;
    Nodo* anterior;

    Nodo(int k) : clave(k), grado(0), marcado(false), 
                 padre(nullptr), hijo(nullptr), 
                 siguiente(this), anterior(this) {}
};

struct HeapFibonacci {
    Nodo* min;
    int tamano;
};

void enlazarNodos(Nodo* a, Nodo* b) {
    a->anterior->siguiente = b;
    b->anterior->siguiente = a;
    swap(a->anterior, b->anterior);
}

void eliminarDeLista(Nodo* x) {
    x->anterior->siguiente = x->siguiente;
    x->siguiente->anterior = x->anterior;
    x->siguiente = x->anterior = x;
}

HeapFibonacci crearHeap() {
    return {nullptr, 0};
}

void agregarHijo(Nodo* padre, Nodo* hijo) {
    hijo->padre = padre;
    if (!padre->hijo) {
        padre->hijo = hijo;
    } else {
        enlazarNodos(padre->hijo, hijo);
    }
    padre->grado++;
    hijo->marcado = false;
}

void consolidar(HeapFibonacci* heap) {
    int max_grado = log2(heap->tamano) + 1;
    vector<Nodo*> grados(max_grado + 1, nullptr);
    
    vector<Nodo*> raices;
    Nodo* actual = heap->min;
    do {
        raices.push_back(actual);
        actual = actual->siguiente;
    } while(actual != heap->min);

    for (Nodo* raiz : raices) {
        Nodo* x = raiz;
        int grado = x->grado;
        while (grados[grado] != nullptr) {
            Nodo* y = grados[grado];
            if (x->clave > y->clave) swap(x, y);
            eliminarDeLista(y);
            agregarHijo(x, y);
            grados[grado] = nullptr;
            grado++;
        }
        grados[grado] = x;
    }

    heap->min = nullptr;
    for (Nodo* g : grados) {
        if (g) {
            if (!heap->min || g->clave < heap->min->clave) {
                heap->min = g;
            }
        }
    }
}

Nodo* insertar(HeapFibonacci* heap, int clave) {
    Nodo* nuevo = new Nodo(clave);
    if (!heap->min) {
        heap->min = nuevo;
    } else {
        enlazarNodos(heap->min, nuevo);
        if (clave < heap->min->clave) heap->min = nuevo;
    }
    heap->tamano++;
    return nuevo;
}

void unir(HeapFibonacci* destino, HeapFibonacci* fuente) {
    if (!fuente->min) return;
    
    if (!destino->min) {
        *destino = *fuente;
    } else {
        enlazarNodos(destino->min, fuente->min);
        if (fuente->min->clave < destino->min->clave) {
            destino->min = fuente->min;
        }
        destino->tamano += fuente->tamano;
    }
    fuente->min = nullptr;
    fuente->tamano = 0;
}

int obtenerMinimo(HeapFibonacci* heap) {
    if (!heap->min) {
        heap->tamano = 0;
        heap->min = nullptr;
        return -INT_MIN;
    }
    
    Nodo* minimo = heap->min;
    if (minimo->hijo) {
        Nodo* hijo = minimo->hijo;
        do {
            Nodo* siguiente = hijo->siguiente;
            eliminarDeLista(hijo);
            enlazarNodos(heap->min, hijo);
            hijo->padre = nullptr;
            hijo = siguiente;
        } while (hijo != minimo->hijo);
    }
    
    eliminarDeLista(minimo);
    if (minimo == minimo->siguiente) {
        heap->min = nullptr;
    } else {
        heap->min = minimo->siguiente;
        consolidar(heap);
    }
    
    delete minimo;
    heap->tamano--;
    return minimo->clave;
}

void liberarHeap(HeapFibonacci* heap) {
    while (heap->tamano > 0) {
        obtenerMinimo(heap);
    }
}

void generarHeap(unsigned semilla, int n, HeapFibonacci* heap) {
    mt19937 gen(semilla);
    uniform_int_distribution<int> dist(1, 1000000);
    for (int i = 0; i < n; ++i) {
        insertar(heap, dist(gen));
    }
}

// Experimento 1: Rendimiento de inserciones
void experimento1(unsigned seed, int M) {
    vector<int> numeros;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);

    for (int i = 0; i < M; ++i)
        numeros.push_back(dist(gen));

    long long tiempo;
    HeapFibonacci heap = crearHeap();
    int tam = 0;
    
    auto inicio = high_resolution_clock::now();
    for (int num : numeros){
        insertar(&heap, num);
    }
    auto fin = high_resolution_clock::now();
    tiempo=duration_cast<microseconds>(fin - inicio).count();

    cout << "\nPara M=" << M 
         << "\nTiempo total= " << tiempo << " microsegundos\n"
         << "Tiempo promedio por insercion= " << (double)tiempo / M << " microsegundos.\n";
    cout << "----------------------------------\n";
    liberarHeap(&heap); // Liberar memoria del heap
}

// Experimento 2: Mezcla de operaciones
void experimento2(unsigned seed, int O) {
    vector<int> operaciones;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist_op(1, 4); // cambiar 1,4 para unión

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
    HeapFibonacci heap = crearHeap();
    int m;
    mt19937 genNum(seed);
    mt19937 gen_uni(seed);
    uniform_int_distribution<int> distNum(1, 1000000);
    uniform_int_distribution<int> distheap2(1, 100);
    generarHeap(seed, 100, &heap); //Crea un heap inicial de 100 nodos

    auto inicio = high_resolution_clock::now();
    for (int op : operaciones) {
        switch(op) {
            case 1: 
                insertar(&heap, distNum(genNum)); 
                break;
            case 2: m = obtenerMinimo(&heap); break;
            case 3: 
                if (heap.tamano>0) {  // Verificar heap vacío correctamente
                    obtenerMinimo(&heap);
                }
                break;
            case 4: {
                HeapFibonacci heap2 = crearHeap();
                generarHeap(seed, distheap2(gen_uni), &heap2);
                int prev_size = heap.tamano;
                unir(&heap, &heap2);
                liberarHeap(&heap2);  // Solo liberar heap2, no los nodos unidos
                break;
            }
        }
    }
    auto fin = high_resolution_clock::now();
    tiempo=duration_cast<microseconds>(fin - inicio).count();


    cout << "\nPara O=" << O 
         << "\nTiempo total= " << tiempo << " microsegundos\n"
         << "Tiempo promedio por operacion= " << (double)tiempo / O << " microsegundos.\n"
         << "Cantidad de inserciones=" << ins 
         << ", consulta tope=" << cons
         << ", uniones=" << uni
         << ", obtener tope=" << ext << ".\n";
    cout << "----------------------------------\n";
    liberarHeap(&heap);
    
}

// Experimento 3: Secuencia completa
void experimento3(unsigned seed, int N) {
    vector<vector<long long>> tiempos(4); // 0:ins, 1:cons, 2:ext, 3:union
    HeapFibonacci heap = crearHeap();
    mt19937 gen(seed);
    mt19937 gen_uni(seed);
    uniform_int_distribution<int> distNum(1, 1000000);
    uniform_int_distribution<int> distHeap2(1, 100);

    // Inserciones
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        insertar(&heap, distNum(gen));
    auto fin = high_resolution_clock::now();
    tiempos[0].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Consultas
    int m;
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        m = obtenerMinimo(&heap);
    fin = high_resolution_clock::now();
    tiempos[1].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Extracciones
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        if (heap.tamano > 0) obtenerMinimo(&heap);
    fin = high_resolution_clock::now();
    tiempos[2].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Uniones
    int tiempo = 0;
    for (int i = 0; i < N; ++i) {
        HeapFibonacci heap2 = crearHeap();
        generarHeap(seed + i, distHeap2(gen_uni), &heap2);
        inicio = high_resolution_clock::now();
        unir(&heap, &heap2);
        fin = high_resolution_clock::now();
        tiempo += duration_cast<microseconds>(fin - inicio).count();
    }
    
    tiempos[3].push_back(tiempo);
    liberarHeap(&heap); // Liberar memoria del heap
    

    string tipos[4] = {"Insercion", "Consulta", "Extraccion", "Union"};
    cout << "\nPara N=" << N << endl;
    for (int i = 0; i < 4; ++i) {
        auto [min, max] = minmax_element(tiempos[i].begin(), tiempos[i].end()); //Ya no es necesario pero se tiene el dato
        double prom = accumulate(tiempos[i].begin(), tiempos[i].end(), 0.0) / N;
        cout << tipos[i] << ": min=" << *min << "microseg, max=" << *max 
             << "microseg, promedio=" << prom << "microseg\n";
    }
    cout << "----------------------------------\n";
}



int main() {
    unsigned seed;
    cin >> seed;
    vector<int> valoresN;
    int n;
    while (cin >> n && n != -1) {
        valoresN.push_back(n);
    }
    if (valoresN.empty()) {
        cout << "No se ingresaron valores, usando {100, 1000, 10000, 100000, 1000000} por defecto.\n";
        valoresN = {100, 1000, 10000, 100000, 1000000};
    }

    // Experimento 1
    cout << "======Experimento 1: Inserciones." << endl;
    for (auto M : valoresN) {
        experimento1(seed, M);;
    }
    
    cout << "======Experimento 2: Operaciones." << endl;
    for (auto O : valoresN) {
        experimento2(seed, O);
    }
    cout << "======Experimento 3: Operaciones en secuencia." << endl;
    for (auto N : valoresN) {
        experimento3(seed, N);
    }
    

    return 0;
}