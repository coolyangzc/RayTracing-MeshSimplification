#include "Raytracer.h"
#include "common.h"
#include <ctime>
#include <cstdlib>


Raytracer::Raytracer()
{
    srand(time(0));
}

Raytracer::~Raytracer()
{
}

void Raytracer::Square_Input(std::ifstream& fin)
{
    std::string str;
    Texture* texture = new Texture;
    Vec3f v[4], vt[4], vn[4];
    while (fin>>str)
    {
        if (same(str, "v"))
            rep(i,4) fin >> v[i];
        else if (same(str, "vt"))
            rep(i,4) fin >> vt[i];
        else if (same(str, "vn"))
            rep(i,4) fin >> vn[i];
        else if (texture->Input(fin, str))
            continue;
        else if (same(str, "End")) break;
    }

    Triangle* new_Tri = new Triangle;
    delete new_Tri->texture;
    new_Tri->texture = texture;
    rep(i,3)
    {
        new_Tri->v[i] = v[i];
        new_Tri->vn[i] = vn[i];
        new_Tri->vt[i] = vt[i];
    }
    new_Tri->Calc();
    scene.AddObject(new_Tri);

    new_Tri = new Triangle;
    delete new_Tri->texture;
    new_Tri->texture = texture;
    rep(i,3)
    {
        int j = (i==0)?i:i+1;
        new_Tri->v[i] = v[j];
        new_Tri->vn[i] = vn[j];
        new_Tri->vt[i] = vt[j];
    }
    new_Tri->Calc();
    scene.AddObject(new_Tri);
}

void Raytracer::Input(std::ifstream& fin)
{
    if (!fin.is_open()) return;
    std::string str;
    Object* new_obj = NULL;
    while (fin >> str)
    {
        new_obj = NULL;
        if (same(str, "Sphere")) new_obj = new Sphere;
        else if (same(str, "Triangle")) new_obj = new Triangle;
        else if (same(str, "Light")) new_obj = new Light;
        else if (same(str, "Obj"))
        {
            simpleobj = new SimpleObj;
            simpleobj->Input(fin, &scene.object_vector, scene.info.Smooth_Normal);
        }
        else if (same(str, "Objsimplify")) simplifier.Input(fin);
        else if (same(str, "Camera")) camera.Input(fin);
        else if (same(str, "Parameter")) scene.Input(fin);
        else if (same(str, "Square"))
        {
            Square_Input(fin);
            continue;
        }
        if (new_obj)
        {
            new_obj->Input(fin);
            scene.AddObject(new_obj);
        }
    }
    scene.Confine();
    kdtree.Leafnum = scene.info.kdTreeLeafnum;
    kdtree.SAH = scene.info.kdTree_SAH;

    clock_t s = clock();
    kdtree.Setup(&scene.object_vector);
    clock_t t = clock();
    dur1 = t-s;
    std::cout << "K-d tree Setup Time:" << dur1/CLOCKS_PER_SEC << std::endl;
    dur1 = 0;

}

void Raytracer::Input(std::string file)
{
    std::ifstream fin(file.c_str());
    Input(fin);
}

void Raytracer::Output(std::string file)
{
    Render();
    bmp.ReadMatrix(camera.H, camera.W, camera.pic);
    bmp.SaveBMP(file);
}

void Raytracer::Render()
{
    ParameterList& info = scene.info;
    Vec3f Ray_D;
    dur1 = dur2 = 0;
    double d = (double) 1 / info.AntiAliasing;
    if (camera.Aperature_R > eps)
    {
        Sphere Cam(camera.O, camera.Aperature_R);
        Vec3f CamO, focus;
        Color Real, Res;
        rep(i,camera.H)
        {
            if ((i & 7) == 0)
                std::cout << "sample: " << i << "/" << camera.H << std::endl;
            rep(j,camera.W)
            {
                focus = camera.O + camera.Ray_emit(j + 0.5,i + 0.5) * camera.Aperature_D;
                camera.pic[i][j] = Real = Raytracing(camera.O, (focus-camera.O).Normalize(), 0);;
                bool flag = 1;
                rep(k, camera.Aperature_Limit)
                {
                    CamO = Cam.RandomPoint();
                    camera.pic[i][j] += Res = Raytracing(CamO, (focus-CamO).Normalize(), 0);;
                    if (dist(Res, Real) > 0.05) flag = 0;
                }
				// The results are similar
                if (flag)
                {
                    camera.pic[i][j] /= (camera.Aperature_Limit+1);
                    continue;
                }
				// Require more sampling (the results are not similar)
                rep(k, camera.Aperature_Q - camera.Aperature_Limit)
                {
                    CamO = Cam.RandomPoint();
                    camera.pic[i][j] += Raytracing(CamO, (focus-CamO).Normalize(), 0);
                }
                camera.pic[i][j] /= (camera.Aperature_Q+1);
            }
        }
        return;
    }
    if (info.AntiAliasing <= 1 || info.AA_resample > eps)
        rep(i,camera.H)
        {
            if ((i & 7) == 0)
                std::cout << "sample: " << i << "/" << camera.H << std::endl;
            rep(j,camera.W)
                camera.pic[i][j] = Raytracing(camera.O, camera.Ray_emit(j + 0.5,i + 0.5), 0);
        }
    if (info.AntiAliasing <= 1) return;

    if (info.AA_resample < eps)
    {
        rep(i,camera.H)
        {
            if ((i & 7) == 0)
                std::cout << "sample: " << i << "/" << camera.H << std::endl;
            rep(j,camera.W)
            {
                camera.pic[i][j] = 0;
                rep(dx, info.AntiAliasing)
                    rep(dy, info.AntiAliasing)
                    {
                        Ray_D = camera.Ray_emit(j + (fRandom()+dy)*d, i + (fRandom()+dx)*d);
                        camera.pic[i][j] += Raytracing(camera.O, Ray_D, 0);
                    }
                camera.pic[i][j] /= sqr(info.AntiAliasing);
            }
        }
        return;
    }

    rep(i,camera.H)
    {
        if ((i & 7) == 0)
            std::cout << "resample: " << i << "/" << camera.H << std::endl;
        rep(j,camera.W)
        {
            if ((i==0          || dist(camera.pic[i][j], camera.pic[i-1][j]) < info.AA_resample) &&
                (i==camera.H-1 || dist(camera.pic[i][j], camera.pic[i+1][j]) < info.AA_resample) &&
                (j==0          || dist(camera.pic[i][j], camera.pic[i][j-1]) < info.AA_resample) &&
                (j==camera.W-1 || dist(camera.pic[i][j], camera.pic[i][j+1]) < info.AA_resample))
                continue;
            rep(dx, info.AntiAliasing)
                rep(dy, info.AntiAliasing)
                {
                    Ray_D = camera.Ray_emit(j + (fRandom()+dy)*d, i + (fRandom()+dx)*d);
                    camera.pic[i][j] += Raytracing(camera.O, Ray_D, 0);
                }
            camera.pic[i][j] /= sqr(info.AntiAliasing) + 1;
        }
    }
}

