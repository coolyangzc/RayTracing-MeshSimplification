#ifndef OBJECT_H
#define OBJECT_H

#include "Texture.h"
#include <fstream>

class Object
{
public:
    Texture* texture;
    Vec3f boxmin, boxmax;
    Object();
    virtual ~Object();

    virtual bool isLight() const {return false;}
    virtual bool isTriangle() const {return false;}
    virtual bool Inside(const Vec3f&) const {return false;}
    virtual void Input(std::ifstream& fin) = 0;
    virtual bool Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist, Vec3f* N = 0, Color* color = 0) = 0;

};

class Sphere: public Object
{
public:
    Vec3f O;
    double R, R2, recR;

    Sphere() {}
    Sphere(const Vec3f&, const double&);

    Vec3f RandomPoint();
    void Input(std::ifstream& fin);
    bool Inside(const Vec3f&) const;
    bool Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist, Vec3f* N = 0, Color* color = 0);
};

class Triangle: public Object
{
public:
    Vec3f v[3], vt[3], vn[3], Normal, U, V;

    Triangle() {}
    bool isSmooth_Normal() const {return !vn[0].isZero();}
    bool isTriangle() const {return true;}
    void Input(std::ifstream& fin);
    bool Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist, Vec3f* N = 0, Color* color = 0);
    void Calc();

private:
    Vec3f CalcUV(short axis);
};

class Light: public Sphere
{
public:

    bool isLight() const {return true;}
};

#endif // OBJECT_H
