#include <iostream>
#include <vector>
#include <tuple>
#include <limits>
#include <chrono>
#include <random>
#include <numeric>

#define vi vector<int>
#define vf vector<float>
#define vvf vector<vf>

using namespace std;
using namespace std::chrono;

int calcular_suma_submatriz(vvf& matriz, int fila_inicio, int col_inicio, int fila_fin, int col_fin) {
    int suma = 0;
    for (int i = fila_inicio; i <= fila_fin; ++i) {
        for (int j = col_inicio; j <= col_fin; ++j) {
            suma += matriz[i][j];
        }
    }
    return suma;
}

// Encuentra la submatriz con la suma máxima usando fuerza bruta
tuple<int, pair<int, int>, pair<int, int>> suma_max(vvf& matriz) {
    int filas = matriz.size();
    int columnas = matriz[0].size();
    int suma_max = numeric_limits<int>::min();
    pair<int, int> esquina_sup_izq, esquina_inf_der;

    for (int fila_inicio = 0; fila_inicio < filas; ++fila_inicio) {
        for (int col_inicio = 0; col_inicio < columnas; ++col_inicio) {
            for (int fila_fin = fila_inicio; fila_fin < filas; ++fila_fin) {
                for (int col_fin = col_inicio; col_fin < columnas; ++col_fin) {
                    int suma_actual = calcular_suma_submatriz(matriz, fila_inicio, col_inicio, fila_fin, col_fin);
                    if (suma_actual > suma_max) {
                        suma_max = suma_actual;
                        esquina_sup_izq = {fila_inicio, col_inicio};
                        esquina_inf_der = {fila_fin, col_fin};
                    }
                }
            }
        }
    }

    return make_tuple(suma_max, esquina_sup_izq, esquina_inf_der);
}

vvf generar_matriz(int seed, int n, int iter) {
    vvf matriz(n, vf(n));
    mt19937 gen(seed);
    uniform_real_distribution<float> prob_dist(0.0f, 1.0f);
    uniform_real_distribution<float> val_dist(0.0f, 100.0f);

    float prob_positivo = (float)(iter) / 20.0f;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float valor = val_dist(gen);
            float p = prob_dist(gen);

            if (p < prob_positivo) {
                matriz[i][j] = valor; // Positivo
            } else {
                matriz[i][j] = -valor; // Negativo
            }
        }
    }
    return matriz;
}


int main() {
    vi seeds = 
        {11301229, 12291328, 13281427, 14271526, 15261625, 16251724, 17241823, 18231922, 19222021, 20212120,
        21202219, 22192318, 23182417, 24172516, 25162615, 26152714, 27142813, 28132912, 29123011, 30111130};
    
    vi valores_N = {5,10,15,20,25,30, 35, 40, 45, 50};

    for(int i = 0; i < valores_N.size(); i++) {
        int promedio= 0;
        for(int j=0; j<20; j++){
            vvf matriz = generar_matriz(seeds[j], valores_N[i], j+1);
            /*cout << "Matriz generada:" << endl;
            
            for (auto& fila : matriz) {
                for (auto& elem : fila) {
                    cout << elem << " ";
                }
                cout << endl;
            }*/

            auto inicio = high_resolution_clock::now();
            auto [suma_max_val, esquina_sup_izq, esquina_inf_der] = suma_max(matriz);
            auto fin = high_resolution_clock::now();

            /*cout << "Suma Máxima (Fuerza Bruta): " << suma_max << endl;
            cout << "Esquina Superior Izquierda: (" << esquina_sup_izq.first << ", " << esquina_sup_izq.second << ")" << endl;
            cout << "Esquina Inferior Derecha: (" << esquina_inf_der.first << ", " << esquina_inf_der.second << ")" << endl;*/
            
            promedio += duration_cast<microseconds>(fin - inicio).count();
        }
        cout << "Tiempo promedio para N=" << valores_N[i] << ": " << (float)promedio / 20 << " microsegundos." << endl;
    }

    return 0;
}
