// Written by: Ofir Cohen 307923615, Omer Guttman 313432213, Omri Gil 305292179

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

// variables declarations //
float red = 0.0, green = 0.0, blue = 0.0; // initiate it to black
int tmpx, tmpy; // store the first point when shape is line, or circle
float window_w = 500;
float window_h = 500;
int shape = 1; // modes
int movex = 0;
int movey = 0;
int objectWidth, objectHeight = 0;
int zoomStop = 3; // was made to prevent cord data loss for zooming out passed the original cords
int rotateCount = 0; //0 default, 1 rotate 1 time to right, 2 rotate 2 
bool moved = false;
bool horizontal = false;
bool vertical = false;
bool first = true;
bool isSecond = false;
// variables declarations //

// vectors declarations //
Point curveArrayToPrint[100];
vector<vector<int>> vectorLines;
vector<vector<int>> vectorCircles;
vector<vector<int>> vectorCurves;
vector<vector<int>> ogvectorLines;
vector<vector<int>> ogvectorCircles;
vector<vector<int>> ogvectorCurves;
vector<vector<int>> movestack;
vector<Pixel> pixels;		// store all pixels
vector<Pixel> centeredPixels;

// vectors declarations //
void drawObject(int mode, int zoom);
void findObCenter(vector<Pixel>&p,int& centerX, int& centerY);
void centerObject(vector<Pixel> &p);
void move(int x, int y, int tmpx, int tmpy, int change);
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

