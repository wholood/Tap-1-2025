#include "Quickhull.h" // Incluye el archivo de encabezado que acabamos de crear

// No necesitamos incluir <vector>, <algorithm>, <cmath> aquí de nuevo si QuickHull.h ya lo hace.
// Pero si una función específica usa algo extra, podrías incluirlo.

// --- Implementación de Funciones Auxiliares para QuickHull ---

double point_line_distance(Point p, Point A, Point B) {
    return (double)(B.x - A.x) * (p.y - A.y) - (double)(B.y - A.y) * (p.x - A.x);
}

Point find_farthest_point(const std::vector<Point>& points, Point A, Point B, double* max_dist_ptr) {
    Point farthest_point = A;
    double max_distance = -1.0;

    for (const auto& p : points) {
        double dist = std::abs(point_line_distance(p, A, B)); // Distancia absoluta
        if (dist > max_distance) {
            max_distance = dist;
            farthest_point = p;
        }
    }
    if (max_dist_ptr) {
        *max_dist_ptr = max_distance; // Pasa la distancia máxima por referencia si se necesita
    }
    return farthest_point;
}

// quick_hull_recursive es una función interna de la implementación, no está declarada en el .h
// Por eso, se define aquí (y se marca como 'static' si solo se usara dentro de este archivo,
// aunque no es estrictamente necesario en C++ si no se declara en el .h)
void quick_hull_recursive(std::vector<Point>& hull, const std::vector<Point>& points, Point A, Point B, double side) {
    if (points.empty()) {
        return;
    }

    double max_dist = 0.0;
    Point farthest_point = find_farthest_point(points, A, B, &max_dist);

    if (max_dist <= 0.001) {
        bool b_found = false;
        for(const auto& p : hull) {
            if (p == B) {
                b_found = true;
                break;
            }
        }
        if (!b_found) {
            auto it = std::find(hull.begin(), hull.end(), A);
            if (it != hull.end()) {
                hull.insert(++it, B);
            }
        }
        return;
    }

    auto it_A = std::find(hull.begin(), hull.end(), A);
    if (it_A != hull.end()) {
        hull.insert(++it_A, farthest_point);
    } else {
        hull.push_back(farthest_point);
    }

    std::vector<Point> left_set_AB;
    std::vector<Point> left_set_farthestB;

    for (const auto& p : points) {
        if (point_line_distance(p, A, farthest_point) * side > 0 &&
            point_line_distance(p, farthest_point, B) * side > 0 &&
            point_line_distance(p, A, B) * side < 0 )
        {
            continue;
        }

        if (point_line_distance(p, A, farthest_point) * side > 0) {
            left_set_AB.push_back(p);
        }
        if (point_line_distance(p, farthest_point, B) * side > 0) {
            left_set_farthestB.push_back(p);
        }
    }

    quick_hull_recursive(hull, left_set_AB, A, farthest_point, side);
    quick_hull_recursive(hull, left_set_farthestB, farthest_point, B, side);
}

// --- Implementación de la Función Principal para QuickHull ---

std::vector<Point> get_convex_hull(const std::vector<Point>& points) {
    std::vector<Point> hull_result;

    if (points.size() < 3) {
        if (points.size() == 2) {
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

    auto min_x_it = std::min_element(points.begin(), points.end(),
        [](const Point& a, const Point& b) { return a.x < b.x; });
    Point P1 = *min_x_it;

    auto max_x_it = std::max_element(points.begin(), points.end(),
        [](const Point& a, const Point& b) { return a.x < b.x; });
    Point P2 = *max_x_it;

    hull_result.push_back(P1);

    std::vector<Point> upper_set;
    std::vector<Point> lower_set;

    for (const auto& p : points) {
        if (p == P1 || p == P2) continue;

        double dist_to_line = point_line_distance(p, P1, P2);
        if (dist_to_line > 0) {
            upper_set.push_back(p);
        } else if (dist_to_line < 0) {
            lower_set.push_back(p);
        }
    }

    quick_hull_recursive(hull_result, upper_set, P1, P2, 1);
    quick_hull_recursive(hull_result, lower_set, P2, P1, 1);

    // Asegurarse de que P2 esté en la hull_result si no fue insertado por la recursión
    auto it_P1 = std::find(hull_result.begin(), hull_result.end(), P1);
    auto it_P2 = std::find(hull_result.begin(), hull_result.end(), P2);

    if (it_P1 == hull_result.end()) hull_result.insert(hull_result.begin(), P1);
    if (it_P2 == hull_result.end()) hull_result.push_back(P2);

    // Ordenar los puntos del casco convexo de forma circular (ej. por ángulo respecto al centroide)
    if (hull_result.size() > 2) {
        // Calcular el centroide
        double cx = 0.0, cy = 0.0;
        for (const auto& p : hull_result) {
            cx += p.x;
            cy += p.y;
        }
        cx /= hull_result.size();
        cy /= hull_result.size();

        // Ordenar por ángulo respecto al centroide
        std::sort(hull_result.begin(), hull_result.end(), [cx, cy](const Point& a, const Point& b) {
            double angle_a = std::atan2(a.y - cy, a.x - cx);
            double angle_b = std::atan2(b.y - cy, b.x - cx);
            return angle_a < angle_b;
        });
    }

    return hull_result;
}