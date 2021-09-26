#include <gl/glut.h>
#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <thread>
#include <chrono>
#include <format>
#include "block.h"
#include "maze.h"
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX = 1000;
const int BLOCKSIZE = 30;
const int MAZESIZE = 30;
block maze[MAZESIZE+10][MAZESIZE+10];

void linesDDA(int x0, int y0, int x1, int y1) {
	//ofstream out("DDA.txt");
	//cout << std::format("{} {} {} {}\n", x0, y0, x1, y1);
	int dx = x1 - x0, dy = y1 - y0, step;
	float x_inc, y_inc, x = x0, y = y0;
	if (abs(dx) > abs(dy))
		step = abs(dx);
	else
		step = abs(dy);
	x_inc = (float)dx/step;
	y_inc = (float)dy/step;
	//cout << x_inc << " " << y_inc << endl << step << endl;
	glPointSize(1);
	//glColor3f(0, 0, 0);
	glBegin(GL_POINTS);
	for (int k = 0; k <= step; k++) {
		glVertex2i(int(x + 0.5), int(y + 0.5));
		//glVertex2i(round(x), round(y));
		x += x_inc;
		y += y_inc;
	}
	glEnd();
}

void displayRec() {
	int startY = (MAX - MAZESIZE*BLOCKSIZE) >> 1;
	glColor3f(0, 0, 0);
	for (int i = 1; i <= MAZESIZE; i++) {
		int startX = (MAX - MAZESIZE*BLOCKSIZE) >> 1;
		for (int j = 1; j <= MAZESIZE; j++) {
			maze[i][j].x0 = startX;
			maze[i][j].y0 = startY;
			maze[i][j].x1 = startX + BLOCKSIZE;
			maze[i][j].y1 = startY + BLOCKSIZE;
			/*glBegin(GL_LINE_LOOP);
			glVertex2d(maze[i][j].x0, maze[i][j].y0);
			glVertex2d(maze[i][j].x1, maze[i][j].y0);
			glVertex2d(maze[i][j].x1, maze[i][j].y1);
			glVertex2d(maze[i][j].x0, maze[i][j].y1);			
			glEnd();*/
			glColor3f(0, 0, 0);
			linesDDA(maze[i][j].x0, maze[i][j].y0, maze[i][j].x1, maze[i][j].y0);
			linesDDA(maze[i][j].x1, maze[i][j].y0, maze[i][j].x1, maze[i][j].y1);
			linesDDA(maze[i][j].x1, maze[i][j].y1, maze[i][j].x0, maze[i][j].y1);
			linesDDA(maze[i][j].x0, maze[i][j].y1, maze[i][j].x0, maze[i][j].y0);
			//cout << format("{} {} {} {}\n", maze[i][j].x0, maze[i][j].y0, maze[i][j].x1, maze[i][j].y1);
			startX += BLOCKSIZE;
		}
		startY += BLOCKSIZE;
		//cout << endl;
	}
	glFlush();
}



void overlapLine(const block& a, const block& b) {
	glColor3f(1, 1, 1);
	auto [x0, y0, x1, y1] = a.findOverlapLine(b); 
	/*glBegin(GL_LINES);
	glVertex2i(x0, y0);
	glVertex2i(x1, y1);

	glEnd();*/
	glColor3f(1, 1, 1);
	linesDDA(x0, y0, x1, y1);
	glColor3f(0, 0, 0);
	glBegin(GL_POINTS);
	glVertex2i(x0, y0);
	glVertex2i(x0, y0);
	glVertex2i(x1, y1);
	glVertex2i(x1, y1);
	glEnd();
}

mt19937_64 b;
std::vector<std::pair<int, int>> _{ {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

void flood_fill_4(int x, int y) {
	std::shuffle(std::begin(_), std::end(_), b);
	for (auto [X, Y] : _) {
		int _x = x + X, _y = y + Y;
		if (_x <= 0 || _y <= 0 || _x > MAZESIZE || _y > MAZESIZE)
			continue;
		if (maze[_x][_y].colored == 0) {
			overlapLine(maze[x][y], maze[_x][_y]);
			maze[_x][_y].colored = 1;
			flood_fill_4(_x, _y);
		}
	}
}

void displayLoop() {
	glClearColor(1, 1, 1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, MAX, MAX, 0.0);
	displayRec();
	maze[MAZESIZE/2][MAZESIZE/2].colored = 1;
	//flood_fill_4(MAZESIZE/2, MAZESIZE/2);
	flood_fill_4(1, 2);
	glFlush();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(500, 300);
	glutInitWindowSize(MAX, MAX);
	glutCreateWindow("maze");
	glutDisplayFunc(displayLoop);
	glutMainLoop();
	return 0;
}