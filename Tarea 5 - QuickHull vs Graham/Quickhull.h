#ifndef QUICQHULL_H
#define QUICQHULL_H

#include <vector>
#include <algorithm> 
#include <cmath>     
#include "Point.h"

double point_line_distance(Point p, Point A, Point B);
Point find_farthest_point(const std::vector<Point>& points, Point A, Point B, double* max_dist_ptr);

std::vector<Point> get_convex_hull(const std::vector<Point>& points);

#endif // QUICQHULL_H