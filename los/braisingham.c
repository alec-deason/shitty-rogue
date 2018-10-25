// Based on:
//
//  https://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/FOURTH_EDITION/PROGRAMS/bresenham.c
//
// Compiling via:
//
//  cc braisingham.c -lGL -lGLU -lglut -o show && ./show`
//
// Issues:
// 
//  Does not handle directions

#include <GL/glut.h>
#include <stdio.h>

void draw_pixel(int ix, int iy) {
  glBegin(GL_POINTS);
    glVertex2i(ix, iy);
  glEnd();
}

void braise(int a_x, int a_y, int b_x, int b_y) {
    // initialize starting (x,y)
    int x = a_x;
    int y = a_y;

    // calculate x- and y-distances
    int dx = abs(b_x - a_x);
    int dy = abs(b_y - a_y);

    // set signs on increments
    int x_increment = (b_x >= a_x) ? 1 : -1;
    int y_increment = (b_y >= a_y) ? 1 : -1;

    // pointers to things what get changed
    int *rise, *run;
    int *stepper, *step;
    int *bumper, *bump;

    // draw starting pixel
    draw_pixel(x,y);

    // set up which values will be modified based on
    // the slope of the line
    if (dx > dy) {
        // The slope is shallow
        rise = &dy;
        run = &dx;

        stepper = &x;
        step = &x_increment;

        bumper = &y;
        bump = &y_increment;
    } else {
        // The slope is steep (or 45 degrees)
        rise = &dx;
        run = &dy;

        stepper = &y;
        step = &y_increment;

        bumper = &x;
        bump = &x_increment;
    }

    // multiply everything by 2 to get rid of the 0.5 and
    // leave only integer math (and shifts)
    int error = (2 * *rise) - *run;

    // error adjustements
    int increment = 2 * *rise;
    int drain = 2 * *run;

    // step 'run' many steps along the stepper axis
    for (int i = 0; i < *run; i++) {
        // advance stepper
        *stepper += *step;
        // advance "error"
        error += increment;

        // bucket is full
        if (error >= 0) {
            // advance the bumper
            *bumper += *bump;
            // "reset" the "error"
            error -= drain;
        }

        draw_pixel(x,y);
    }
}

///////////////////////
// GL Rendering Code //
///////////////////////

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    braise(200, 200, 100, 50);
    braise(100, 100, 200, 50);
    braise(200, 100, 10, 150);
    braise(100, 50, 75, 75);
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
