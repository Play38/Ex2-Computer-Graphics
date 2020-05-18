// Written by: Ofir Cohen 307923615, Omer Guttman 313432213, Omri Gil 305292179
#include "Point.h"

Point::Point() {
}

Point::~Point() {
}
void Point::setxy(float x2, float y2)
{
	x = x2; y = y2;
}

const Point& Point::operator=(const Point& rPoint)
{
	x = rPoint.x;
	y = rPoint.y;
	return *this;
}
