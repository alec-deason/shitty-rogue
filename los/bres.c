// Stole from here:
//  https://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/FOURTH_EDITION/PROGRAMS/bresenham.c

#define BLACK 0
#include <GL/glut.h>
#include <stdio.h>

void draw_pixel(int ix, int iy) {
  glBegin(GL_POINTS);
    glVertex2i(ix, iy);
  glEnd();
}

void bres(int a_x, int a_y, int b_x, int b_y) {
    int error;
    int x_increment, y_increment, increment_and_drain, increment;

    // initialize starting (x,y)
    int x = a_x;
    int y = a_y;

    // calculate x- and y-distances
    int dx = abs(b_x - a_x);
    int dy = abs(b_y - a_y);

    // set signs on increments
    x_increment = (b_x >= a_x) ? 1 : -1;
    y_increment = (b_y >= a_y) ? 1 : -1;

    if (dx > dy) {
        // shallow slope
        draw_pixel(x,y);

        // multiply everything by 2 to get rid of the 0.5 and
        // leave only integer math (and shifts)
        error = 2 * dy - dx;

        // two increment options
        increment = 2 * dy;
        increment_and_drain = increment - (2 * dx);

        // take i steps, where i is the distance between a and b
        for (int i = 0; i < dx; i++) {
            if (error >= 0) {
                y += y_increment;
                error += increment_and_drain;
            } else {
                error += increment;
            }

            // x takes a step
            x += x_increment;
            draw_pixel(x,y);
        }
    } else {
        // 45 degree or steep slope
        draw_pixel(x,y);
        error = 2 * dx - dy;
        increment = 2 * dx;
        increment_and_drain = increment - (2 * dy);

        for (int i = 0; i < dy; i++) {
            if (error >= 0) {
                x += x_increment;
                error += increment_and_drain;
            } else {
                error += increment;
            }

            y += y_increment;
            draw_pixel(x,y);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    bres(200, 200, 100, 50);
    bres(100, 100, 200, 50);
    bres(200, 100, 10, 150);
    bres(100, 50, 75, 75);
    glFlush();
}

void myinit() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 499.0, 0.0, 499.0);
}

void main(int argc, char** argv) {
/* standard GLUT initialization */

    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); /* default, not needed */
    glutInitWindowSize(500,500); /* 500x500 pixel window */
    glutInitWindowPosition(0,0); /* place window top left on display */
    glutCreateWindow("Bresenham's Algorithm"); /* window title */
    glutDisplayFunc(display); /* display callback invoked when window opened */

    myinit(); /* set attributes */

    glutMainLoop(); /* enter event loop */
}


