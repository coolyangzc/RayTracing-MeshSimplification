#include "Kdtree.h"
#include "common.h"
#include <cstdlib>
#include <algorithm>

bool Node::Inside(const Vec3f& v) const
{
    rep(i,3)
        if (v[i] < boxmin[i] - eps10 || v[i] > boxmax[i] + eps10)
            return false;
    return true;
}

bool Node::Intersect(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist) const
{
    if (Inside(Ray_O))
    {
        dist = 0;
        return true;
    }
    double d[3], d2[3];
    rep(i,3)
    {
        if (fabs(Ray_D[i]) < eps)
        {
            d[i]  = (boxmin[i] - Ray_O[i]) * inf;
            d2[i] = (boxmax[i] - Ray_O[i]) * inf;
        }
        else
        {
            d[i]  = (boxmin[i] - Ray_O[i]) / Ray_D[i];
            d2[i] = (boxmax[i] - Ray_O[i]) / Ray_D[i];
        }
        d[i] = getmin(d[i], d2[i]);
    }
    double newdist = getmax(d[0], d[1]);
    newdist = getmax(d[2], newdist);

    if (newdist > dist || newdist < eps) return false;
    if (Inside(Ray_O + Ray_D * newdist))
    {
        dist = newdist;
        return true;
    }
    return false;
}

void Kdtree::Setup(std::vector <Object*>* object_vector)
{
    obj_vector = object_vector;
    int n = obj_vector->size();
    if (n == 0) return;
    id = new int[n];
    tmpid = new int[n*3];
    surface = new double[n*3];
    rep(i,n) id[i] = i;
    root = Build(0,n-1,(SAH)?-1:0);
    std::cout << "K-d tree Node Num:" << cnt << std::endl;
    delete []tmpid;
    delete []surface;
}

Kdtree::~Kdtree()
{
    if (id) delete []id;
    if (root) Clear(root);
}

void Kdtree::Kdhalf(int left, int right, int mid, int sp)
{
    if (left == right) return;
    int l = left, r = right;
    int mi = id[l + Random(r-l)];
    double m = (*obj_vector)[mi]->boxmin[sp];

    FOR(i, left, right)
        if ((*obj_vector)[id[i]]->boxmin[sp] < m)
            id[l++] = id[i];
        else if (id[i]!=mi)
            tmpid[r--] = id[i];
    FOR(i, r+1, right)
        id[i] = tmpid[i];
    id[r] = mi;
    if (r==mid) return;
    if (r>mid) Kdhalf(left, r-1, mid, sp);
         else  Kdhalf(r+1,right, mid, sp);
}

void Kdtree::Kdsort(int left, int right, int sp)
{
    int l = left, r = right;
    double m = (*obj_vector)[tmpid[l+Random(r-l)]]->boxmin[sp];
    while(l <= r)
    {
        while ((*obj_vector)[tmpid[l]]->boxmin[sp] < m-eps) l++;
        while ((*obj_vector)[tmpid[r]]->boxmin[sp] > m+eps) r--;
        if (l <= r)
        {
            std::swap(tmpid[l], tmpid[r]);
            l++; r--;
        }
    }
    if (left < r) Kdsort(left, r, sp);
    if (l < right) Kdsort(l, right, sp);
}

void Kdtree::CalcSAH(int l, int r, int sp)
{
    Kdsort(l, r, sp);

    Vec3f boxmin(inf, inf, inf), boxmax(-inf, -inf, -inf);

    FOR(i,l,r)
    {
        boxmin.Vgetmin((*obj_vector)[tmpid[i]]->boxmin);
        boxmax.Vgetmax((*obj_vector)[tmpid[i]]->boxmax);
        surface[i] = (boxmax - boxmin).Surfacediv2();
    }
    boxmin = inf, boxmax = -inf;
    double surface2, SAH;
    FORD(i,r-1,l)
    {
        boxmin.Vgetmin((*obj_vector)[tmpid[i+1]]->boxmin);
        boxmax.Vgetmax((*obj_vector)[tmpid[i+1]]->boxmax);
        surface2 = (boxmax - boxmin).Surfacediv2();
        SAH = surface[i] * (i-l+1) + surface2 * (r-i);

        if (SAH < bestSAH)
        {
            bestSAH = SAH;
            split = i;
        }
    }
}

Node* Kdtree::Build(int l, int r, int sp)
{
    cnt++;
    Node* newNode = new Node;
    if (r-l < Leafnum)
    {
        newNode->boxmin = (*obj_vector)[id[l]]->boxmin;
        newNode->boxmax = (*obj_vector)[id[l]]->boxmax;
        FOR(i, l+1, r)
        {
            newNode->boxmin.Vgetmin((*obj_vector)[id[i]]->boxmin);
            newNode->boxmax.Vgetmax((*obj_vector)[id[i]]->boxmax);
        }
        newNode->ch[0] = newNode->ch[1] = NULL;
        newNode->Lid = l;
        newNode->Rid = r;
        return newNode;
    }
    newNode->Lid = -1;
    if (sp == -1)
    {
        int len = r-l+1;
        rep(i, len)
            tmpid[i] = tmpid[len+i] = tmpid[len*2+i] = id[l+i];
        bestSAH = inf;
        bestSAH *= inf;
        CalcSAH(0,    len-1,  0);
        CalcSAH(len,  len*2-1,1);
        CalcSAH(len*2,len*3-1,2);
        int d;
        if (split <= len-1) d = 0;
        else if (split <= len*2-1) d = len;
        else d = len<<1;
        rep(i,len)
            id[l+i] = tmpid[i+d];
        split = l + split % len;
        newNode->ch[0] = Build(l,   split, -1);
        newNode->ch[1] = Build(split+1, r, -1);
    }
    else
    {
        int m = (l+r) >> 1;
        Kdhalf(l, r, m, sp);
        newNode->ch[0] = Build(l,   m, (sp+1)%3);
        newNode->ch[1] = Build(m+1, r, (sp+1)%3);
    }

    newNode->boxmin = newNode->ch[0]->boxmin;
    newNode->boxmin.Vgetmin(newNode->ch[1]->boxmin);
    newNode->boxmax = newNode->ch[0]->boxmax;
    newNode->boxmax.Vgetmax(newNode->ch[1]->boxmax);

    return newNode;
}
int Kdtree::Search(Node* x, const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist) const
{
    if (x->Lid != -1)
    {
        int hitid = -1;
        FOR(i, x->Lid, x->Rid)
            if ((*obj_vector)[id[i]]->Intersect(Ray_O, Ray_D, dist))
                hitid = id[i];
        return hitid;
    }

    double d[2];
    rep(i,2)
    {
        d[i] = inf;
        x->ch[i]->Intersect(Ray_O, Ray_D, d[i]);
    }
    if (d[0] == inf && d[1] == inf) return -1;
    int p = (d[0] < d[1]);

    int hitid = Search(x->ch[p], Ray_O, Ray_D, dist);
    if (dist < d[p^1] - eps || d[p^1] == inf) return hitid;
    int hitid2 = Search(x->ch[p^1], Ray_O, Ray_D, dist);
    return (hitid2 == -1)? hitid: hitid2;
}

int Kdtree::GetHitid(const Vec3f& Ray_O, const Vec3f& Ray_D, double& dist) const
{
    if (obj_vector->size() == 0) return -1;
    return Search(root, Ray_O, Ray_D, dist);
}

void Kdtree::Clear(Node* x)
{
    rep(i,2)
        if (x->ch[i]) Clear(x->ch[i]);
    delete x;
}
