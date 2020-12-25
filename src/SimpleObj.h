#ifndef SIMPLEOBJ_H
#define SIMPLEOBJ_H

#include "Object.h"
#include "Texture.h"
#include <iostream>
#include <string>
#include <vector>

class SimpleObj
{
public:
    SimpleObj();
    virtual ~SimpleObj();
    void Input(std::ifstream& fin, std::vector <Object*>* object_vector, bool Smooth = 0);

    std::vector <Object*>* obj_vector;
    std::vector <Texture> text_vector;

    double scale, angle, cosv, sinv;
    Vec3f place;
    bool Smooth_N;
    short order[3], axis;

    std::string filepath;

private:
    bool LoadFromObj();
    void LoadFromMtl(char* file);
};

#endif // SIMPLEOBJ_H
