#include "common.h"
#include "Object.h"
#include <cstdlib>

Object::Object()
{
    texture = new Texture;
    boxmin = inf;
    boxmax = -inf;
}

Object::~Object()
{
}

Sphere::Sphere(const Vec3f& O_, const double& R_): O(O_), R(R_)
{
    texture = new Texture;
    R2 = R*R;
    recR = 1/R;
}

bool Sphere::Inside(const Vec3f& v) const
{
    return (dist2(v, O) < R2);
}

Vec3f Sphere::RandomPoint()
{
    Vec3f P;
    do
        rep(i,3) P[i] = fRandom() * 2 - 1;
    while (P.length2() > 1);
    return O + P * R;
}

void Sphere::Input(std::ifstream& fin)
{
    std::string str;
    while (fin>>str)
    {
        if (same(str, "O")) fin >> O;
        else if (same(str, "R"))
        {
            fin >> R;
            R2 = R*R;
            recR = 1/R;
        }
        else if (texture->Input(fin, str))
            continue;
        else if (same(str, "End")) break;
    }
    boxmax = O+R;
    boxmin = O-R;
}

bool Sphere::Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist, Vec3f* N, Color* color)
{
    Vec3f T = O - Ray_O;
    if (dist < T.length() - R ) return false;
    double Ray_OtoFoot = T * Ray_D;
    double OtoFoot2 = T.length2() - sqr(Ray_OtoFoot);
    if (OtoFoot2 > R2 + eps) return false;
    double newdist = Ray_OtoFoot - sqrt(R2 - OtoFoot2);
    if (newdist > dist) return false;
    if (newdist < eps)
        if (Inside(Ray_O))
            newdist = Ray_OtoFoot + sqrt(R2 - OtoFoot2);
        else return false;

    dist = newdist;
    if (N)
    {
        *N = (Ray_O + Ray_D*dist -O)*recR;
        if (*N*Ray_D > eps) *N = -*N;
    }
    if (color) *color = texture->color;
    return true;
}

void Triangle::Input(std::ifstream& fin)
{
    std::string str;
    while (fin>>str)
    {
        if (same(str, "v"))
            rep(i,3) fin >> v[i];
        else if (same(str, "vt"))
            rep(i,3) fin >> vt[i];
        else if (same(str, "vn"))
            rep(i,3) fin >> vn[i];
        else if (texture->Input(fin, str))
            continue;
        else if (same(str, "End")) break;
    }
    Calc();
}

bool Triangle::Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist, Vec3f* N, Color* color)
{
    Vec3f par(det(v[0]-Ray_O, v[0]-v[1],  v[0]-v[2]),
              det(Ray_D,      v[0]-Ray_O, v[0]-v[2]),
              det(Ray_D,      v[0]-v[1],  v[0]-Ray_O));
    par /= det(Ray_D, v[0]-v[1], v[0]-v[2]);
    if (par[0]<eps || par[1]<-eps || par[2]<-eps || par[1]>1+eps || par[2]>1+eps || par[1]+par[2]>1+eps)
        return false;

    if (par[0] > dist) return false;
    dist = par[0];

    par[0] = 1 - par[1] - par[2];
    Vec3f Coor;
    Coor = par[0] * vt[0] + par[1] * vt[1] + par[2] * vt[2];

    if (N)
    {
        if (isSmooth_Normal())
            *N = (par[0] * vn[0] + par[1] * vn[1] + par[2] * vn[2]).Normalize();
        else *N = Normal;
        if (texture->bump != NULL)
        {
            double u,v;
            texture->bump->GetSlope(Coor.x, Coor.y, u, v);
            *N = (*N + (U*u + V*v) * texture->bump_k).Normalize();
        }
        if (*N*Ray_D > eps) *N = -*N;
    }
    if (color)
    {
        if (texture->isBmp())
            *color = texture->bmp->Getcolor(Coor.x, Coor.y).mul(texture->color);
        else
            *color = texture->color;
    }
    return true;
}

Vec3f Triangle::CalcUV(short axis)
{
    Vec3f p[2];
    p[0] = vt[1]-vt[0];
    p[1] = vt[2]-vt[0];
    rep(i,2)
        if (fabs(p[i][axis^1]) < eps)
            return ((v[i+1]-v[0]) * ((p[i][axis]>0)?1:-1)).Normalize();
    double x[2], k = p[1][axis^1] / p[0][axis^1];
    p[0] = p[0] - p[1] * k;

    if (p[0][axis] > 0)
        return ((v[1]-v[0]) - (v[2]-v[0]) * k).Normalize();
    else
        return ((v[0]-v[1]) + (v[2]-v[0]) * k).Normalize();

}

void Triangle::Calc()
{
    Normal = Cross(v[1]-v[0], v[2]-v[0]).Normalize();
    rep(i,3)
    {
        boxmin.Vgetmin(v[i]);
        boxmax.Vgetmax(v[i]);
    }
    if (texture->bump != NULL)
    {
        U = CalcUV(0);
        V = CalcUV(1);
    }
}
