#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <random>
#include <tuple>
#include <numeric>
#include <cassert>
using namespace std;
using namespace std::chrono;

struct Nodo {
    int valor;
    int grado;
    Nodo* padre;
    Nodo* hijo;
    Nodo* hermano;

    Nodo(int v) : valor(v), grado(0), padre(nullptr), hijo(nullptr), hermano(nullptr) {}
};

Nodo* unirArboles(Nodo* a, Nodo* b) {
    if (a->valor > b->valor) swap(a, b);
    b->padre = a;
    b->hermano = a->hijo;
    a->hijo = b;
    a->grado++;

    assert(b->hermano != a);
    return a;
}


// Función auxiliar: Consolida los árboles en el heap
void consolidar(vector<Nodo*>& heap) {
    vector<Nodo*> grados(64, nullptr);
    for (auto nodo : heap) {
        Nodo* actual = nodo;
        while (actual) {
            Nodo* sig = actual->hermano;
            int grado = actual->grado;
            while (grados[grado]) {
                actual = unirArboles(actual, grados[grado]);
                grados[grado] = nullptr;
                grado++;
            }
            grados[grado] = actual;
            actual = sig;
        }
    }
    heap.clear();
    for (auto& arbol : grados) {
        if (arbol) {
            arbol->hermano = nullptr;
            heap.push_back(arbol);
        }
    }
}

void insertar(vector<Nodo*>& heap, int valor) {
    Nodo* nuevo = new Nodo(valor);
    heap.push_back(nuevo);
    consolidar(heap);
}

int obtenerMinimo(const vector<Nodo*>& heap) {
    if (heap.empty()) return -1;
    int min_val = numeric_limits<int>::max();
    for (auto arbol : heap) {
        if (arbol->valor < min_val) min_val = arbol->valor;
    }
    return min_val;
}

void extraerMinimo(vector<Nodo*>& heap) {
    if (heap.empty()) return;

    // Encontrar el mínimo
    auto min_it = heap.begin();
    for (auto it = heap.begin(); it != heap.end(); ++it) {
        if ((*it)->valor < (*min_it)->valor) min_it = it;
    }

    Nodo* min_nodo = *min_it;
    heap.erase(min_it);

    // Manejo seguro de hijos
    vector<Nodo*> hijos;
    Nodo* hijo = min_nodo->hijo;
    while (hijo) {
        hijos.push_back(hijo);
        hijo = hijo->hermano;
    }

    // Reiniciar relaciones de los hijos
    for (auto& h : hijos) {
        h->padre = nullptr;
        h->hermano = nullptr;
        heap.push_back(h);
    }

    consolidar(heap);
    delete min_nodo;
}

void liberarHeap(vector<Nodo*>& heap) {
    for (auto nodo : heap) {
        // Liberar recursivamente todos los nodos del árbol binomial
        vector<Nodo*> pila;
        if (nodo) pila.push_back(nodo);
        while (!pila.empty()) {
            Nodo* actual = pila.back();
            pila.pop_back();
            Nodo* hijo = actual->hijo;
            while (hijo) {
                Nodo* siguiente = hijo->hermano;
                pila.push_back(hijo);
                hijo = siguiente;
            }
            delete actual;
        }
    }
    heap.clear();
}

void unir(vector<Nodo*>& heap1, vector<Nodo*>& heap2) {
    vector<Nodo*> merged;
    auto it1 = heap1.begin();
    auto it2 = heap2.begin();

    // Paso 1: Merge inicial ordenado por grado
    while (it1 != heap1.end() && it2 != heap2.end()) {
        if ((*it1)->grado <= (*it2)->grado) {
            merged.push_back(*it1);
            ++it1;
        } else {
            merged.push_back(*it2);
            ++it2;
        }
    }
    while (it1 != heap1.end()) merged.push_back(*it1++);
    while (it2 != heap2.end()) merged.push_back(*it2++);

    // Paso 2: Consolidación 
    auto Na = merged.begin();
    auto NaPrev = merged.end(); // Inicialmente no hay anterior
    
    while (Na != merged.end()) {
        auto Nb = next(Na);
        if (Nb == merged.end()) break;

        // Caso 1: Grado(Na) < Grado(Nb)
        if ((*Na)->grado < (*Nb)->grado) {
            if (NaPrev != merged.end() && (*NaPrev)->grado == (*Na)->grado) {
                *Na = unirArboles(*NaPrev, *Na);
                Na = merged.erase(NaPrev);
                NaPrev = (Na == merged.begin()) ? merged.end() : prev(Na);
            } else {
                NaPrev = Na;
                ++Na;
            }
        }
        // Caso 2: Grado(Na) > Grado(Nb)
        else if ((*Na)->grado > (*Nb)->grado) {
            if (NaPrev != merged.end() && (*NaPrev)->grado == (*Nb)->grado) {
                *NaPrev = unirArboles(*NaPrev, *Nb);
                merged.erase(Nb);
                Na = NaPrev;
            } else {
                *Na = *Nb;
                merged.erase(Nb);
            }
        }
        // Caso 3: Grados iguales
        else {
            *Na = unirArboles(*Na, *Nb);
            merged.erase(Nb);
            NaPrev = Na;
            ++Na;
        }
    }

    
    heap1 = merged;
    heap2.clear();
}


