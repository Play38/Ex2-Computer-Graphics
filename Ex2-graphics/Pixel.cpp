// Written by: Ofir Cohen 307923615, Omer Guttman 313432213, Omri Gil 305292179
#include "Pixel.h"


Pixel::Pixel(){

}

Pixel::Pixel(int x, int y, float r, float g, float b){
    newX = x;
    newY = y;
    newR = r;
    newG = g;
    newB = b;
}

Pixel::~Pixel(){

}
int Pixel::getX(){
    return newX;
}

int Pixel::getY(){
    return newY;
}

float Pixel::getR(){
    return newR;
}

float Pixel::getG(){
    return newG;
}

float Pixel::getB(){
    return newB;
}

void Pixel::setPosition(int x, int y){
    newX = x;
    newY = y;
}

const Pixel& Pixel::operator=(const Pixel& rPixel)
{
    newX = rPixel.newX;
    newY = rPixel.newY;
    return *this;
}