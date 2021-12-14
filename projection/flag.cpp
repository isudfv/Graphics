#include <gl/glut.h>
#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <thread>
#include <chrono>
#include <format>
#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;

const int MAX = 1000;
const int FLAGX = 300;
const int FLAGY = 200;
//const int startx = "MAX - FLAGX"


//void displayLoop() {
//	glClearColor(1, 1, 1, 1.0);
//	glClear(GL_COLOR_BUFFER_BIT);
//	//glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	//gluOrtho2D(0.0, MAX, MAX, 0.0);
//
//	//glTranslatef(0.2f, 0.0f, 0.0f);
//	glRotatef(20, 0, 0, 1.0f);
//	glColor3f(1, 0, 0);
//	glBegin(GL_QUADS);
//		glVertex3f(-0.51f, -0.51f, 0.0f);
//		glVertex3f(0.51f, -0.51f, 0.0f);   
//		glVertex3f(0.51f, 0.51f, 0.0f);
//		glVertex3f(-0.51f, 0.51f, 0.0f);
//	glEnd();
//	glFlush();
//}

void displayLoop() {
	double wx = glutGet(GLUT_WINDOW_WIDTH);
	double wy = glutGet(GLUT_WINDOW_HEIGHT);
	double flagx_ul = wx * 2 > wy *3 ? -wy * 3 / 2 / wx : -1.0;
	double flagy_ul = wx * 2 > wy *3 ? 1.0              : wx * 2 / 3 / wy;
	double flagx_lr = wx * 2 > wy *3 ? wy * 3 / 2 / wx  : 1.0;
	double flagy_lr = wx * 2 > wy *3 ? -1.0             : -wx * 2 / 3 / wy;

	glClearColor(1, 1, 1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(0.0, wx, wy, 0.0);
	glColor3ub(244, 0, 2);
	glBegin(GL_QUADS);
	glVertex3d(flagx_ul, flagy_ul, 0);
	glVertex3d(flagx_lr, flagy_ul, 0);
	glVertex3d(flagx_lr, flagy_lr, 0);
	glVertex3d(flagx_ul, flagy_lr, 0);
	glEnd();
	glFlush();


	auto Sin = [](int angel) { return sin(angel * acos(-1) / 180); };
	auto Cos = [](int angel) { return cos(angel * acos(-1) / 180); };

	GLdouble a[4][2] = {{0.0, 0.3 * flagy_ul},
						{0.2 * Sin(36) * Sin(18) / Sin(126) * flagx_lr, 0.3 * Sin(18) * Cos(36) / Sin(126) *flagy_ul},
						{0, 0},
						{-0.2 * Sin(36) * Sin(18) / Sin(126) * flagx_lr, 0.3 * Sin(18) * Cos(36) / Sin(126) *flagy_ul}};

	auto drawStar = [&]() {
		glBegin(GL_QUADS);
		for (int i = 0; i < 360; i += 72) {
			for (auto& [x, y] : a) {
				auto _x = x, _y = y;
				x = (_x * wx * Cos(72) - _y * wy * Sin(72)) / wx;
				y = (_x * wx * Sin(72) + _y * wy * Cos(72)) / wy;
			}
			glVertex2d(a[0][0], a[0][1]);
			glVertex2d(a[1][0], a[1][1]);
			glVertex2d(a[2][0], a[2][1]);
			glVertex2d(a[3][0], a[3][1]);
		}
		glEnd();
	};

	glColor3ub(250, 244, 8);
	glLoadIdentity();
	//glRotated(90, 0, 0, 1.0);
	glTranslated(-2.0/3*flagx_lr, 0.5*flagy_ul, 0);
	drawStar();
	glFlush();

	glLoadIdentity();
	glTranslated(-1.0/3*flagx_lr, 0.8*flagy_ul, 0);
	glScaled(1.0/3, 1.0/3, 1.0/3);
	glRotated(90+atan(0.6)*180/acos(-1), 0, 0, 1.0);
	//glRotated(90, 0, 0, 1.0);
	drawStar();
	glFlush();

	glLoadIdentity();
	glTranslated(-0.2*flagx_lr, 0.6*flagy_ul, 0);
	glScaled(1.0/3, 1.0/3, 1.0/3);
	glRotated(90+atan(1.0/7)*180/acos(-1), 0, 0, 1.0);
	//glRotated(90, 0, 0, 1.0);
	drawStar();
	glFlush();

	glLoadIdentity();
	glTranslated(-0.2*flagx_lr, 0.3*flagy_ul, 0);
	glScaled(1.0/3, 1.0/3, 1.0/3);
	glRotated(90-atan(2.0/7)*180/acos(-1), 0, 0, 1.0);
	//glRotated(90, 0, 0, 1.0);
	drawStar();
	glFlush();

	glLoadIdentity();
	glTranslated(-1.0/3*flagx_lr, 0.1*flagy_ul, 0);
	glScaled(1.0/3, 1.0/3, 1.0/3);
	glRotated(90-atan(0.8)*180/acos(-1), 0, 0, 1.0);
	//glRotated(90, 0, 0, 1.0);
	drawStar();
	glFlush();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(500, 0);
	glutInitWindowSize(MAX, MAX);
	glutCreateWindow("flag");
	glutDisplayFunc(displayLoop);
	glutMainLoop();
	return 0;
}


