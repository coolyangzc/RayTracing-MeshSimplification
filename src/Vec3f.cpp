#include "Vec3f.h"
#include "common.h"
#include <cmath>

double& Vec3f::operator [](const int& x)
{
    return _data[x];
}

const double& Vec3f::operator [](const int& x) const
{
    return _data[x];
}

void Vec3f::operator = (const double& f)
{
    x = y = z = f;
}

void Vec3f::operator += (const Vec3f& v)
{
    x+=v.x; y+=v.y; z+=v.z;
}

void Vec3f::operator -= (const Vec3f& v)
{
    x-=v.x; y-=v.y; z-=v.z;
}

void Vec3f::operator *= (const double& f)
{
    x*=f; y*=f; z*=f;
}

void Vec3f::operator /= (const double& f)
{
    x/=f; y/=f; z/=f;
}

const Vec3f Vec3f::operator +(const double& f) const
{
    return Vec3f(x+f, y+f, z+f);
}

const Vec3f Vec3f::operator +(const Vec3f& v) const
{
    return Vec3f(x+v.x, y+v.y, z+v.z);
}

const Vec3f Vec3f::operator -(const double& f) const
{
    return Vec3f(x-f, y-f, z-f);
}

const Vec3f Vec3f::operator -(const Vec3f& v) const
{
    return Vec3f(x-v.x, y-v.y, z-v.z);
}

double Vec3f::operator *(const Vec3f& v) const
{
    return x*v.x + y*v.y + z*v.z;
}

const Vec3f Vec3f::operator *(const double& f) const
{
    return Vec3f(x*f, y*f, z*f);
}

const Vec3f Vec3f::operator /(const double& f) const
{
    return Vec3f(x/f, y/f, z/f);
}

const Vec3f Vec3f::operator -() const
{
    return Vec3f(-x, -y, -z);
}
std::istream& operator >>(std::istream& in, Vec3f& v)
{
    in >> v.x >> v.y >> v.z;
    return in;
}
std::ostream& operator <<(std::ostream& out, const Vec3f& v)
{
    out << v.x << ',' << v.y << ',' << v.z;
    return out;
}

Vec3f Vec3f::mul(const Vec3f& v) const
{
    return Vec3f(x*v.x, y*v.y, z*v.z);
}

Vec3f Vec3f::Normalize()
{
    double len = sqrt(x*x + y*y + z*z);
    return Vec3f(x/=len, y/=len, z/=len);
}

Vec3f Vec3f::Normalize() const
{
    double len = sqrt(x*x + y*y + z*z);
    return Vec3f(x/len, y/len, z/len);
}

Vec3f Vec3f::Reflect(const Vec3f& N) const
{
    return *this - 2 * (*this * N) * N;
}

Vec3f Vec3f::Refract(const Vec3f& N, const double& n) const
{
    double cosI = -(*this * N);
    double cosT2 = 1.0 - sqr(n) * (1.0-sqr(cosI));
    if (cosT2 > -eps)
        return (*this * n + N * (n * cosI - sqrt(cosT2))).Normalize();
    else return Vec3f(0,0,0);
}

void Vec3f::Confine()
{
    x = getmin(1, x);
    y = getmin(1, y);
    z = getmin(1, z);
}

double Vec3f::length() const
{
    return sqrt(x*x + y*y + z*z);
}

double Vec3f::length2() const
{
    return x*x + y*y + z*z;
}

double Vec3f::Surfacediv2() const
{
    return fabs(x*y) + fabs(y*z) + fabs(z*x);
}

bool Vec3f::isZero() const
{
    return (x<eps && x>-eps && y<eps && y>-eps && z<eps && z>-eps);
}

void Vec3f::Vgetmax(const Vec3f& v)
{
    x = getmax(x, v.x); y = getmax(y, v.y); z = getmax(z, v.z);
}

void Vec3f::Vgetmin(const Vec3f& v)
{
    x = getmin(x, v.x); y = getmin(y, v.y); z = getmin(z, v.z);
}

double dist(const Vec3f& a, const Vec3f& b)
{
    return sqrt(sqr(a.x-b.x) + sqr(a.y-b.y) + sqr(a.z-b.z));
}

double dist2(const Vec3f& a, const Vec3f& b)
{
    return sqr(a.x-b.x) + sqr(a.y-b.y) + sqr(a.z-b.z);
}


Vec3f Cross(const Vec3f& a, const Vec3f& b)
{
    return Vec3f(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

double det(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
    return a[0]*b[1]*c[2]+a[1]*b[2]*c[0]+a[2]*b[0]*c[1]
            -a[2]*b[1]*c[0]-a[1]*b[0]*c[2]-a[0]*b[2]*c[1];
}
