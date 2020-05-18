// Written by: Ofir Cohen 307923615, Omer Guttman 313432213, Omri Gil 305292179
#ifndef PIXEL_H
#define PIXEL_H

#include <iostream>
#include <string>
using namespace std;

class Pixel
{
public:
  Pixel();
  Pixel(int, int, float, float, float);
  ~Pixel();

  int getX();
  int getY();
  float getR();
  float getG();
  float getB();

  void setPosition(int, int);
  const Pixel& operator=(const Pixel& rPixel);

private:
  int newX;
  int newY;
  int width;
  float newR;
  float newG;
  float newB;
};

#endif