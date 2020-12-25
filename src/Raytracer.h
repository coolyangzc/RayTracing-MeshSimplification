#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Bmp.h"
#include "Scene.h"
#include "Camera.h"
#include "Kdtree.h"
#include "SimpleObj.h"
#include "Objsimplifier.h"

#include <string>
#include <fstream>

class Raytracer
{
public:
    double dur1, dur2;
    Bmp bmp;
    Scene scene;
    Camera camera;
    SimpleObj* simpleobj;
    Objsimplifier simplifier;
    Kdtree kdtree;

    Raytracer();
    virtual ~Raytracer();

    void Input(std::ifstream& fin);
    void Input(std::string file);

    void Output(std::string file);
    void Render();

    Color Raytracing(const Vec3f& Ray_O, const Vec3f& Ray_D, short depth);

private:
    void Square_Input(std::ifstream& fin);
};



#endif // RAYTRACER_H
