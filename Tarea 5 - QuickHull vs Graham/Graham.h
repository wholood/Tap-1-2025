#ifndef GRAHAM_H
#define GRAHAM_H

#include <vector>
#include <algorithm> // Para std::sort, std::min_element
#include <cmath>     // Para std::atan2, std::abs
#include "Point.h"

// Declaración de la función principal de Graham Scan
std::vector<Point> get_convex_hull_graham(const std::vector<Point>& points);

#endif // GRAHAMSCAN_H