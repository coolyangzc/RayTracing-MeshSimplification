#include "Scene.h"
#include "common.h"

Scene::Scene()
{
    info.Specular_Pow = 50;
    info.Background_Color = Color(0, 0, 0);
    info.Ambient_Light_Luminance = 0;
    info.Uniformed_Light_Luminance = 0;
    info.Uniformed_Object_Parameter = 1;
    info.Light_Display = 0;
    info.Smooth_Normal = 1;
    info.AntiAliasing = 1;
    info.AA_resample = 0.1;
    info.kdTreeLeafnum = 1;
    info.kdTree_SAH = 1;
    info.Confined_RGB = 1;
    info.MaxDepth = 1;
}

Scene::~Scene()
{
    rep(i,object_vector.size())
        delete object_vector[i];
    rep(i,light_vector.size())
        delete light_vector[i];
}

void Scene::AddObject(Object* o)
{
    if (o->isLight())
        light_vector.push_back((Light*)o);
    else
        object_vector.push_back(o);
}

void Scene::clear()
{
    rep(i,object_vector.size())
        delete object_vector[i];
    rep(i,light_vector.size())
        delete light_vector[i];
    object_vector.clear();
    light_vector.clear();
}

void Scene::Input(std::ifstream& fin)
{
    std::string str;
    while (fin >> str)
    {
        if (same(str, "Background"))
            fin >> info.Background_Color;
        else if (same(str, "AmbientLum"))
            fin >> info.Ambient_Light_Luminance;
        else if (same(str, "UniformLum"))
            fin >> info.Uniformed_Light_Luminance;
        else if (same(str, "UniformObj"))
            fin >> info.Uniformed_Object_Parameter;
        else if (same(str, "ConfineRGB"))
            fin >> info.Confined_RGB;
        else if (same(str, "SpecularPow"))
            fin >> info.Specular_Pow;
        else if (same(str, "AntiAliasing"))
            fin >> info.AntiAliasing;
        else if (same(str, "MaxDepth"))
            fin >> info.MaxDepth;
        else if (same(str, "AA_Resample"))
            fin >> info.AA_resample;
        else if (same(str, "SmoothNormal"))
            fin >> info.Smooth_Normal;
        else if (same(str, "LightDisplay"))
            fin >> info.Light_Display;
        else if (same(str, "kdTreeLeafnum"))
            fin >> info.kdTreeLeafnum;
        else if (same(str, "KdTreeSAH"))
            fin >> info.kdTree_SAH;
        else if (same(str, "End")) return;
    }
}

void Scene::Confine()
{
    if (info.Uniformed_Light_Luminance)
    {
        double sum = info.Ambient_Light_Luminance;
        rep(x, light_vector.size())
            sum += light_vector[x]->texture->luminance;
        if (sum > 1)
        {
            info.Ambient_Light_Luminance /= sum;
            rep(x, light_vector.size())
                light_vector[x]->texture->luminance /= sum;
        }
    }
    if (info.Confined_RGB)
        rep(x, object_vector.size())
            object_vector[x]->texture->color.Confine();
    if (!info.Smooth_Normal)
        rep(x,object_vector.size())
        {
            Triangle* obj = (Triangle*)object_vector[x];
            if (obj->isTriangle())
                obj->vn[0] = 0;
        }
}
