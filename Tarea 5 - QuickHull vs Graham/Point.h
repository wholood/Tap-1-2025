#ifndef POINT_H
#define POINT_H

// Estructura para almacenar un punto con su color
struct Point {
    int x;
    int y;
    float r, g, b; // Componentes de color RGB

    // Constructor para facilitar la creación de puntos
    Point(int _x = 0, int _y = 0, float _r = 0.0f, float _g = 0.0f, float _b = 0.0f)
        : x(_x), y(_y), r(_r), g(_g), b(_b) {}

    // Sobrecarga del operador de igualdad para std::find
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

#endif // POINT_H