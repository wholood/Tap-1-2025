#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int filas, columnas;
int salud_minima = 0;

int vida_requerida(int a) {
    return ((1-a) > 1) ? (1-a) : 1;
}
int minimo(int a, int b) {
    return (a < b) ? a : b;
}

void mover(vector<vector<int>>& mazmorra, int i, int j, int suma_actual, int minimo_actual) {
    suma_actual += mazmorra[i][j];
    minimo_actual = minimo(minimo_actual, suma_actual);

    if (i == filas - 1 && j == columnas - 1) {
        int requerida = vida_requerida(minimo_actual);
        //cout << "salud requerida: " << requerida << endl;
        if (salud_minima == 0 || requerida < salud_minima) {
            salud_minima = requerida;
        }
        return;
    }
    if (i + 1 < filas) {
        mover(mazmorra, i + 1, j, suma_actual, minimo_actual);
    }
    if (j + 1 < columnas) {
        mover(mazmorra, i, j + 1, suma_actual, minimo_actual);
    }
}

int dungeon(vector<vector<int>>& mazmorra) {
    filas = mazmorra.size();
    columnas = mazmorra[0].size();
    mover(mazmorra, 0, 0, 0, 0);
    return salud_minima;
}

int main() {
    vector<vector<int>> tablero = {{-2, -3, 3}, {-5, -10, 1}, {10, 30, -5}};
    assert(dungeon(tablero) == 7);
    return 0;
}