void clear() // clears only the screen
{
	pixels.clear();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void clearAll() //wipe out all the data
{
	pixels.clear();
	centeredPixels.clear();
	vectorLines.clear();
	vectorCircles.clear();
	vectorCurves.clear();
	movestack.clear();
	horizontal = false;
	vertical = false;
	moved = false;
	isSecond = false;
	shape = 1;
	zoomStop = 3;
	objectHeight = 0;
	objectWidth = 0;
	rotateCount = 0;
	for (int i = 0; i < 100; i++)
	{
		curveArrayToPrint[i].setxy(0, 0);
	}
	
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


void horizonFlip(int c) //takes the centered object, flips it, and then restore to it's previous position
{
	int centerX, centerY;
	if (horizontal)
		horizontal = false;
	else
		horizontal = true;
	auto ptemp= pixels;
	clear();
	if (moved)
		ptemp = centeredPixels;
	for (unsigned int i = 0; i < ptemp.size(); i++)
	{
		ptemp[i].setPosition(window_w - ptemp[i].getX() , ptemp[i].getY());
	}
	pixels = ptemp;
	if (moved)
	{
		centeredPixels = ptemp;
		if(c)
			move(0, 0, 0, 0, 1);
	}
	


}
void verticalFlip(int c) //takes the centered object, flips it, and then restore to it's previous position
{
	if (vertical)
		vertical = false;
	else
		vertical = true;
	auto ptemp = pixels;
	clear();
	if (moved)
		ptemp = centeredPixels;
	for (unsigned int i = 0; i < ptemp.size(); i++)
	{
		ptemp[i].setPosition(ptemp[i].getX(), window_h - ptemp[i].getY());
	}
	pixels = ptemp;
	if (moved)
	{
		centeredPixels = ptemp;
		if (c)
			move(0, 0, 0, 0, 1); 
	}


}


void rotateRL(int right, int left, int c) // rotates the object 90 degrees left or right, according to the input it recives
{
	auto ptemp = pixels;
	clear();
	if (moved)
		ptemp = centeredPixels;
	for (unsigned int i = 0; i < ptemp.size(); i++)
	{
		if(right)
			ptemp[i].setPosition(ptemp[i].getY(), -ptemp[i].getX() + window_w); // rotate clockwise
		else if(left)
			ptemp[i].setPosition(-ptemp[i].getY() + window_h, ptemp[i].getX()); // rotate clockwise
	}	
	pixels = ptemp;

	if (moved)
	{
		centeredPixels = ptemp;
		if (c)
			move(0, 0, 0, 0, 1);
	}
}

void move(int x, int y, int tmpx, int tmpy, int change) // takes the object and moves it across the screen, if I moved it servel times
{                                                      //it stores the previous positions, and tracing the position from the center to the current
	moved = true;
	if (x | y | tmpx | tmpy)
	{
		movex = x - tmpx;
		movey = tmpy - y;
		movestack.insert(movestack.end(), { movex,movey });
	}
	auto ptemp = pixels;
	clear();
	if (change)
	{
		for (unsigned int i = 0; i < movestack.size(); i++)
		{
			for (unsigned int j = 0; j < ptemp.size(); j++)
			{
				ptemp[j].setPosition(ptemp[j].getX() + movestack[i][0], ptemp[j].getY() + movestack[i][1]);
			}
		}
	}
	else
	{
		for (unsigned int j = 0; j < ptemp.size(); j++)
		{
			ptemp[j].setPosition(ptemp[j].getX() + movex, ptemp[j].getY() + movey);
		}
	}

	pixels = ptemp;
}

void zoomInOut(vector<vector<int>>& v, int mode)
{
	auto vtemp = v;
	if (mode) // if 1 zoomout, 0 zoomin
	{
		for (int i = 0; i < vtemp.size(); i++)
		{
			for (int j = 0; j < vtemp[i].size(); j++)
			{

				vtemp[i][j] /= 2;
			}
		}
	}
	else
	{
		for (int i = 0; i < vtemp.size(); i++)
		{
			for (int j = 0; j < vtemp[i].size(); j++)
			{

				vtemp[i][j] *= 2;
			}
		}
	}

	v = vtemp;
}

void reset()
{
	clearAll();
	vectorLines = ogvectorLines;
	vectorCircles = ogvectorCircles;
	vectorCurves = ogvectorCurves;
	drawObject(0, 0);
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
					if (shape == 6)
					{
						move(x, y, tmpx, tmpy, 0);
					}
					isSecond = false;

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

void processFlipMenu(int value)
{
	shape = value;
		switch (shape)
		{
		case 0:
			horizonFlip(1);
			break;
		case 1:
			verticalFlip(1);
			break;
		}
}

void processZoomMenu(int value)
{

		switch (value)
		{
		case 2: // zoom in
			drawObject(1, 0);
			zoomStop++;
			break;
		case 3: //zoom out
			if (zoomStop != 0)
			{
				drawObject(1, 1); // made it stop after number of zoomouts, as if the object is zoomed out from it's original cords, there's data loss
				zoomStop--;
			}
			break;
		}
}

void processRotateMenu(int value)
{

		switch (value)
		{
		case 4: // rotateRight
			rotateCount = (rotateCount + 1) % 4;
			rotateRL(1, 0, 1);
			break;
		case 5: // rotateLeft
			rotateCount = (rotateCount - 1) % 4;
			if (rotateCount == -1)
				rotateCount = 3;
			rotateRL(0, 1, 1);
			break;
		}
}

void processMainMenu(int value)
{
	switch (value)
	{
		
	case 6:
		shape = value;
		isSecond = false;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;

	case 7:
		clearAll();
		break;
	case 8:
		reset();
		break;
	case 9:
		quit();
		break;
	}
	

}


void createOurMenu()
{

	int flipMenu = glutCreateMenu(processFlipMenu);
	glutAddMenuEntry("Horizon Flip", 0);
	glutAddMenuEntry("Vertical Flip", 1);


	int zoomMenu = glutCreateMenu(processZoomMenu);
	glutAddMenuEntry("Zoom In", 2);
	glutAddMenuEntry("Zoom Out", 3);

	int rotateMenu = glutCreateMenu(processRotateMenu);
	glutAddMenuEntry("Rotate Right", 4);
	glutAddMenuEntry("Rotate Left", 5);

	int main_id = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Flips", flipMenu);
	glutAddSubMenu("Zoom", zoomMenu);
	glutAddSubMenu("Rotates", rotateMenu);
	glutAddMenuEntry("Move", 6);
	glutAddMenuEntry("Clear", 7);
	glutAddMenuEntry("Reset", 8);
	glutAddMenuEntry("Quit", 9);
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
	drawObject(0, 0); // here is where drawObject is called for the first time
}

void read_file(char** argv)
{

	char str[256];
	string line;
	string delimiter = ",";
	size_t pos = 0;
	string token;
	//0 - none, 1 - lines, 2- circle, 3- curves
	int mode = 0;
	int count = 0;
	bool done = false;
	bool error = false;
	bool linedone = false;
	bool circledone = false;
	bool curvedone = false;
	while (!done)
	{
		error = false;
		mode = 0;
		vectorLines.clear();
		vectorCircles.clear();
		vectorCurves.clear();
		cout << "Please enter file name (Ex: ship.txt)" << endl;
		cin.get(str, 256);
		cin.ignore();
		ifstream myfile(str);

		if (myfile.is_open())
		{
			try
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
						else if (!(linedone && circledone && curvedone))
						throw invalid_argument("received negative value");
						break;
					}
					case 1:
					{
						linedone = true;
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
							if ((x1 | y1 | x2 | y2) < 0)
								throw invalid_argument("received negative value");
							vectorLines.insert(vectorLines.end(), { x1,y1,x2,y2 });
							break;
						
					}
					case 2:
					{
						circledone = true;
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
							if (count == 0)
								xc = stoi(token);
							else if (count == 1)
								yc = stoi(token);
							else if (count == 2)
							{
							}
							count++;
							line.erase(0, pos + delimiter.length());

						}
						token = line.substr(0, pos);
						R = stoi(token);
						if ((xc | yc | R) < 0)
							throw invalid_argument("received negative value");
						vectorCircles.insert(vectorCircles.end(), { xc,yc,R });
						break;
					}

					case 3:
					{
						curvedone = true;
						int x1, y1, x2, y2, x3, y3, x4, y4;
						count = 0;

						if (!(line.find("#")))
						{
							mode = 0;
							break;
						}

						line = line.substr(1, line.size() - 2);

						while ((pos = line.find(delimiter)) != string::npos) {
							token = line.substr(0, pos);
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
						if ((x1 | y1 | x2 | y2 | x3 | y3 | x4 | y4) < 0)
							throw invalid_argument("received negative value");
						vectorCurves.insert(vectorCurves.end(), { x1, y1, x2, y2, x3, y3, x4, y4 });
						break;
					}
					}
				}
			}
			catch (...)
			{
				myfile.close();
				error = true;
				cout << "Invalid input. Please fix the text file and try again!\n";
			}
			if (!error)
			{
				myfile.close();
				done = true;
			}
		}

		else cout << "Unable to open file, please try again";
	}
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
void FitCordsToWindow(vector<vector<int>>& v) //fitted it for half of the screen and not all of it as to showcase zooming in is working
{
	auto vtemp = v;
	// handle if there's a higher cord number
	while (maxElement(vtemp) > 250)
	{
		for (int i = 0; i < vtemp.size(); i++)
		{
			for (int j = 0; j < vtemp[i].size(); j++)
			{

				vtemp[i][j] /=2;
			}
		}
	}
	v = vtemp;
	while (maxElement(vtemp) < 250)
	{
		v = vtemp; //there's a reason why it's here
		for (int i = 0; i < vtemp.size(); i++)
		{
			for (int j = 0; j < vtemp[i].size(); j++)
			{
				vtemp[i][j] = vtemp[i][j] * 2;
			}
		}
	}
	//windows_h , window_w = 500
}
void findObCenter(vector<Pixel> &p,int &centerX, int &centerY)
{
	for (unsigned int i = 0; i < p.size(); i++)
	{
		centerX += p[i].getX();
		centerY += p[i].getY();
	}
}
void centerObject(vector<Pixel> &p)
{
	int centerX = 0;
	int centerY = 0;
	findObCenter(p,centerX, centerY);
	centerX /= p.size();
	centerY /= p.size();
	////// CHECK where to center
	if (centerX > (window_w/2))
		centerX = centerX - window_w / 2;
	else
		centerX = window_w/2 - centerX;

	if (centerY > (window_h / 2))
		centerY = centerY - window_w / 2;
	else
		centerY = window_w / 2 - centerY;
	////
	for (unsigned int i = 0; i < p.size(); i++)
	{
		p[i].setPosition(p[i].getX() + centerX, p[i].getY() - centerY);
	}
}
void drawObject(int mode, int zoom)  //if mode is 1, it means we need to zoom in/out, else it means we just draw the object to the fitted window
{
	clear();
		if (mode)
		{
			zoomInOut(vectorLines, zoom);
			zoomInOut(vectorCircles, zoom);
			zoomInOut(vectorCurves, zoom);
		}
		else
		{
			FitCordsToWindow(vectorLines);
			FitCordsToWindow(vectorCircles);
			FitCordsToWindow(vectorCurves);
		}
		if (first) //only occurs on first time of loading the boject
		{
			ogvectorLines = vectorLines;
			ogvectorCircles = vectorCircles;
			ogvectorCurves = vectorCurves;
			first = false;
		}
		for (int i = 0; i < vectorLines.size(); i++)
			myLine(vectorLines[i][0], vectorLines[i][1], vectorLines[i][2], vectorLines[i][3]);

		for (int i = 0; i < vectorCircles.size(); i++)
			myCircle(vectorCircles[i][0], vectorCircles[i][1], vectorCircles[i][2]);

		for (int i = 0; i < vectorCurves.size(); i++)
			myCurve(vectorCurves[i]);

		auto ptemp = pixels;
		centerObject(ptemp);
		pixels = ptemp;
		centeredPixels = ptemp;

		//after drawing it restores previous attrabiutes of the object if it had any
		if (horizontal)
		{
			horizonFlip(0);
			horizontal = true;
		}
		if (vertical)
		{
			verticalFlip(0);
			vertical = true;
		}
		for (int i = 0; i < rotateCount; i++)
			rotateRL(1, 0, 0);
		if (moved)
			move(0, 0, 0, 0, 1);
	
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