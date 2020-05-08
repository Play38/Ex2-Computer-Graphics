
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

private:
  int newX;
  int newY;
  float newR;
  float newG;
  float newB;
};

#endif