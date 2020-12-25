#ifndef OBJSIMPLIFIER_H
#define OBJSIMPLIFIER_H

#include "Vec3f.h"

#include <vector>
#include <fstream>
#include <algorithm>

class Objsimplifier
{
public:
    Objsimplifier();
    ~Objsimplifier();

    void Input(std::ifstream& fin);
    void Clean();
private:

    int *en, *pre, *u, *v, *fa, *id, *inid, *visit, *sk;
    bool *tri_exist, useQEM;
    double *value, ***Q, q[4], Mat[3][4];
    Vec3f tmpv;

    int n, top, Trinum, limnum, list_sz, heap_sz, tmpid[3];
    int small, times, trytimes;
    std::vector <int> vid[3];
    std::vector <int> *tri_belong;
    std::vector <Vec3f> vec3f_vector, newv;
    std::string inputpath, outputpath;
    double proportion;

    int find(int u, int v);
    int getfa(int x) {return (fa[x]==0)?x:fa[x] = getfa(fa[x]);};
    bool exist(int id)
    {
        if (!tri_exist[id]) return false;
        for(int i=0; i<3; ++i)
        {
            tmpid[i] = getfa(vid[i][id]);
            for(int j=0; j<i; ++j)
                if (tmpid[i] == tmpid[j])
                    return tri_exist[id] = false;
        }
        return true;
    }

    bool LoadFromObj();
    void SaveObj();
    void Sort_Up(int x);
    void Sort_Down(int x);
    void Delete(int x, int y);
    void Exchange(int x, int y) {inid[id[x]] = y; inid[id[y]] = x; std::swap(id[x], id[y]);};
    void Setup();
    void Shortest_Edge();
    void QEM();
    void UpdateQ(int x);
    double Optimize(int u, int v, Vec3f& ans);
};

#endif // OBJSIMPLIFIER_H
