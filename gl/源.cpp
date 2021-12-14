#include <GL/glut.h>

void myinit() {
    // attributes
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);

    // set up viewing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 500.0, 0.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void displaySG() {
    GLfloat vertices[3][3] = { {0.0,0.0,0.0}, {500.0, 500.0,0.0}, {500.0,0.0,0.0} };
    // an arbitrary triangle in the plane z = 0;
    GLfloat p[3] = { 7.5, 5.0, 0.0 };
    // or set any desired initial point which is inside the triangle;
    int  j, k;
    int total_points = 50000;
    glBegin(GL_POINTS);
    for (k = 0; k < total_points; k++) {
        /* pick a random vertex from 0, 1, 2*/
        j = rand() % 3;
        // compute new location;
        p[0] = (p[0] + vertices[j][0]) / 2;
        p[1] = (p[1] + vertices[j][1]) / 2;
        // display new point
        glVertex3fv(p);
    }
    glEnd();
    glFlush();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Sierpinski Gasket");
    glutDisplayFunc(displaySG);
    myinit();
    glutMainLoop();
    return 0;
}
