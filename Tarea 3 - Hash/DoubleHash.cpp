#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <random>
#include <algorithm> 
#include <numeric>   
#include <string>   
#include "Nodo.h"
#define ull unsigned long long
using namespace std;
using namespace std::chrono;

// Hash principal para enteros
ull funcionHashEntero(int clave, ull M) {
    return clave % M;
}

// Segundo hash para el paso en doble hashing
ull funcionHashPaso(int clave, ull M) {
    if (M <= 2) return 1;
    return 1 + (clave % (M - 2));
}

class double_hash {
private:
    vector<Nodo*> tabla;
    ull M; 
    ull N; 

    // Primer hash
    ull obtenerHash1(int clave)  {
        return funcionHashEntero(clave, M);
    }

    // Segundo hash (paso)
    ull obtenerHash2(int clave)  {
        return funcionHashPaso(clave, M);
    }

    // Redimensiona la tabla hash y reubica los elementos activos
    void redimensionar(ull capacidad) {
        vector<Nodo*> tablaAntigua = tabla;
        ull M_antiguo = M;

        M = capacidad;
        tabla.assign(M, nullptr);
        N = 0;

        for (ull i = 0; i < M_antiguo; ++i) {
            if (tablaAntigua[i] != nullptr && tablaAntigua[i]->estaActivo) {
                insertar(tablaAntigua[i]->clave, tablaAntigua[i]->valor);
            }
            if (tablaAntigua[i] != nullptr) {
                delete tablaAntigua[i];
            }
        }
    }

public:
    double_hash(ull tamanoInicialM) : M(tamanoInicialM), N(0) {
        tabla.assign(M, nullptr);
    }

    ~double_hash() {
        for (ull i = 0; i < M; ++i) {
            if (tabla[i] != nullptr) {
                delete tabla[i];
            }
        }
    }

    // Inserta un par clave-valor usando doble hashing
    void insertar(int clave, int valor) {
        if (N >= M / 2) { 
            redimensionar(2 * M); // Redimensiona si la carga es alta
        }

        ull hashBase = obtenerHash1(clave);
        ull paso = obtenerHash2(clave);
        ull i = 0;
        ull pos = hashBase;

        // Busca posición libre o actualiza si ya existe
        while (tabla[pos] != nullptr && i < M) {
            if (tabla[pos]->estaActivo && tabla[pos]->clave == clave) {
                tabla[pos]->valor = valor;
                return;
            }
            i++;
            pos = (hashBase + i * paso) % M;
        }

        if (tabla[pos] != nullptr) {
            delete tabla[pos];
        }
        tabla[pos] = new Nodo(clave, valor);
        tabla[pos]->estaActivo = true;
        N++;
    }

    // Busca una clave y retorna puntero al valor (o nullptr)
    int* buscar(int clave)  {
        ull hashBase = obtenerHash1(clave);
        ull paso = obtenerHash2(clave);
        ull i = 0;
        ull pos = hashBase;

        // Recorre usando doble hashing hasta encontrar o agotar
        while (tabla[pos] != nullptr && i < M) {
            if (tabla[pos]->estaActivo && tabla[pos]->clave == clave) {
                return &tabla[pos]->valor;
            }
            i++;
            pos = (hashBase + i * paso) % M;
        }
        return nullptr;
    }

    // Marca como inactivo un elemento y redimensiona si es necesario
    bool eliminar(int clave) {
        ull hashBase = obtenerHash1(clave);
        ull paso = obtenerHash2(clave);
        ull i = 0;
        ull pos = hashBase;

        while (tabla[pos] != nullptr && i < M) {
            if (tabla[pos]->estaActivo && tabla[pos]->clave == clave) {
                tabla[pos]->estaActivo = false;
                N--;

                // Redimensiona si la carga es baja
                if (N > 0 && M > 4 && N <= M / 8) { 
                    redimensionar(M / 2);
                } else if (N == 0 && M > 4) {
                    redimensionar(M / 2);
                }
                return true;
            }
            i++;
            pos = (hashBase + i * paso) % M;
        }
        return false;
    }

    ull obtenerTamano()  {
        return N;
    }

    ull obtenerCapacidad()  {
        return M;
    }

    // Imprime el estado de la tabla hash
    void imprimirTabla()  {
        cout << "--- Tabla Hash (Doble Hash) ---\n";
        for (ull i = 0; i < M; ++i) {
            cout << "Slot " << i << ": ";
            if (tabla[i] != nullptr) {
                cout << "(" << tabla[i]->clave << ", " << tabla[i]->valor << ") ";
                if (!tabla[i]->estaActivo) {
                    cout << "[INACTIVO]";
                }
            } else {
                cout << "[VACÍO]";
            }
            cout << "\n";
        }
        cout << "-------------------------------\n";
    }
};