Color Raytracer::Raytracing(const Vec3f& Ray_O, const Vec3f& Ray_D, short depth)
{
    ParameterList& info = scene.info;

    double dist = inf;
    Object* hitobj = NULL;

    if (info.Light_Display)
        rep(x, scene.light_vector.size())
        {
            Light*& light = scene.light_vector[x];
            if (light->texture->luminance < eps) continue;
            if (light->Intersect(Ray_O, Ray_D, dist))
                hitobj = light;
        }
    int hitid = kdtree.GetHitid(Ray_O, Ray_D, dist);

    if (hitid > -1)
        hitobj = scene.object_vector[hitid];
    else if (hitobj == NULL)
        return info.Background_Color;
    else return Vec3f(1,1,1);
    Vec3f Normal;
    Color color;

    dist = inf;
    hitobj->Intersect(Ray_O, Ray_D, dist, &Normal, &color);

    Vec3f hitpoint = Ray_O + Ray_D*dist;
    Vec3f Ray_Refl = Ray_D.Reflect(Normal);

    Color result = info.Ambient_Light_Luminance * hitobj->texture->diffuse * color;

    if (depth < info.MaxDepth)
    {
        if (hitobj->texture->reflection > eps)
            result += hitobj->texture->reflection *
                      Raytracing(hitpoint + Ray_Refl*eps, Ray_Refl, depth+1);
        if (hitobj->texture->refraction > eps)
        {
            double n = (double) 1 / hitobj->texture->rindex;
            if (hitobj->Inside(Ray_O))
                n = 1 / n;
            Vec3f Ray_Refr = Ray_D.Refract(Normal, n);
            if (!Ray_Refr.isZero())
                result += hitobj->texture->refraction *
                          Raytracing(hitpoint + Ray_Refr*eps, Ray_Refr, depth+1);
        }

    }

    rep(x, scene.light_vector.size())
    {
        Light*& light = scene.light_vector[x];

        if (light->texture->luminance < eps) continue;

        Vec3f light_D = (light->O - hitpoint).Normalize();
        double Dot = Normal*light_D, SpecLum = Ray_Refl*light_D;

        if (SpecLum < eps) SpecLum = 0;
                      else SpecLum = pow(SpecLum, info.Specular_Pow);
        if (Dot < eps) Dot = 0;
        if (Dot < eps && SpecLum < eps) continue;

        dist = inf;
        double partlum = 1;
        if (kdtree.GetHitid(hitpoint + light_D * eps, light_D, dist) != -1)
            if (!light->texture->MC_quality)
                continue;
            else
                partlum = 0;
        rep(tt, light->texture->MC_quality)
        {
            dist = inf;
            light_D = (light->RandomPoint() - hitpoint).Normalize();
            if (kdtree.GetHitid(hitpoint + light_D * eps, light_D, dist) == -1)
                partlum += 1;
            if (tt == light->texture->MC_limit - 1)
            {
                if (partlum < eps) break;
                if (fabs(partlum - light->texture->MC_limit - 1) < eps)
                {
                    partlum = light->texture->MC_quality + 1;
                    break;
                }
            }
        }
        partlum /= (light->texture->MC_quality+1);
        if (partlum < eps) continue;

        double diff = hitobj->texture->diffuse  * light->texture->luminance * partlum * Dot;
        double spec = hitobj->texture->specular * light->texture->luminance * partlum * SpecLum;

        result += color * diff;
        result += Vec3f(1,1,1) * spec;

    }
    return result;

}
