#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <random>
#include <algorithm> 
#include <numeric>   
#include <string>   
#include "Nodo.h" // Incluimos la estructura Nodo
#define ull unsigned long long
using namespace std;
using namespace std::chrono;

ull funcionHashEntero(int clave, ull M) {
    return clave % M;
}

class separate_chaining {
private:
    vector<list<Nodo>> tabla;
    ull M; // Tamaño de la tabla
    ull N; // Nmero de elementos actuales

    // Obtener el indice hash
    ull obtenerHash(int clave) {
        return funcionHashEntero(clave, M);
    }

public:
    separate_chaining(ull tamanoInicialM) : M(tamanoInicialM), N(0) {
        tabla.resize(M);
    }

    void insertar(int clave, int valor) {
        ull indice = obtenerHash(clave);
        // Buscar si la clave ya existe para actualizar el valor
        for (auto& nodo : tabla[indice]) {
            if (nodo.clave == clave) {
                nodo.valor = valor;
                return;
            }
        }
        // Si no existe, añadir un nuevo nodo al final de la lista
        tabla[indice].emplace_back(clave, valor);
        N++;
    }

    int* buscar(int clave) {
        ull indice = obtenerHash(clave);
        for (auto& nodo : tabla[indice]) {
            if (nodo.clave == clave) {
                return &nodo.valor; // Retorna un puntero al valor encontrado
            }
        }
        return nullptr; // Clave no encontrada
    }

    bool eliminar(int clave) {
        ull indice = obtenerHash(clave);
        for (auto it = tabla[indice].begin(); it != tabla[indice].end(); ++it) {
            if (it->clave == clave) {
                tabla[indice].erase(it); // Eliminar el nodo de la lista
                N--;
                return true; // Eliminado con éxito
            }
        }
        return false; // Clave no encontrada
    }

    ull obtenerTamano() {
        return N;
    }

    ull obtenerCapacidad() {
        return M;
    }

    void imprimirTabla() {
        cout << "--- Tabla Hash (Encadenamiento Separado) ---\n";
        for (ull i = 0; i < M; ++i) {
            cout << "Slot " << i << ": ";
            for (auto& nodo : tabla[i]) {
                cout << "(" << nodo.clave << ", " << nodo.valor << ") ";
            }
            cout << "\n";
        }
        cout << "-------------------------------------------\n";
    }
};

// --- Funciones auxiliares para generar datos (duplicadas para cada main) ---

// Genera un vector de claves enteras nicas y aleatorias
vector<int> generarClaves(int cantidad, unsigned seed) {
    vector<int> claves(cantidad);
    iota(claves.begin(), claves.end(), 0);
    shuffle(claves.begin(), claves.end(), mt19937(seed));
    return claves;
}

// Genera un nmero entero aleatorio dentro de un rango
int generarNumeroAleatorio_main(unsigned seed, int minVal, int maxVal) {
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(gen);
}

void ejecutar_experimento(ull M_inicial, vector<int>& claves_insercion, vector<int>& claves_busqueda, vector<int>& claves_eliminacion) {
    
    separate_chaining ht(M_inicial);

    long long tiempo_total_insercion = 0;
    long long tiempo_total_busqueda = 0;
    long long tiempo_total_eliminacion = 0;

    int elementos_insertados = 0;
    int elementos_encontrados = 0;
    int elementos_eliminados = 0;

    // --- Inserciones ---
    auto inicio_insercion = high_resolution_clock::now();
    for (int clave : claves_insercion) {
        ht.insertar(clave, clave * 10);
        elementos_insertados++;
    }
    auto fin_insercion = high_resolution_clock::now();
    tiempo_total_insercion = duration_cast<microseconds>(fin_insercion - inicio_insercion).count();

    // --- Bsquedas ---
    auto inicio_busqueda = high_resolution_clock::now();
    for (int clave : claves_busqueda) {
        int* valor = ht.buscar(clave);
        if (valor != nullptr) {
            elementos_encontrados++;
        }
    }
    auto fin_busqueda = high_resolution_clock::now();
    tiempo_total_busqueda = duration_cast<microseconds>(fin_busqueda - inicio_busqueda).count();

    // --- Eliminaciones ---
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
    cout << "    Bsquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "    Eliminaciones (" << claves_eliminacion.size() << " intentos, " << elementos_eliminados << " eliminados): " << tiempo_total_eliminacion << " microsegundos\n";
}


