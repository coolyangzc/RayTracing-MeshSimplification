#include "Raytracer.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include "common.h"
using namespace std;

int main()
{
    Raytracer tracer;

    tracer.Input("ferrari.txt");
    tracer.Output("test.bmp");

    return 0;
}
