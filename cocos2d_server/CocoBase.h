#ifndef COCOBASE_H
#define COCOBASE_H
#include "math.h"





#define CCLOG(format, ...)      {fprintf(stdout,format, ##__VA_ARGS__);\
fflush(stdout);}

struct Point{
    float x,y;
    Point(float x=0,float y=0):x(x),y(y){}
    inline void operator*=(const float f);
    inline void operator+=(const Point &p);
    inline Point operator+(const Point &p)const;

    inline void operator-=(const Point &p);
    inline Point operator-(const Point &p)const;
    inline void negate();
    inline Point operator-() const;

    inline Point operator*(const float f)const;
    inline Point operator/(float s) const;


    inline void normalize();
    inline float length();
    inline float length_squre();


    inline Point rotate(const Point& other) const ;
    inline float dot(const Point& v) const;
};
#include "CocoBase.inl"
#endif // COCOBASE_H
