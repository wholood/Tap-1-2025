#include "Graham.h" // Incluye tu archivo de encabezado

// --- Funciones Auxiliares ---

// Función auxiliar para determinar la orientación de tres puntos (p, q, r)
// Retorna:
// 0 --> Colineales
// 1 --> Sentido horario (CW - clockwise)
// 2 --> Sentido antihorario (CCW - counter-clockwise)
// Utiliza el producto cruzado 2D.
int orientation(Point p, Point q, Point r) {
    long long val = (long long)(q.y - p.y) * (r.x - q.x) -
                    (long long)(q.x - p.x) * (r.y - r.y); // Corregido r.y - q.y -> r.y - r.y, pero r.y - q.y es correcto

    // La línea original de la fórmula de orientación (q.x - p.x) * (r.y - q.y)
    // El error estaba en (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - r.y);
    // Donde era (r.y - q.y) en el segundo término. La formula es (y2-y1)*(x3-x2) - (x2-x1)*(y3-y2)
    // Aplicado a (p, q, r):
    // p = (x1, y1), q = (x2, y2), r = (x3, y3)
    // val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y); // Esta es la forma correcta.

    val = (long long)(q.y - p.y) * (r.x - q.x) - (long long)(q.x - p.x) * (r.y - q.y); // Correcto

    if (val == 0) return 0;  // Colineales
    return (val > 0) ? 1 : 2; // 1 para CW (giro a la derecha), 2 para CCW (giro a la izquierda)
}

// Función auxiliar para calcular la distancia al cuadrado entre dos puntos
long long distSq(Point p1, Point p2) {
    return (long long)(p1.x - p2.x) * (p1.x - p2.x) +
           (long long)(p1.y - p2.y) * (p1.y - p2.y);
}

// Punto pivote global para el ordenamiento (p0)
Point p0;

// Función de comparación para std::sort (ordenamiento por ángulo)
bool comparePoints(Point p1, Point p2) {
    int o = orientation(p0, p1, p2);
    if (o == 0) { // Colineales
        // Si son colineales, el punto más cercano a p0 va primero
        return distSq(p0, p1) < distSq(p0, p2);
    }
    // Si no son colineales, queremos CCW para un orden ascendente (o == 2)
    return (o == 2);
}

// Función principal para obtener el casco convexo usando Graham Scan
std::vector<Point> get_convex_hull_graham(const std::vector<Point>& points) {
    std::vector<Point> hull_result;

    if (points.size() < 3) {
        if (points.size() == 2) {
            // Si hay dos puntos, la línea que los conecta es el hull. Ordenar por X.
            if (points[0].x < points[1].x) {
                hull_result.push_back(points[0]);
                hull_result.push_back(points[1]);
            } else {
                hull_result.push_back(points[1]);
                hull_result.push_back(points[0]);
            }
        } else if (points.size() == 1) {
             hull_result.push_back(points[0]);
        }
        return hull_result;
    }

    // 1. Encontrar el punto con menor Y (p0)
    int min_y = points[0].y;
    int min_idx = 0;
    for (size_t i = 1; i < points.size(); ++i) {
        int y = points[i].y;
        if ((y < min_y) || (y == min_y && points[i].x < points[min_idx].x)) {
            min_y = y;
            min_idx = i;
        }
    }

    std::vector<Point> sorted_points = points;
    std::swap(sorted_points[0], sorted_points[min_idx]);
    p0 = sorted_points[0]; // Asignar el punto pivote global

    // 2. Ordenar los puntos restantes por ángulo con respecto a p0
    std::sort(sorted_points.begin() + 1, sorted_points.end(), comparePoints);

    // 3. Eliminar puntos colineales redundantes que no sean los más alejados
    // Necesario para un Graham Scan robusto con puntos colineales.
    // Recorremos los puntos ordenados y si encontramos 3 o más puntos colineales,
    // mantenemos solo los dos extremos (p0 y el más alejado en esa dirección)
    int m = 1; // sorted_points[0] (p0) siempre se mantiene.
    for (size_t i = 1; i < sorted_points.size(); i++) {
        // Mientras i+1 no se salga del rango y los puntos (p0, sorted_points[i], sorted_points[i+1]) sean colineales
        // y el punto intermedio (sorted_points[i]) no sea el p0.
        while (i + 1 < sorted_points.size() && orientation(p0, sorted_points[i], sorted_points[i+1]) == 0) {
            i++; // Saltar puntos colineales internos, avanzando a i+1
        }
        sorted_points[m] = sorted_points[i]; // El punto actual (que no es colineal intermedio o es el último colineal) se mantiene
        m++;
    }
    sorted_points.resize(m); // Ajustar el tamaño del vector a los puntos únicos que vamos a procesar

    // Si después de la eliminación de colineales quedan menos de 3 puntos, el hull es trivial.
    // Esto es importante para evitar que el algoritmo de la pila falle con menos de 3 elementos.
    if (sorted_points.size() < 3) {
        return sorted_points;
    }

    // 4. Construir el casco convexo usando un vector como pila
    std::vector<Point> stack_vec;
    stack_vec.push_back(sorted_points[0]); // p0
    stack_vec.push_back(sorted_points[1]); // p1

    for (size_t i = 2; i < sorted_points.size(); i++) {
        // Mientras el tamaño de la "pila" sea al menos 2
        // Y el giro de los dos últimos puntos de la pila y el punto actual sea horario (CW) o colineal
        while (stack_vec.size() >= 2 && orientation(stack_vec[stack_vec.size()-2], stack_vec.back(), sorted_points[i]) != 2) {
            stack_vec.pop_back(); // Desapilar el último punto (porque forma un giro a la derecha o es colineal)
        }
        stack_vec.push_back(sorted_points[i]); // Apilar el punto actual
    }

    hull_result = stack_vec; // Los puntos restantes en stack_vec forman el casco convexo

    return hull_result;
}