// Genera un vector de claves aleatorias
vector<int> generarClaves(int cantidad, unsigned seed) {
    vector<int> claves(cantidad);
    iota(claves.begin(), claves.end(), 0);
    shuffle(claves.begin(), claves.end(), mt19937(seed));
    return claves;
}

// Genera un número aleatorio en un rango
int generarNumeroAleatorio_main(unsigned seed, int minVal, int maxVal) {
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(gen);
}

// Ejecuta un experimento de inserción, búsqueda y eliminación, midiendo tiempos
void ejecutar_experimento(ull M_inicial, vector<int>& claves_insercion, vector<int>& claves_busqueda, vector<int>& claves_eliminacion) {
    
    double_hash ht(M_inicial);

    long long tiempo_total_insercion = 0;
    long long tiempo_total_busqueda = 0;
    long long tiempo_total_eliminacion = 0;

    int elementos_insertados = 0;
    int elementos_encontrados = 0;
    int elementos_eliminados = 0;

    // Inserciones
    auto inicio_insercion = high_resolution_clock::now();
    for (int clave : claves_insercion) {
        ht.insertar(clave, clave * 10);
        elementos_insertados++;
    }
    auto fin_insercion = high_resolution_clock::now();
    tiempo_total_insercion = duration_cast<microseconds>(fin_insercion - inicio_insercion).count();

    // Búsquedas
    auto inicio_busqueda = high_resolution_clock::now();
    for (int clave : claves_busqueda) {
        int* valor = ht.buscar(clave);
        if (valor != nullptr) {
            elementos_encontrados++;
        }
    }
    auto fin_busqueda = high_resolution_clock::now();
    tiempo_total_busqueda = duration_cast<microseconds>(fin_busqueda - inicio_busqueda).count();

    // Eliminaciones
    auto inicio_eliminacion = high_resolution_clock::now();
    for (int clave : claves_eliminacion) {
        bool eliminado = ht.eliminar(clave);
        if (eliminado) {
            elementos_eliminados++;
        }
    }
    auto fin_eliminacion = high_resolution_clock::now();
    tiempo_total_eliminacion = duration_cast<microseconds>(fin_eliminacion - inicio_eliminacion).count();

    cout << "    Inserciones (" << elementos_insertados << " elementos): " << tiempo_total_insercion << " microsegundos\n";
    cout << "    Busquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "    Eliminaciones (" << claves_eliminacion.size() << " intentos, " << elementos_eliminados << " eliminados): " << tiempo_total_eliminacion << " microsegundos\n";
}

