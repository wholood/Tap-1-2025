#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>

using namespace std;

int filas, columnas;

int saludMinima(vector<vector<int>>& mazmorra, int i, int j, vector<vector<int>>& memo) {
    if (memo[i][j] != -1) {
        return memo[i][j]; // Devolvemos el resultado almacenado si ya lo hemos calculado
    }
    //celda de la princesa
    if (i == filas - 1 && j == columnas - 1) {
        return max(1 - mazmorra[i][j], 1); // La salud mínima requerida en la celda de la princesa
    }
    
    // Calculamos la salud mínima requerida para moverse a la derecha o abajo.

    int derecha;
    // Si no estamos en la última columna, podemos movernos a la derecha.
    if (j + 1 < columnas){
        derecha = saludMinima(mazmorra, i, j + 1, memo); // Llamada recursiva a la derecha
    }
    else{ // Si estamos en la última columna, no podemos movernos a la derecha.
        derecha = INT_MAX;
    }

    int abajo;
    // Si no estamos en la última fila, podemos movernos hacia abajo.
    if (i + 1 < filas){
        abajo = saludMinima(mazmorra, i + 1, j, memo); // Llamada recursiva hacia abajo
    }
    else{ // Si estamos en la última fila, no podemos movernos hacia abajo.
        abajo = INT_MAX;
    }
    
    
    // La salud requerida en (i,j) depende del mínimo entre derecha/abajo y el valor actual.
    // Si la salud requerida es menor que 1, la ajustamos a 1.
    int requerida = max(min(derecha, abajo) - mazmorra[i][j], 1); // La salud mínima requerida en la celda actual
    
    
    // Almacenamos el resultado en la tabla de memoización.
    memo[i][j] = requerida;
    return requerida;
}

int calcularSaludInicial(vector<vector<int>>& mazmorra) {
    filas = mazmorra.size();
    columnas = mazmorra[0].size();

    vector<vector<int>> memo(filas, vector<int>(columnas, -1));
    
    return saludMinima(mazmorra, 0, 0, memo);
}

int main() {
    vector<vector<int>> tablero = {{-2, -3, 3}, {-5, -10, 1}, {10, 30, -5}};
    int resultado = calcularSaludInicial(tablero);
    cout << "Salud min: " << resultado << endl; // Salida: 7
    return 0;
}

