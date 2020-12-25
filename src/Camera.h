#ifndef CAMERA_H
#define CAMERA_H

#include "Vec3f.h"
#include <fstream>

class Camera
{
public:
    Vec3f O, D, N, Dx, Dy;
    int H, W;
    double len_H, len_W, Hdiv2, Wdiv2, Aperature_R, Aperature_D, Aperature_Q, Aperature_Limit;
    Color** pic;

    Camera();
    ~Camera();
    void Input(std::ifstream& fin);

    Vec3f Ray_emit(const double& x, const double& y) const;
};

#endif // CAMERA_H
