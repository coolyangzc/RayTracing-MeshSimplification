#include "Camera.h"
#include "common.h"

Camera::Camera()
{
    pic = NULL;
    W = H = 0;
    Aperature_R = 0;
    Aperature_D = 1;
    Aperature_Limit = 4;
}

Camera::~Camera()
{
    if (pic)
    {
        rep(i,H) delete []pic[i];
        delete []pic;
    }
}

void Camera::Input(std::ifstream& fin)
{
    if (pic)
    {
        rep(i,H) delete []pic[i];
        delete []pic;
    }

    std::string str;
    while (fin >> str)
    {
        if (same(str, "O")) fin >> O;
        else if (same(str, "D")) fin >> D;
        else if (same(str, "N")) fin >> N;
        else if (same(str, "image_H")) fin >> H;
        else if (same(str, "image_W")) fin >> W;
        else if (same(str, "len_H")) fin >> len_H;
        else if (same(str, "len_W")) fin >> len_W;
        else if (same(str, "Aperture_R")) fin >> Aperature_R;
        else if (same(str, "Aperture_D")) fin >> Aperature_D;
        else if (same(str, "Aperture_Q")) fin >> Aperature_Q >> Aperature_Limit;
        else if (same(str, "End")) break;
    }

    Wdiv2 = (double)(W-1)/2;
    Hdiv2 = (double)(H-1)/2;

    N.Normalize();

    Dy = N * (len_H / H);
    Dx = Cross(D, N).Normalize();
    Dx *= len_W / W;

    pic = new Color* [H];
    rep(i,H)
        pic[i] = new Color[W];
}

Vec3f Camera::Ray_emit(const double& x, const double& y) const
{
    return (D + Dx * (x-Wdiv2) + Dy * (y-Hdiv2)).Normalize();
}
