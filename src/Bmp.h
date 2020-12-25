#ifndef BMP_H
#define BMP_H

#include <string>
#include "Vec3f.h"

class Bmp
{
public:
    Bmp(int H_ = 0, int W_ = 0, Color** Mat_ = NULL);
    ~Bmp();
    void Initialize(int H_ = 0, int W_ = 0, Color** Mat_ = NULL);
    void ReadBMP(std::string file);
    void SaveBMP(std::string file);
    void ReadMatrix(int H_, int W_, Color** Mat_);
    Color Getcolor(double x, double y);
    void GetSlope(double x, double y, double& u, double& v);

private:
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;
    typedef long LONG;

    struct BITMAPFILEHEADER
    {
        //WORD    bfType; 因字节对齐问题而在此省略
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
    };

    struct BITMAPINFOHEADER
    {
        DWORD  biSize;
        LONG   biWidth;
        LONG   biHeight;
        WORD   biPlanes;
        WORD   biBitCount;
        DWORD  biCompression;
        DWORD  biSizeImage;
        LONG   biXPelsPerMeter;
        LONG   biYPelsPerMeter;
        DWORD  biClrUsed;
        DWORD  biClrImportant;
    };

    struct RGB
    {
        BYTE b, g, r;
    };

    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;

    RGB** pic;
    int H, W;
    void Redata(int H_, int W_);

    Bmp(const Bmp&);


};

#endif // BMP_H
