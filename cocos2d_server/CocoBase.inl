
#ifndef COCOBASE_CPP
#define COCOBASE_CPP
#pragma message("Your Message Here--------------------------------------------------------------")

inline void Point::operator*=(float t){
    x*=t;
    y*=t;
}
inline Point Point::operator *(float t)const{
    Point res=*this;
    res*=t;
    return res;
}
inline Point Point::operator/(const float s) const
{
    return Point(this->x / s, this->y / s);
}

inline void Point::operator+=(const Point &p){
    x+=p.x;
    y+=p.y;
}
inline Point Point::operator+(const Point &p)const{
    Point tmp=*this;
    tmp+=p;
    return tmp;
}


inline void Point::operator-=(const Point &p){
    x-=p.x;
    y-=p.y;
}
inline Point Point::operator-(const Point &p)const{
    Point tmp=*this;
    tmp-=p;
    return tmp;
}

inline Point Point::operator-() const
{
    Point result(*this);
    result.negate();
    return result;
}

inline void Point::negate()
{
    x = -x;
    y = -y;
}


inline void Point::normalize(){
    float l=sqrt(lengthSquared());
    if(l<0.00001){
        x=1;
        y=0;
        return ;
    }
    x/=l;
    y/=l;
}

inline float Point::length(){
    return sqrt(lengthSquared());
}
inline float Point::lengthSquared(){
    return x*x+y*y;
}


inline Point Point::rotate(const Point& other) const {
    return Point(x*other.x - y*other.y, x*other.y + y*other.x);
}

inline float Point::dot(const Point& v) const
{
    return (x * v.x + y * v.y);
}

#endif