void generarheap(unsigned seed, int N, vector<Nodo *>& heap) {
    vector<int> numeros;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);
    
    int tam = 0;
    for (int i = 0; i < N; ++i)
        numeros.push_back(dist(gen));
    for (int num : numeros)
        insertar(heap, num);
    
}



// Experimento 1: Rendimiento de inserciones
void experimento1(unsigned seed, int M) {
    vector<int> numeros;
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);

    for (int i = 0; i < M; ++i)
        numeros.push_back(dist(gen));

    long long tiempo;
    vector<Nodo*> heap;
    int tam = 0;
    
    auto inicio = high_resolution_clock::now();
    for (int num : numeros){
        insertar(heap, num);
    }
    auto fin = high_resolution_clock::now();
    tiempo=duration_cast<microseconds>(fin - inicio).count();

    cout << "\nPara M=" << M 
         << "\nTiempo total= " << tiempo << " microsegundos\n"
         << "Tiempo promedio por insercion= " << (double)tiempo / M << " microsegundos.\n";
    cout << "----------------------------------\n";
    liberarHeap(heap); // Liberar memoria del heap
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
    vector<Nodo*> heap;
    int tam = heap.size(), m;
    mt19937 gen_num(seed);
    mt19937 gen_uni(seed);
    uniform_int_distribution<int> dist_num(1, 1000000);
    uniform_int_distribution<int> distheap2(1, 100);
    generarheap(seed, 100, heap); //Crea un heap inicial de 100 nodos

    auto inicio = high_resolution_clock::now();
    for (int op : operaciones) {
        switch(op) {
            case 1: 
                insertar(heap, dist_num(gen_num)); 
                tam++;  // Actualizar tam en inserción
                break;
            case 2: m = obtenerMinimo(heap); break;
            case 3: 
                if (!heap.empty()) {  // Verificar heap vacío correctamente
                    extraerMinimo(heap);
                    tam--;  // Actualizar tam en extracción
                }
                break;
            case 4: {
                vector<Nodo*> heap2;
                generarheap(seed, distheap2(gen_uni), heap2);
                int prev_size = heap.size();
                unir(heap, heap2);
                tam = heap.size();  // Actualizar tam después de unión
                liberarHeap(heap2);  // Solo liberar heap2, no los nodos unidos
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
    liberarHeap(heap);
    
}

// Experimento 3: Secuencia completa
void experimento3(unsigned seed, int N) {
    vector<vector<long long>> tiempos(4); // 0:ins, 1:cons, 2:ext, 3:union
    vector<Nodo*> heap;
    int tam = 0;
    mt19937 gen(seed);
    mt19937 gen_uni(seed);
    uniform_int_distribution<int> dist_num(1, 1000000);
    uniform_int_distribution<int> distHeap2(1, 100);

    // Inserciones
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        insertar(heap, dist_num(gen));
    auto fin = high_resolution_clock::now();
    tiempos[0].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Consultas
    int m;
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        m = obtenerMinimo(heap);
    fin = high_resolution_clock::now();
    tiempos[1].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Extracciones
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
        if (tam > 0) extraerMinimo(heap);
    fin = high_resolution_clock::now();
    tiempos[2].push_back(duration_cast<microseconds>(fin - inicio).count());

    // Uniones
    inicio = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        vector<Nodo*> heap2;
        generarheap(seed + i, distHeap2(gen_uni), heap2);
        unir(heap, heap2);
    }
    fin = high_resolution_clock::now();
    tiempos[3].push_back(duration_cast<microseconds>(fin - inicio).count());
    liberarHeap(heap); // Liberar memoria del heap
    

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