#include "Texture.h"
#include "common.h"
#include <fstream>

Texture::Texture()
{
    diffuse = rindex = bump_k = 1;
    specular = reflection = refraction = 0;
    MC_quality = MC_limit = 0;
    bmp = bump = NULL;
    name = "";
    color = 1;
}

Texture::~Texture()
{
}

bool Texture::Input(std::ifstream& fin, const std::string& str)
{
    if (same(str, "Color"))     fin >> color;
    else if (same(str, "Lum"))  fin >> luminance;
    else if (same(str, "Diff")) fin >> diffuse;
    else if (same(str, "Spec")) fin >> specular;
    else if (same(str, "Refl")) fin >> reflection;
    else if (same(str, "Refr")) fin >> refraction;
    else if (same(str, "Rindex")) fin >> rindex;
    else if (same(str, "MC_quality")) fin >> MC_quality >> MC_limit;
    else if (same(str, "bump_k")) fin >> bump_k;
    else if (same(str, "bump_map"))
    {
        std::string file;
        fin >> file;
        bump = new Bmp;
        bump->ReadBMP(file);
    }
    else if (same(str, "Bmp"))
    {
        std::string file;
        fin >> file;
        bmp = new Bmp;
        bmp->ReadBMP(file);
    }
    else
        return false;
    return true;
}
