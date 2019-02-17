#ifndef MATH_UTILS_BASE_H
#define MATH_UTILS_BASE_H
//#include "cocos2d.h"

//using namespace cocos2d;



class Base2D
{
public:
    Base2D();
    static const float PI;
    template<class Point>
    static float find_angle(const Point &p){
        Point f=p;
        f.normalize();
        float angle=acos(f.x);
        if(f.y<0)
            angle=PI*2-angle;
        return angle;
    }
    template<class Point>
    static float angle_refresh_pi_npi(float x){
        if(x<0)
          x+=int(-x/PI/2+2)*2*PI;
        x-=int(x/PI/2)*2*PI;
        if(x>PI)
            return x-2*PI;
        return x;
    }
};

#endif // COCOSUTILS_H
