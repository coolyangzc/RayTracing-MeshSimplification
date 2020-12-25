#ifndef SCENE_H
#define SCENE_H

#include "Object.h"
#include <vector>
#include <fstream>

struct ParameterList
{
    Color Background_Color;
    double Ambient_Light_Luminance, AA_resample;
    short Specular_Pow, AntiAliasing, MaxDepth, kdTreeLeafnum;

    bool Uniformed_Light_Luminance, Uniformed_Object_Parameter;
    bool Confined_RGB, Smooth_Normal, Light_Display, kdTree_SAH;
};

class Scene
{
public:
    std::vector <Object*> object_vector;
    std::vector <Light*> light_vector;
    ParameterList info;

    Scene();
    virtual ~Scene();

    void AddObject(Object*);
    void clear();
    void Input(std::ifstream& fin);
    void Confine();
private:
    Scene(const Scene&);

};

#endif // SCENE_H
