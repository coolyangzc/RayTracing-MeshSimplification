#include "Bmp.h"
#include "common.h"
#include <fstream>
#include <cstdio>
#include <iostream>

Bmp::Bmp(int H_, int W_, Color** Mat_): pic(NULL)
{
    Initialize(H_, W_, Mat_);
}

Bmp::~Bmp()
{
    if (pic)
    {
        rep(i,H)
            delete[] pic[i];
        delete[] pic;
    }
}

void Bmp::Redata(int H_, int W_)
{
    if (pic)
    {
        rep(i,H)
            delete[] pic[i];
        delete[] pic;
    }

    H = H_; W = W_;
    if (H)
    {
        pic = new RGB*[H];
        rep(i,H)
            pic[i] = new RGB[W];
    }
}

void Bmp::Initialize(int H_, int W_, Color** Mat_)
{
    Redata(H_, W_);
    if (Mat_)
        rep(i,H) rep(j,W)
        {
            pic[i][j].r = (int) (getmin(1, Mat_[i][j].r) * 255);
            pic[i][j].g = (int) (getmin(1, Mat_[i][j].g) * 255);
            pic[i][j].b = (int) (getmin(1, Mat_[i][j].b) * 255);
        }

    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    FileHeader.bfOffBits = 54;

    InfoHeader.biSize = 40;
    InfoHeader.biWidth = W;
    InfoHeader.biHeight = H;
    InfoHeader.biPlanes = 1;
    InfoHeader.biBitCount = 24;
    InfoHeader.biCompression = 0; //BI_RGB
    InfoHeader.biSizeImage = 0;
    InfoHeader.biXPelsPerMeter = 0;
    InfoHeader.biYPelsPerMeter = 0;
    InfoHeader.biClrUsed = 0;
    InfoHeader.biClrImportant = 0;

    FileHeader.bfSize = InfoHeader.biSizeImage + FileHeader.bfOffBits;
}

void Bmp::ReadMatrix(int H_, int W_, Color** Mat_)
{
    Initialize(H_, W_, Mat_);
}

void Bmp::ReadBMP(std::string file)
{
    FILE* fin = fopen(file.c_str(), "rb");
    if (fin == NULL)
        printf("Error: loading %s\n failed", file.c_str());

    WORD bfType;
	fread(&bfType, 2, 1, fin);
    fread(&FileHeader, sizeof(FileHeader), 1, fin);
    fread(&InfoHeader, sizeof(InfoHeader), 1, fin);

    H = InfoHeader.biHeight;
    W = InfoHeader.biWidth;
    Redata(H, W);
    int repW = (4 - (W*3)%4) % 4;
    BYTE BYTE0;
    rep(i,H)
    {
        fread(&pic[i][0], 3*W, 1, fin);
        if (repW)
           fread(&BYTE0, 1, repW, fin);
    }
    fclose(fin);
}

void Bmp::SaveBMP(std::string file)
{

    FILE* fout = fopen(file.c_str(), "wb");
    WORD bfType = 0x4D42;
    fwrite(&bfType, 2, 1, fout);
    fwrite(&FileHeader, sizeof(FileHeader), 1, fout);
    fwrite(&InfoHeader, sizeof(InfoHeader), 1, fout);

    int repW = (4 - (W*3)%4) % 4;
    BYTE BYTE0 = 0;
    rep(i,H)
    {
        fwrite(&pic[i][0], 3*W, 1, fout);
        if (repW)
           fwrite(&BYTE0, 1, repW, fout);
    }
    fclose(fout);

}

Color Bmp::Getcolor(double x, double y)
{
    int X = x, Y = y;
    x -= X; y-=Y;
    if (x < -eps) x+=1;
    if (y < -eps) y+=1;
    X = x * W;
    Y = y * H;
    return Vec3f((double)pic[Y][X].r/255,
                 (double)pic[Y][X].g/255,
                 (double)pic[Y][X].b/255);
}

void Bmp::GetSlope(double x, double y, double& u, double& v)
{
    int X = x, Y = y;
    x -= X; y-=Y;
    if (x < -eps) x+=1;
    if (y < -eps) y+=1;
    X = x * W;
    Y = y * H;
    if (X && X<W-1) u = (double)(pic[Y][X-1].b - pic[Y][X+1].b)/255;
               else u = 0;
    if (Y && Y<H-1) v = (double)(pic[Y-1][X].b - pic[Y+1][X].b)/255;
               else v = 0;
}