// Experimento dominado por inserciones
void experimento1(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por inserciones (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves para insertar: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    
    vector<int> claves_insercion = claves_base;
    vector<int> claves_busqueda(claves_base.begin(), claves_base.begin() + num_claves / 10);
    vector<int> claves_eliminacion(claves_base.begin(), claves_base.begin() + num_claves / 20);

    ejecutar_experimento(M_inicial, claves_insercion, claves_busqueda, claves_eliminacion);
    cout << "------------------------------------------------------------------\n";
}

// Experimento dominado por búsquedas
void experimento2(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por busquedas (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial(claves_base.begin(), claves_base.begin() + num_claves / 2);
    vector<int> claves_busqueda = generarClaves(num_claves, seed + 1);
    
    double_hash ht(M_inicial);
    for(int clave : claves_insercion_inicial) ht.insertar(clave, clave * 10);
    
    long long tiempo_total_busqueda = 0;
    int elementos_encontrados = 0;

    // Solo mide búsquedas
    auto inicio_busqueda = high_resolution_clock::now();
    for (int clave : claves_busqueda) {
        int* valor = ht.buscar(clave); 
        if (valor != nullptr) {
            elementos_encontrados++;
        }
    }
    auto fin_busqueda = high_resolution_clock::now();
    tiempo_total_busqueda = duration_cast<microseconds>(fin_busqueda - inicio_busqueda).count();

    cout << "    Busquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "------------------------------------------------------------------\n";
}

// Experimento dominado por eliminaciones
void experimento3(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por eliminaciones (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial = claves_base;
    vector<int> claves_eliminacion = claves_base;

    double_hash ht(M_inicial);
    for(int clave : claves_insercion_inicial) ht.insertar(clave, clave * 10);

    long long tiempo_total_eliminacion = 0;
    int elementos_eliminados = 0;

    // Solo mide eliminaciones
    auto inicio_eliminacion = high_resolution_clock::now();
    for (int clave : claves_eliminacion) {
        bool eliminado = ht.eliminar(clave);
        if (eliminado) {
            elementos_eliminados++;
        }
    }
    auto fin_eliminacion = high_resolution_clock::now();
    tiempo_total_eliminacion = duration_cast<microseconds>(fin_eliminacion - inicio_eliminacion).count();

    cout << "    Eliminaciones (" << claves_eliminacion.size() << " intentos, " << elementos_eliminados << " eliminados): " << tiempo_total_eliminacion << " microsegundos\n";
    cout << "------------------------------------------------------------------\n";
}

// Experimento con operaciones aleatorias (inserción, búsqueda, eliminación)
void experimento4(unsigned seed, int num_operaciones_totales, ull M_inicial) {
    cout << "\nExperimento Uso promedio (M inicial: " << M_inicial << ") ======\n";
    cout << "Numero total de operaciones: " << num_operaciones_totales << "\n";

    vector<int> claves_disponibles = generarClaves(num_operaciones_totales * 2, seed);
    
    double_hash ht(M_inicial);
    mt19937 gen_op(seed + 2);
    uniform_int_distribution<int> dist_op(1, 3);

    long long tiempo_total = 0;
    int idx_claves_insertadas = 0;
    int count_inserciones = 0, count_busquedas = 0, count_eliminaciones = 0;

    // Mezcla operaciones aleatoriamente
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < num_operaciones_totales; ++i) {
        int op_tipo = dist_op(gen_op);
        int clave_operar;

        if (op_tipo == 1) {
            // Inserción
            if (idx_claves_insertadas < claves_disponibles.size()) {
                clave_operar = claves_disponibles[idx_claves_insertadas++];
                ht.insertar(clave_operar, clave_operar * 10);
                count_inserciones++;
            }
        } else if (op_tipo == 2) {
            // Búsqueda
            if (!claves_disponibles.empty()) {
                clave_operar = claves_disponibles[generarNumeroAleatorio_main(seed + i, 0, claves_disponibles.size() - 1)];
                ht.buscar(clave_operar);
                count_busquedas++;
            }
        } else {
            // Eliminación
             if (!claves_disponibles.empty()) {
                clave_operar = claves_disponibles[generarNumeroAleatorio_main(seed + i, 0, claves_disponibles.size() - 1)];
                bool eliminado = ht.eliminar(clave_operar);
                if (eliminado) count_eliminaciones++;
             }
        }
    }
    auto fin = high_resolution_clock::now();
    tiempo_total = duration_cast<microseconds>(fin - inicio).count();

    cout << "    Tiempo total: " << tiempo_total << " microsegundos\n";
    cout << "    Tiempo promedio por operacian: " << (double)tiempo_total / num_operaciones_totales << " microsegundos\n";
    cout << "    Desglose de operaciones: Inserciones=" << count_inserciones
              << ", Busquedas=" << count_busquedas
              << ", Eliminaciones=" << count_eliminaciones << "\n";
    cout << "------------------------------------------------------------------\n";
}

int main() {
    unsigned seed;
    cin >> seed;

    vector<int> valores_M;
    int m_val;
    // Lee valores de M hasta -1
    while (cin >> m_val && m_val != -1) {
        valores_M.push_back(m_val);
    }
    if (valores_M.empty()) {
        cout << "No se ingresaron valores de M, usando {7, 17, 29, 53} por defecto.\n";
        valores_M = {7, 17, 29, 53};
    }

    vector<int> valores_op;
    int op_val;
    // Lee valores de operaciones hasta -1
    while (cin >> op_val && op_val != -1) {
        valores_op.push_back(op_val);
    }
    if (valores_op.empty()) {
        cout << "No se ingresaron valores de Operaciones, usando {100, 1000} por defecto.\n";
        valores_op = {100, 1000};
    }

    // Ejecuta todos los experimentos para cada combinación de M y operaciones
    for (ull M_actual : valores_M) {
        cout << "====== Experimentos para M = " << M_actual << " ======\n";
        for(ull num_op : valores_op) {
            cout << "====== Operaciones = " << num_op << " ======\n"; 
            experimento1(seed, num_op, M_actual);
            experimento2(seed, num_op, M_actual);
            experimento3(seed, num_op, M_actual);
            experimento4(seed, num_op, M_actual);
        }
    } 

    return 0;
}