// Experimento: Uso dominado por inserciones
void experimento1(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento: Uso dominado por inserciones (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves para insertar: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    
    vector<int> claves_insercion = claves_base;
    vector<int> claves_busqueda(claves_base.begin(), claves_base.begin() + num_claves / 10);
    vector<int> claves_eliminacion(claves_base.begin(), claves_base.begin() + num_claves / 20);

    ejecutar_experimento(M_inicial, claves_insercion, claves_busqueda, claves_eliminacion);
    cout << "------------------------------------------------------------------\n";
}

// Experimento: Uso dominado por bsquedas
void experimento2(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento: Uso dominado por bsquedas (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial(claves_base.begin(), claves_base.begin() + num_claves / 2);
    vector<int> claves_busqueda = generarClaves(num_claves, seed + 1);
    
    // Para este experimento, primero debemos insertar las claves iniciales
    // para que haya algo que buscar.
    separate_chaining ht(M_inicial);
    for(int clave : claves_insercion_inicial) ht.insertar(clave, clave * 10);
    
    long long tiempo_total_busqueda = 0;
    int elementos_encontrados = 0;

    auto inicio_busqueda = high_resolution_clock::now();
    for (int clave : claves_busqueda) {
        int* valor = ht.buscar(clave);
        if (valor != nullptr) {
            elementos_encontrados++;
        }
    }
    auto fin_busqueda = high_resolution_clock::now();
    tiempo_total_busqueda = duration_cast<microseconds>(fin_busqueda - inicio_busqueda).count();

    cout << "    Bsquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "------------------------------------------------------------------\n";
}

// Experimento: Uso dominado por eliminaciones
void experimento3(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento: Uso dominado por eliminaciones (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial = claves_base;
    vector<int> claves_eliminacion = claves_base;

    // Primero insertamos, luego medimos solo las eliminaciones
    separate_chaining ht(M_inicial);
    for(int clave : claves_insercion_inicial) ht.insertar(clave, clave * 10);

    long long tiempo_total_eliminacion = 0;
    int elementos_eliminados = 0;

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

// Experimento: Uso promedio (mezcla balanceada de operaciones)
void experimento4(unsigned seed, int num_operaciones_totales, ull M_inicial) {
    cout << "\nExperimento: Uso promedio (M inicial: " << M_inicial << ") ======\n";
    cout << "Nmero total de operaciones: " << num_operaciones_totales << "\n";

    vector<int> claves_disponibles = generarClaves(num_operaciones_totales * 2, seed);
    
    separate_chaining ht(M_inicial);
    mt19937 gen_op(seed + 2);
    uniform_int_distribution<int> dist_op(1, 3); // 1: insertar, 2: buscar, 3: eliminar

    long long tiempo_total = 0;
    int idx_claves_insertadas = 0;
    int count_inserciones = 0, count_busquedas = 0, count_eliminaciones = 0;

    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < num_operaciones_totales; ++i) {
        int op_tipo = dist_op(gen_op);
        int clave_operar;

        if (op_tipo == 1) { // Insertar
            if (idx_claves_insertadas < claves_disponibles.size()) {
                clave_operar = claves_disponibles[idx_claves_insertadas++];
                ht.insertar(clave_operar, clave_operar * 10);
                count_inserciones++;
            }
        } else if (op_tipo == 2) { // Buscar
            if (!claves_disponibles.empty()) {
                clave_operar = claves_disponibles[generarNumeroAleatorio_main(seed + i, 0, claves_disponibles.size() - 1)];
                ht.buscar(clave_operar);
                count_busquedas++;
            }
        } else { // Eliminar (op_tipo == 3)
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
    cout << "    Tiempo promedio por operacion: " << (double)tiempo_total / num_operaciones_totales << " microsegundos\n";
    cout << "    Desglose de operaciones: Inserciones=" << count_inserciones
              << ", Bsquedas=" << count_busquedas
              << ", Eliminaciones=" << count_eliminaciones << "\n";
    cout << "------------------------------------------------------------------\n";
}


// --- Funcion principal del programa ---
int main() {
    unsigned seed;
    cin >> seed;

    vector<int> valores_M;
    int m_val;
    while (cin >> m_val && m_val != -1) {
        valores_M.push_back(m_val);
    }
    if (valores_M.empty()) {
        cout << "No se ingresaron valores de M, usando {7, 17, 29, 53} por defecto.\n";
        valores_M = {7, 17, 29, 53};
    }

    
    vector<int> valores_op;
    int op_val;
    while (cin >> op_val && op_val != -1) {
        valores_op.push_back(op_val);
    }
    if (valores_op.empty()) {
        cout << "No se ingresaron valores de Operaciones, usando {100, 1000} por defecto.\n";
        valores_op = {100, 1000};
    }

    // Ejecutar los experimentos para cada valor de M
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