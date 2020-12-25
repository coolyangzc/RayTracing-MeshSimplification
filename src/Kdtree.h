#ifndef KDTREE_H
#define KDTREE_H

#include "Object.h"
#include <vector>

struct Node
{
    Node* ch[2];
    Vec3f boxmin, boxmax;
    int Lid, Rid;
    bool Inside(const Vec3f&) const;
    bool Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist) const;
};

class Kdtree
{
public:
    bool SAH;
    int Leafnum;
    Kdtree() {output = 0; id = tmpid = NULL; root = NULL; surface = NULL; obj_vector = NULL; Leafnum = 1; cnt = 0;}
    ~Kdtree();
    void Setup(std::vector <Object*> *object_vector);
    int GetHitid(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist) const;

private:

    bool output;

    int *id, *tmpid, split, cnt;
    Node* root;
    double bestSAH, *surface;
    std::vector <Object*> *obj_vector;

    int Search(Node*, const Vec3f&, const Vec3f&, double&) const;
    Node* Build(int l, int r, int sp);
    Kdtree(const Kdtree&);
    void CalcSAH(int l, int r, int sp);
    void Kdhalf(int l, int r, int mid, int sp);
    void Kdsort(int l, int r, int sp);
    void Clear(Node*);
};

#endif // KDTREE_H
