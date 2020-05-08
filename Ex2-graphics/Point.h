#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <string>

using namespace std;

class Point
{
public:
	float x, y;
	Point();
	~Point();
	void setxy(float x2, float y2);
	//operator overloading for '=' sign
	const Point& operator=(const Point& rPoint);


};


#endif