#ifndef CAMERA_H
#define CAMERA_H

#include "Vec3f.h"
#include <fstream>

/*
 * O: the center of the camera.
 * D: the direction vector of the camera. |D| is the distance between the canvas and the camera.
 * N: denotes the positive direction of the y-axis of the canvas. It will be normalized when initializing.
 * O, D, N are all in world space.
 
 * H, W: the image resolution (height and width)
 * len_H, len_W: the length and width of the canvas in world space.
*/

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
