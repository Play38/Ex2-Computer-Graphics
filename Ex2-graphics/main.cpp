#pragma warning(disable:4996)
#include "Pixel.h"
#include "Point.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>


/*#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0*/



int factorial(int n)
{
	if (n <= 1)
		return(1);
	else
		n = n * factorial(n - 1);
	return n;
}

float binomial_coff(float n, float k)
{
	float ans;
	ans = factorial(n) / (factorial(k) * factorial(n - k));
	return ans;
}

using namespace std;

float red = 0.0, green = 0.0, blue = 0.0; // initiate it to black
int tmpx, tmpy; // store the first point when shape is line, or circle
int tmpcx1, tmpcy1, tmpcx2, tmpcy2, tmpcx3, tmpcy3, tmpcx4, tmpcy4;
bool isSecond = false;
float window_w = 500;
float window_h = 500;
int objectWidth, objectHeight = 0;
Point curveArrayToPrint[100];
vector<vector<int>> vectorLines;
vector<vector<int>> vectorCircles;
vector<vector<int>> vectorCurves;

int shape = 1; // modes

vector<Pixel> pixels;		// store all pixels

void drawFromFile();
void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glBegin(GL_POINTS);

	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		glColor3f(pixels[i].getR(), pixels[i].getG(), pixels[i].getB());
		glVertex2i(pixels[i].getX(), pixels[i].getY());
	}
	glEnd();
	glutSwapBuffers();
}

