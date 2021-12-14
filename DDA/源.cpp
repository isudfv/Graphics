#include <gl/glut.h>
#include <random>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <format>
using namespace std;

const int MAX = 1000;
//#define ttt 900

std::vector<std::tuple<int, int, int, int>> getRandomLines(int count) {
	std::vector<std::tuple<int, int, int, int >> temp(count);
	std::mt19937 gen(1);
	std::uniform_int_distribution<> dis(0, MAX), m(1, 20), b(0, 250);
	std::generate(temp.begin(), temp.end(), [&]() {
		return std::make_tuple(dis(gen), dis(gen), dis(gen), dis(gen));
	});
	return temp;
}

//std::vector<std::tuple<int, int, int, int>> getRandomLines(int count) {
//	std::vector<std::tuple<int, int, int, int >> temp(count);
//	std::mt19937 gen(1);
//	std::uniform_real_distribution<> dis(0, 1);
//	std::generate(temp.begin(), temp.end(), [&]() {
//		return std::make_tuple(0, 800, ttt, (800 - ttt * dis(gen)) );
//	});
//	return temp;
//}

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
	glPointSize(9);
	glColor3f(1.0, 0, 0);
	glBegin(GL_POINTS);
	for (int k = 0; k <= step; k++) {
		//glVertex2i(int(x + 0.5), int(y + 0.5));
		glVertex2i(round(x), round(y));
		x += x_inc;
		y += y_inc;
	}
	glEnd();
}

void linesBresenham(int x0, int y0, int x1, int y1) {
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2; /* error value e_xy */

	glBegin(GL_POINTS);
	for (;;) {  /* loop */
		glVertex2i(x0, y0);
		if (x0==x1 && y0==y1) break;
		e2 = 2*err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
	glEnd();
}

//void linesBresenham(int x1, int y1, int x2, int y2) {
//	int x, y, dx, dy, s1, s2, p, temp, interchange, i;
//	x = x1;
//	y = y1;
//	dx = abs(x2-x1);
//	dy = abs(y2-y1);
//
//	if (x2>x1)
//		s1 = 1;
//	else
//		s1 = -1;
//
//	if (y2>y1)
//		s2 = 1;
//	else
//		s2 = -1;
//
//	if (dy>dx) {
//		temp = dx;
//		dx = dy;
//		dy = temp;
//		interchange = 1;
//	} else
//		interchange = 0;
//
//	p = 2*dy-dx;
//	glBegin(GL_POINTS);
//	for (i = 1; i<=dx; i++) {
//		
//		//out << std::format("{} {}\n", x, y);
//		glVertex2i(x, y);
//		if (p>=0) {
//			if (interchange == 0)
//				y = y+s2;
//			else
//				x = x+s1;
//			p = p-2*dx;
//		}
//		if (interchange == 0)
//			x = x+s1;
//		else
//			y = y+s2;
//		p = p+2*dy;
//	}
//		glEnd();
//}



void display(int lines) {
	//glClearColor(1, 1, 1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, MAX, MAX, 0.0);
	glColor3f(1.0, 0, 0);
	auto rdLines = getRandomLines(lines);

	auto startTime = chrono::high_resolution_clock::now();
	for (auto &[x0, y0, x1, y1] : rdLines) {
		linesDDA(x0, y0, x1, y1);
		//cout << x0 << " " << y0 << " " << x1 << " " << y1 << endl;
		//linesBresenham(x0, y0, x1, y1);
	}	
	auto endTime = chrono::high_resolution_clock::now();
	glFlush();
	cout << format("DDA costs {}\n", chrono::duration_cast<chrono::milliseconds>(endTime - startTime));
	ofstream out1("DDA.txt", ios::app);
	out1 << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << endl;

	this_thread::sleep_for(1s);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	this_thread::sleep_for(1s);

	startTime = chrono::high_resolution_clock::now();
	for (auto& [x0, y0, x1, y1] : rdLines) {
		//linesDDA(x0, y0, x1, y1);
		//cout << x0 << " " << y0 << " " << x1 << " " << y1 << endl;
		linesBresenham(x0, y0, x1, y1);
	}
	endTime = chrono::high_resolution_clock::now();
	cout << format("Bresenham costs {}\n", chrono::duration_cast<chrono::milliseconds>(endTime - startTime));
	ofstream out("Bresenham.txt", ios::app);
	out << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << endl;
	glFlush();


	/*ifstream in1("DDA.txt");
	ifstream in2("Bresenham.txt");
	int a, b;
	std::vector<std::pair<int, int>> DDA, Bresenham;
	while (in1 >> a >> b) {
		DDA.emplace_back(a, b);
	}
	while (in2 >> a >> b) {
		Bresenham.emplace_back(a, b);
	}
	std::sort(std::begin(DDA), std::end(DDA)); 
	std::sort(std::begin(Bresenham), std::end(Bresenham));
	in1.close(), in2.close();
	ofstream out1("DDA.txt");
	ofstream out2("Bresenham.txt");
	for (auto &[x, y] : DDA) {
		out1 << x << " " << y << endl;
	}
	for (auto& [x, y] : Bresenham) {
		out2 << x << " " << y << endl;
	}*/
	/*std::cout << DDA.size() << Bresenham.size() << endl;
	cout << std::equal(std::begin(DDA), std::end(DDA), std::begin(Bresenham));*/
	//std::cout << cmp()

	//glBegin(GL_POINTS);
	//for (size_t i = 0; i < 200; i++) {
	//	glVertex2i(50, i);
	//}
	//glEnd();
}

void displayLoop() {
	//std::vector<int> loop{ 5000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000 };
	std::vector<int> loop{ 10 };
	for (auto p : loop) {
		display(p);
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(500, 300);
	glutInitWindowSize(MAX, MAX);
	glutCreateWindow("DDA_Bresenham");
	glutDisplayFunc(displayLoop);
	glutMainLoop();
	return 0;
}