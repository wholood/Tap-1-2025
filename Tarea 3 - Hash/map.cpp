#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_map>
#define ull unsigned long long
using namespace std;
using namespace std::chrono;

vector<int> generarClaves(int cantidad, unsigned seed) {
    vector<int> claves(cantidad);
    iota(claves.begin(), claves.end(), 0);
    shuffle(claves.begin(), claves.end(), mt19937(seed));
    return claves;
}

int generarNumeroAleatorio_main(unsigned seed, int minVal, int maxVal) {
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(gen);
}

void ejecutar_experimento(ull M_inicial, vector<int>& claves_insercion, vector<int>& claves_busqueda, vector<int>& claves_eliminacion) {

    unordered_map<int, int> ht;
    ht.reserve(M_inicial);

    long long tiempo_total_insercion = 0;
    long long tiempo_total_busqueda = 0;
    long long tiempo_total_eliminacion = 0;

    int elementos_insertados = 0;
    int elementos_encontrados = 0;
    int elementos_eliminados = 0;

    auto inicio_insercion = high_resolution_clock::now();
    for (int clave : claves_insercion) {
        ht[clave] = clave * 10;
        elementos_insertados++;
    }
    auto fin_insercion = high_resolution_clock::now();
    tiempo_total_insercion = duration_cast<microseconds>(fin_insercion - inicio_insercion).count();

    auto inicio_busqueda = high_resolution_clock::now();
    for (int clave : claves_busqueda) {
        auto it = ht.find(clave);
        if (it != ht.end()) {
            elementos_encontrados++;
        }
    }
    auto fin_busqueda = high_resolution_clock::now();
    tiempo_total_busqueda = duration_cast<microseconds>(fin_busqueda - inicio_busqueda).count();

    auto inicio_eliminacion = high_resolution_clock::now();
    for (int clave : claves_eliminacion) {
        size_t erased_count = ht.erase(clave);
        if (erased_count > 0) {
            elementos_eliminados++;
        }
    }
    auto fin_eliminacion = high_resolution_clock::now();
    tiempo_total_eliminacion = duration_cast<microseconds>(fin_eliminacion - inicio_eliminacion).count();

    cout << "    Inserciones (" << elementos_insertados << " elementos): " << tiempo_total_insercion << " microsegundos\n";
    cout << "    Busquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "    Eliminaciones (" << claves_eliminacion.size() << " intentos, " << elementos_eliminados << " eliminados): " << tiempo_total_eliminacion << " microsegundos\n";
}

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

void experimento2(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por busquedas (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial(claves_base.begin(), claves_base.begin() + num_claves / 2);
    vector<int> claves_busqueda = generarClaves(num_claves, seed + 1);

    unordered_map<int, int> ht;
    ht.reserve(M_inicial);
    for(int clave : claves_insercion_inicial) ht[clave] = clave * 10;

    long long tiempo_total_busqueda = 0;
    int elementos_encontrados = 0;

    auto inicio_busqueda = high_resolution_clock::now();
    for (int clave : claves_busqueda) {
        auto it = ht.find(clave);
        if (it != ht.end()) {
            elementos_encontrados++;
        }
    }
    auto fin_busqueda = high_resolution_clock::now();
    tiempo_total_busqueda = duration_cast<microseconds>(fin_busqueda - inicio_busqueda).count();

    cout << "    Busquedas (" << claves_busqueda.size() << " intentos, " << elementos_encontrados << " encontrados): " << tiempo_total_busqueda << " microsegundos\n";
    cout << "------------------------------------------------------------------\n";
}

void experimento3(unsigned seed, int num_claves, ull M_inicial) {
    cout << "\nExperimento Uso dominado por eliminaciones (M inicial: " << M_inicial << ") ======\n";
    cout << "Cantidad de claves base: " << num_claves << "\n";

    vector<int> claves_base = generarClaves(num_claves, seed);
    vector<int> claves_insercion_inicial = claves_base;
    vector<int> claves_eliminacion = claves_base;

    unordered_map<int, int> ht;
    ht.reserve(M_inicial);
    for(int clave : claves_insercion_inicial) ht[clave] = clave * 10;

    long long tiempo_total_eliminacion = 0;
    int elementos_eliminados = 0;

    auto inicio_eliminacion = high_resolution_clock::now();
    for (int clave : claves_eliminacion) {
        size_t erased_count = ht.erase(clave);
        if (erased_count > 0) {
            elementos_eliminados++;
        }
    }
    auto fin_eliminacion = high_resolution_clock::now();
    tiempo_total_eliminacion = duration_cast<microseconds>(fin_eliminacion - inicio_eliminacion).count();

    cout << "    Eliminaciones (" << claves_eliminacion.size() << " intentos, " << elementos_eliminados << " eliminados): " << tiempo_total_eliminacion << " microsegundos\n";
    cout << "------------------------------------------------------------------\n";
}

void experimento4(unsigned seed, int num_operaciones_totales, ull M_inicial) {
    cout << "\nExperimento Uso promedio (M inicial: " << M_inicial << ") ======\n";
    cout << "Numero total de operaciones: " << num_operaciones_totales << "\n";

    vector<int> claves_disponibles = generarClaves(num_operaciones_totales * 2, seed);

    unordered_map<int, int> ht;
    ht.reserve(M_inicial);
    mt19937 gen_op(seed + 2);
    uniform_int_distribution<int> dist_op(1, 3);

    long long tiempo_total = 0;
    int idx_claves_insertadas = 0;
    int count_inserciones = 0, count_busquedas = 0, count_eliminaciones = 0;

    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < num_operaciones_totales; ++i) {
        int op_tipo = dist_op(gen_op);
        int clave_operar;

        if (op_tipo == 1) {
            if (idx_claves_insertadas < claves_disponibles.size()) {
                clave_operar = claves_disponibles[idx_claves_insertadas++];
                ht[clave_operar] = clave_operar * 10;
                count_inserciones++;
            }
        } else if (op_tipo == 2) {
            if (!claves_disponibles.empty()) {
                clave_operar = claves_disponibles[generarNumeroAleatorio_main(seed + i, 0, claves_disponibles.size() - 1)];
                ht.find(clave_operar);
                count_busquedas++;
            }
        } else {
             if (!claves_disponibles.empty()) {
                clave_operar = claves_disponibles[generarNumeroAleatorio_main(seed + i, 0, claves_disponibles.size() - 1)];
                size_t erased_count = ht.erase(clave_operar);
                if (erased_count > 0) count_eliminaciones++;
             }
        }
    }
    auto fin = high_resolution_clock::now();
    tiempo_total = duration_cast<microseconds>(fin - inicio).count();

    cout << "    Tiempo total: " << tiempo_total << " microsegundos\n";
    cout << "    Tiempo promedio por operacion: " << (double)tiempo_total / num_operaciones_totales << " microsegundos\n";
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