void clear()
{
	pixels.clear();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void quit()
{
	std::cout << "Goodbye" << std::endl;
	exit(0);
}

void drawPixel(int mousex, int mousey)
{
	Pixel newPixel(mousex, window_h - mousey, red, green, blue);
	pixels.push_back(newPixel);
}



void myLine(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawPixel(y, x);
	else
		drawPixel(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawPixel(y, x);
		else
			drawPixel(x, y);
	}
}

/**
 * Midpoint circle algorithm
 */
void myCircle(int x0, int y0, int radius)
{
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y)
	{
		drawPixel(x0 + x, y0 + y);
		drawPixel(x0 + y, y0 + x);
		drawPixel(x0 - y, y0 + x);
		drawPixel(x0 - x, y0 + y);
		drawPixel(x0 - x, y0 - y);
		drawPixel(x0 - y, y0 - x);
		drawPixel(x0 + y, y0 - x);
		drawPixel(x0 + x, y0 - y);

		if (err <= 0)
		{
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0)
		{
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}
//Calculate the bezier point


void myCurve(vector<int> v)
{
	for (int j = 0; j <= 6; j += 2)
		curveArrayToPrint[j / 2].setxy((float)v[j], (float)v[j + 1]);

	Point p1 = curveArrayToPrint[0];
	/* Draw each segment of the curve.Make t increment in smaller amounts for a more detailed curve.*/
	for (double t = 0.0; t <= 1.0; t += 0.002)
	{
		Point p2;
		p2.x = 0; p2.y = 0;
	for (int i = 0; i < 4; i++)
	{
		p2.x = p2.x + binomial_coff((float)(4 - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (4 - 1 - i)) * curveArrayToPrint[i].x;
		p2.y = p2.y + binomial_coff((float)(4 - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (4 - 1 - i)) * curveArrayToPrint[i].y;
	}
		myLine(p1.x, p1.y, p2.x, p2.y);
		p1 = p2;
	}
	
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	isSecond = false;
	switch (key)
	{
	case 'q':
	case 27: // 27 is the esc key
		quit();
		break;
	
	}
}

void setHeightWidth()
{

	int a = 0;
	int nmax = 0;
	int nmin = 501;

	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		a = pixels[i].getX();
		nmax = max(a, nmax);
		nmin = min(a, nmin);
	}
	objectWidth = abs(nmax - nmin);
}
void horizonFlip()
{
	setHeightWidth();
	auto pixeltemp = pixels;
	int tempx, tempy = 0;
	clear();
	for (unsigned int i = 0; i < pixeltemp.size(); i++)
	{
		tempx = objectWidth - pixeltemp[i].getX() - 1;
		tempy = pixeltemp[i].getY();
		pixeltemp[i].setPosition(tempx, tempy);
	}
	pixels = pixeltemp;


}
void mouse(int bin, int state, int x, int y)
{
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
			{
				if (!isSecond)
				{
					tmpx = x;
					tmpy = y;
					isSecond = true;
				}
				else
				{
					if (shape == 1)
					{
						cout << "bop" << endl;
						horizonFlip();
						shape = 0;
					}
					else if (shape == 2)
					{

					}
					isSecond = false;
				}
				if (shape == 3)
				{
					//curveArrayToPrint[curvePoints].setxy((float)x, (float)(y));
					//curvePoints++;

					//if (curvePoints == clicks)
					{
						//Point p1 = curveArrayToPrint[0];
						/* Draw each segment of the curve.Make t increment in smaller amounts for a more detailed curve.*/
						/*for (double t = 0.0; t <= 1.0; t += 0.002)
						{
							//Point p2 = curveCalc(curveArrayToPrint, t);
							myLine(p1.x, p1.y, p2.x, p2.y);
							p1 = p2;
						}
						curvePoints = 0;*/
					}
				}
			}
		}
	
}



void reshape(int w, int h)
{
	window_w = w;
	window_h = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void processMainMenu(int value)
{
	switch (value)
	{
	case 0:
		quit();
	
	}
}



void processShapeMenu(int value)
{
	shape = value;
	isSecond = false;

	switch (shape)
	{
	case 1:
	case 2:
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}




void createOurMenu()
{

	int actionMenu = glutCreateMenu(processShapeMenu);
	glutAddMenuEntry("Flip horizon", 1);
	glutAddMenuEntry("Circle", 2);
	glutAddMenuEntry("Curve", 3);



	int main_id = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Actions", actionMenu);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(void)
{
	/* background color */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(red, green, blue);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, window_w, 0.0, window_h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FPS(int val)
{
	glutPostRedisplay();
	glutTimerFunc(0, FPS, 0);
}

void callbackInit()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutTimerFunc(17, FPS, 0);
	drawFromFile();
}

// later add errorcheck for invalid input
void read_file(char** argv)
{
	string line;
	ifstream myfile("ship.txt");
	string delimiter = ",";
	size_t pos = 0;
	string token;
	//0 - none, 1 - lines, 2- circle, 3- curves
	int mode = 0;
	int count = 0;


	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			switch (mode)
			{
				case 0:
				{
					if (!(line.find("lines")))
						mode = 1;
					else if (!(line.find("circles")))
						mode = 2;
					else if (!(line.find("curves")))
						mode = 3;
					break;
				}
				case 1:
				{
					int x1, x2, y1, y2;
					count = 0;
					if (!(line.find("#")))
					{
						mode = 0;
						break;
					}

					// remove () from the string
					line = line.substr(1, line.size() - 2);

					while ((pos = line.find(delimiter)) != string::npos) {
						token = line.substr(0, pos);
						cout << token << endl;
						if (count == 0)
							x1 = stoi(token);
						else if (count == 1)
							y1 = stoi(token);
						else if (count == 2)
							x2 = stoi(token);
						count++;
						line.erase(0, pos + delimiter.length());

					}
					token = line.substr(0, pos);
					y2 = stoi(token);
					vectorLines.insert(vectorLines.end(), { x1,y1,x2,y2 });
					break;
				}
				case 2:
				{
					int xc, yc, R;
					count = 0;
					if (!(line.find("#")))
					{
						mode = 0;
						break;
					}

					line = line.substr(1, line.size() - 2);

					while ((pos = line.find(delimiter)) != string::npos) {
						token = line.substr(0, pos);
						cout << token << endl;
						if (count == 0)
							xc = stoi(token);
						else if (count == 1)
							yc = stoi(token);
						else if (count == 2)
						{}
						count++;
						line.erase(0, pos + delimiter.length());

					}
					token = line.substr(0, pos);
					R = stoi(token);
					vectorCircles.insert(vectorCircles.end(), { xc,yc,R });
					break;
				}

				case 3:
				{
					int x1, y1, x2, y2, x3 ,y3, x4, y4;
					count = 0;

					if (!(line.find("#")))
					{
						mode = 0;
						break;
					}

					line = line.substr(1, line.size() - 2);

					while ((pos = line.find(delimiter)) != string::npos) {
						token = line.substr(0, pos);
						cout << token << endl;
						if (count == 0)
							x1 = stoi(token);
						else if (count == 1)
							y1 = stoi(token);
						else if (count == 2)
							x2 = stoi(token);
						else if (count == 3)
							y2 = stoi(token);
						else if (count == 4)
							x3 = stoi(token);
						else if (count == 5)
							y3 = stoi(token);
						else if (count == 6)
							x4 = stoi(token);
						count++;
						line.erase(0, pos + delimiter.length());

					}
					token = line.substr(0, pos);
					y4 = stoi(token);
					vectorCurves.insert(vectorCurves.end(), { x1, y1, x2, y2, x3, y3, x4, y4});
					break;
				}
			}
		}
		myfile.close();
	}

	else cout << "Unable to open file";
}

int maxElement(vector<vector<int>>& v)
{
	int a, nmax = 0;

	for (int i= 0; i < v.size(); i++)
	{
			auto it = max_element(begin(v[i]), end(v[i]));
			a = it[0];
			nmax = max(a, nmax);
	}

	return nmax;
}
void FitCordsToWindow(vector<vector<int>>& v)
{
	auto vtemp = v;
	// handle if there's a higher cord number
	while (maxElement(vtemp) > 500)
	{
		for (int i = 0; i < vtemp.size(); i++)
		{
			for (int j = 0; j < v[i].size(); j++)
			{

				vtemp[i][j] /=2;
			}
		}
	}
	while (maxElement(vtemp) < 500)
	{
		v = vtemp;
		for (int i = 0; i < vtemp.size(); i++)
		{
			for (int j = 0; j < v[i].size(); j++)
			{
				vtemp[i][j] = vtemp[i][j] * 2;
			}
		}
	}
	int yx = 5;
	//windows_h , window_w = 500
}

void centerObject()
{
	int a = 0;
	int nmax = 0;
	int nmin = 1;

	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		a = pixels[i].getX();
		nmax = max(a, nmax);
		nmin = min(a, nmin);
	}
	nmax = abs(window_w - nmax);
	nmin = abs(window_w - nmin);

	/*while (nmax != nmin)
	{

		nmax = abs(window_w - nmax);
		nmin = abs(window_w - nmin);
	}*/
}
void drawFromFile() 
{
	FitCordsToWindow(vectorLines);
	FitCordsToWindow(vectorCircles);
	FitCordsToWindow(vectorCurves);
	for (int i = 0; i < vectorLines.size(); i++)
		myLine(vectorLines[i][0], vectorLines[i][1], vectorLines[i][2], vectorLines[i][3]);

	for (int i = 0; i < vectorCircles.size(); i++)
		myCircle(vectorCircles[i][0], vectorCircles[i][1], vectorCircles[i][2]);

	for (int i = 0; i < vectorCurves.size(); i++)
		myCurve(vectorCurves[i]);

	centerObject();
}

int main(int argc, char **argv)
{
	read_file(argv);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(window_w, window_h);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Paint");
	callbackInit();
	init();
	createOurMenu();
	glutMainLoop();
	return (0);
}