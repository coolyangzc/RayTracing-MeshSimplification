#ifndef VEC3F_H
#define VEC3F_H

#include <iostream>

class Vec3f;

typedef Vec3f Color;

class Vec3f
{
public:
    union
    {
        struct {double x, y, z;};
        struct {double r, g, b;};
        struct {double _data[3];};
    };

    Vec3f(double x_=0, double y_=0, double z_=0):x(x_), y(y_), z(z_) {}
    ~Vec3f() {};

    double& operator [](const int&);
    const double& operator [](const int&) const;

    void operator = (const double&);
    void operator += (const Vec3f&);
    void operator -= (const Vec3f&);
    void operator *= (const double&);
    void operator /= (const double&);

    const Vec3f operator +(const double&) const;
    const Vec3f operator +(const Vec3f&) const;
    const Vec3f operator -(const double&) const;
    const Vec3f operator -(const Vec3f&) const;
    const Vec3f operator *(const double&) const;
    friend const Vec3f operator *(const double& f, const Vec3f& self) {return self * f;}
    const Vec3f operator /(const double&) const;

    const Vec3f operator -() const;

    double operator *(const Vec3f&) const;      //���

    friend std::istream& operator >>(std::istream&, Vec3f&);
    friend std::ostream& operator <<(std::ostream&, const Vec3f&);

    Vec3f mul(const Vec3f&) const;

    Vec3f Normalize();
    Vec3f Normalize() const;

    Vec3f Reflect(const Vec3f& N) const;                    //�Է�����N�ķ������������߾�Ϊ��λ����
    Vec3f Refract(const Vec3f& N, const double& n) const;   //�Է�����N������������������Ϊn
    void Confine();
    double length() const;
    double length2() const;
    double Surfacediv2() const;                         //�������һ��
    bool isZero() const;

    void Vgetmax(const Vec3f&);
    void Vgetmin(const Vec3f&);
};


Vec3f Cross(const Vec3f&, const Vec3f&);                //���
double dist(const Vec3f&, const Vec3f&);                //����
double dist2(const Vec3f&, const Vec3f&);               //����ƽ��
double det(const Vec3f&, const Vec3f&, const Vec3f&);   //����ʽ


#endif // VEC3F_H
