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

vf comprimir_filas(vvf& matriz, int fila_inicio, int fila_fin) {
    int columnas = matriz[0].size();
    vf comprimido(columnas, 0);
    for (int fila = fila_inicio; fila <= fila_fin; ++fila) {
        for (int col = 0; col < columnas; ++col) {
            comprimido[col] += matriz[fila][col];
        }
    }
    return comprimido;
}

tuple<int, int, int> kadane_1d(vf& arreglo) {
    int suma_max = numeric_limits<int>::min();
    int suma_actual = 0;
    int inicio = 0, fin = 0, temp_inicio = 0;

    for (int i = 0; i < arreglo.size(); ++i) {
        suma_actual += arreglo[i];
        if (suma_actual > suma_max) {
            suma_max = suma_actual;
            inicio = temp_inicio;
            fin = i;
        }
        if (suma_actual < 0) {
            suma_actual = 0;
            temp_inicio = i + 1;
        }
    }

    return make_tuple(suma_max, inicio, fin);
}

tuple<int, pair<int, int>, pair<int, int>> submatriz_suma_maxima(vvf& matriz_entrada) {
    vvf matriz = matriz_entrada;

    int filas = matriz.size();
    int columnas = matriz[0].size();
    int suma_max = numeric_limits<int>::min();
    int fila_sup = 0, fila_inf = 0, col_izq = 0, col_der = 0;

    for (int sup = 0; sup < filas; ++sup) {
        for (int inf = sup; inf < filas; ++inf) {
            vf comprimido = comprimir_filas(matriz, sup, inf);
            auto [suma_actual, izq, der] = kadane_1d(comprimido);

            if (suma_actual > suma_max) {
                suma_max = suma_actual;
                fila_sup = sup;
                fila_inf = inf;
                col_izq = izq;
                col_der = der;
            }
        }
    }

    return make_tuple(suma_max, make_pair(fila_sup, col_izq), make_pair(fila_inf, col_der));
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
    
    vi valores_N = {5,10,15,20,25,30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};

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
            auto [suma_max, esquina_sup_izq, esquina_inf_der] = submatriz_suma_maxima(matriz);
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
