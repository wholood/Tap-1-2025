#include <GL/freeglut.h>
#include <vector>
#include "Quickhull.h" 
#include "Graham.h"
#include "Point.h"
using namespace std;

// Vector para almacenar todos los puntos dibujados por el usuario
vector<Point> user_points;

// Vector para almacenar los puntos del Casco Convexo
vector<Point> convex_hull_points;
vector<Point> graham_points;

// Variable para controlar si estamos en una nueva serie de puntos (después de un cambio de color o un nuevo dibujo)
bool new_series_active = true; // Empieza true para permitir dibujar puntos rojos al inicio

// Dimensiones de la ventana
int win_width = 800;
int win_height = 600;

// --- Funciones de FreeGLUT ---

// Función de dibujo
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Borra el búfer de color con el color de fondo

    // Dibujar los puntos del usuario
    glPointSize(10.0f); // Tamaño de los puntos
    glBegin(GL_POINTS);
    for (const auto& p : user_points) {
        glColor3f(p.r, p.g, p.b); // Color de cada punto individual
        glVertex2i(p.x, p.y);
    }
    glEnd();

    // Dibujar el casco convexo como un polígono
    if (convex_hull_points.size() > 1) {
        glColor3f(0.0f, 0.0f, 0.0f); // Color negro para el casco convexo
        glLineWidth(5.0f);           // Ancho de línea para el casco

        glBegin(GL_LINE_LOOP); // Dibuja una secuencia de líneas conectadas que se cierra al final
        for (const auto& p : convex_hull_points) {
            glVertex2i(p.x, p.y);
        }
        glEnd();
    }
    if (graham_points.size() > 1) {
        glColor3f(0.0f, 0.0f, 0.0f); // Color Gris para graham
        glLineWidth(3.0f);           // Ancho de línea para el casco

        glBegin(GL_LINE_LOOP); // Dibuja una secuencia de líneas conectadas que se cierra al final
        for (const auto& p : graham_points) {
            glVertex2i(p.x, p.y);
        }
        glEnd();
    }

    glFlush(); // Asegura que los comandos de dibujo se ejecuten inmediatamente
}

// Función de manejo de eventos del mouse
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) { // Solo nos interesan los eventos de presionar el botón
        if (button == GLUT_LEFT_BUTTON) {
            // Si es una nueva serie o no hay puntos, limpia y empieza de nuevo
            if (new_series_active) {
                user_points.clear();
                convex_hull_points.clear(); // Limpia también el casco anterior
                graham_points.clear(); // Limpia también el casco de Graham
                new_series_active = false;
            }

            // Añadir un nuevo punto rojo
            // Si quieres un límite, puedes usar user_points.size() < MAX_POINTS
            Point new_point(x, win_height - y, 1.0f, 0.0f, 0.0f); // Rojo
            user_points.push_back(new_point);
            
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (user_points.size() >= 3 ) {
                convex_hull_points = get_convex_hull(user_points);
            } else {
                convex_hull_points.clear(); // No hay hull si hay menos de 3 puntos
            }
            for (auto& p : user_points) {
                p.r = 0.0f;
                p.g = 0.0f;
                p.b = 1.0f; // Azul
            }
            new_series_active = true; // La próxima izquierda iniciará una nueva serie
            glutPostRedisplay(); // Redibuja la ventana
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (user_points.size() >= 3 ) {
                graham_points = get_convex_hull_graham(user_points);
            } else {
                graham_points.clear(); // No hay hull si hay menos de 3 puntos
            }
            for (auto& p : user_points) {
                p.r = 0.0f;
                p.g = 1.0f; // Verde
                p.b = 0.0f;
            }
            new_series_active = true; // La próxima izquierda iniciará una nueva serie
            glutPostRedisplay(); // Redibuja la ventana
        }
    }
}

// Función de manejo de eventos de redimensionamiento de ventana
void reshape(int w, int h) {
    win_width = w;
    win_height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Función principal
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("Tarea 5 - QuickHull vs Graham Scan");

    glClearColor(1.0, 1.0, 1.0, 1.0); // Fondo blanco

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}