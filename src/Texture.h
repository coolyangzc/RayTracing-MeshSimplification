#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vec3f.h"
#include "Bmp.h"

class Texture
{
public:
    Color color;
    Bmp *bmp, *bump;
    std::string name;
    short MC_quality, MC_limit;
    bool isBmp() const {return bmp!=NULL;}
    double luminance, bump_k;
    double diffuse, specular, reflection, refraction, rindex;
    Texture();
    virtual ~Texture();
    bool Input(std::ifstream& fin, const std::string& str);
};

#endif // TEXTURE_H
