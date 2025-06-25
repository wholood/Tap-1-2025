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

class lineal_probing {
private:
    vector<Nodo*> tabla; // Almacenamos punteros a Nodos
    ull M; // Tamaño de la tabla
    ull N; // Numero de elementos activos

    // Ayudante para obtener el indice hash inicial
    ull obtenerHash(int clave) {
        return funcionHashEntero(clave, M);
    }

    // Redimensionar la tabla (crear una nueva tabla mas grande y reinsertar elementos)
    void redimensionar(ull capacidad) {
        vector<Nodo*> tablaAntigua = tabla;
        ull M_antiguo = M;

        M = capacidad;
        tabla.assign(M, nullptr); // Redimensionar e inicializar con nullptr
        N = 0; // Reiniciar el contador de elementos activos para la reinsercian

        for (ull i = 0; i < M_antiguo; ++i) {
            if (tablaAntigua[i] != nullptr && tablaAntigua[i]->estaActivo) {
                // Solo reinsertar nodos activos
                insertar(tablaAntigua[i]->clave, tablaAntigua[i]->valor);
                delete tablaAntigua[i]; // Liberar la memoria del nodo antiguo
            } else if (tablaAntigua[i] != nullptr) {
                // Liberar la memoria de los nodos inactivos/eliminados
                delete tablaAntigua[i];
            }
        }
    }

public:
    lineal_probing(ull tamanoInicialM) : M(tamanoInicialM), N(0) {
        tabla.assign(M, nullptr); // Inicializar todos los punteros a nullptr
    }

    // Destructor para liberar la memoria de los Nodos
    ~lineal_probing() {
        for (ull i = 0; i < M; ++i) {
            if (tabla[i] != nullptr) {
                delete tabla[i];
            }
        }
    }

    void insertar(int clave, int valor) {
        // Redimensionar si la tabla esta medio llena
        if (N >= M / 2) {
            redimensionar(2 * M);
        }

        ull i = obtenerHash(clave);
        // Sondear linealmente hasta encontrar un slot vacio o inactivo, o la clave
        while (tabla[i] != nullptr && tabla[i]->estaActivo) {
            if (tabla[i]->clave == clave) { // Clave ya existe, actualizar valor
                tabla[i]->valor = valor;
                return;
            }
            i = (i + 1) % M; // Siguiente slot
        }
        // Slot encontrado (vacio o inactivo)
        if (tabla[i] != nullptr) { // Si era un slot inactivo, liberar el nodo antiguo
            delete tabla[i];
        }
        tabla[i] = new Nodo(clave, valor); // Crear y asignar nuevo nodo
        N++;
    }

 int* buscar(int clave) {
        ull hashInicial = obtenerHash(clave);
        ull i = hashInicial;

        while (tabla[i] != nullptr) { // Mientras no encontremos un slot vacio
            if (tabla[i]->estaActivo && tabla[i]->clave == clave) {
                return &tabla[i]->valor; // Encontrado y activo
            }
            i = (i + 1) % M; // Siguiente slot
            if (i == hashInicial) break; // Evitar ciclo infinito si la tabla esta llena
        }
        return nullptr; // Clave no encontrada
    }

    bool eliminar(int clave) {
        ull hashInicial = obtenerHash(clave);
        ull i = hashInicial;

        while (tabla[i] != nullptr) {
            if (tabla[i]->estaActivo && tabla[i]->clave == clave) {
                tabla[i]->estaActivo = false; // Marcar como inactivo
                N--;

                // Re-hashear los elementos subsiguientes en el cluster
                ull j = (i + 1) % M;
                while (tabla[j] != nullptr) {
                    if (!tabla[j]->estaActivo) { // Saltar nodos ya eliminados
                        j = (j + 1) % M;
                        if (j == hashInicial) break;
                        continue;
                    }
                    Nodo* nodoAReubicar = tabla[j];
                    tabla[j] = nullptr; // Temporalmente quitar el nodo
                    N--; // Decrementar N ya que se incrementara en insertar
                    insertar(nodoAReubicar->clave, nodoAReubicar->valor); // Re-insertar
                    delete nodoAReubicar; // Liberar el nodo movido
                    j = (j + 1) % M;
                    if (j == hashInicial) break;
                }

                // Redimensionar si la tabla esta muy vacia
                if (N > 0 && N == M / 8) {
                    redimensionar(M / 2);
                }
                return true;
            }
            i = (i + 1) % M;
            if (i == hashInicial) break;
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
        cout << "--- Tabla Hash (Sondeo Lineal) ---\n";
        for (ull i = 0; i < M; ++i) {
            cout << "Slot " << i << ": ";
            if (tabla[i] != nullptr) {
                cout << "(" << tabla[i]->clave << ", " << tabla[i]->valor << ") ";
                if (!tabla[i]->estaActivo) {
                    cout << "[INACTIVO]";
                }
            } else {
                cout << "[VACIO]";
            }
            cout << "\n";
        }
        cout << "-------------------------------------\n";
    }
};


// --- Funciones auxiliares para generar datos (duplicadas para cada main) ---

// Genera un vector de claves enteras unicas y aleatorias
vector<int> generarClaves(int cantidad, unsigned seed) {
    vector<int> claves(cantidad);
    iota(claves.begin(), claves.end(), 0);
    shuffle(claves.begin(), claves.end(), mt19937(seed));
    return claves;
}

// Genera un numero entero aleatorio dentro de un rango
int generarNumeroAleatorio_main(unsigned seed, int minVal, int maxVal) {
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(gen);
}

void ejecutar_experimento(ull M_inicial, vector<int>& claves_insercion, vector<int>& claves_busqueda, vector<int>& claves_eliminacion) {
    
    lineal_probing ht(M_inicial);

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

    // --- Busquedas ---
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
    cout << "    Busquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "    Eliminaciones (" << claves_eliminacion.size() << " intentos, " << elementos_eliminados << " eliminados): " << tiempo_total_eliminacion << " microsegundos\n";
}


// Experimento: Uso dominado por inserciones
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

// Experimento: Uso dominado por busquedas
void experimento2(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por busquedas (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial(claves_base.begin(), claves_base.begin() + num_claves / 2);
    vector<int> claves_busqueda = generarClaves(num_claves, seed + 1);
    
    // Para este experimento, primero debemos insertar las claves iniciales
    // para que haya algo que buscar.
    lineal_probing ht(M_inicial);
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

    cout << "    Busquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "------------------------------------------------------------------\n";
}

// Experimento: Uso dominado por eliminaciones
void experimento3(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por eliminaciones (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial = claves_base;
    vector<int> claves_eliminacion = claves_base;

    // Primero insertamos, luego medimos solo las eliminaciones
    lineal_probing ht(M_inicial);
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
    cout << "\nExperimento Uso promedio (M inicial: " << M_inicial << ") ======\n";
    cout << "Numero total de operaciones: " << num_operaciones_totales << "\n";

    vector<int> claves_disponibles = generarClaves(num_operaciones_totales * 2, seed);
    
    lineal_probing ht(M_inicial);
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
    cout << "    Tiempo promedio por operacian: " << (double)tiempo_total / num_operaciones_totales << " microsegundos\n";
    cout << "    Desglose de operaciones: Inserciones=" << count_inserciones
              << ", Busquedas=" << count_busquedas
              << ", Eliminaciones=" << count_eliminaciones << "\n";
    cout << "------------------------------------------------------------------\n";
}


// --- Funcian principal del programa ---
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
