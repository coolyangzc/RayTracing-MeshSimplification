#include "Objsimplifier.h"
#include "common.h"

#include <cstring>

Objsimplifier::Objsimplifier()
{
    en = pre = u = v = id = inid = fa = visit = sk = NULL;
    value = NULL;
    tri_belong = NULL;
    tri_exist = NULL;
    Q = NULL;
    useQEM = 1;
    trytimes = 1;
}

void Objsimplifier::Clean()
{
    std::cout << "!" << std::endl;
    if (u) delete []u;
    if (v) delete []v;
    if (sk) delete []sk;
    if (fa) delete []fa;
    if (en) delete []en;
    if (id) delete []id;
    if (pre) delete []pre;
    if (inid) delete []inid;
    if (value) delete []value;
    if (visit) delete []visit;
    if (tri_exist) delete []tri_exist;
    if (tri_belong) delete []tri_belong;
    if (Q)
    {
        For(i,n)
        {
            rep(j,4) delete []Q[i][j];
            delete []Q[i];
        }
        delete []Q;
    }
    newv.clear();
    rep(i,3) vid[i].clear();
    vec3f_vector.clear();
    Vec3f tmp;
    vec3f_vector.push_back(tmp);
    list_sz = 1;
    n = Trinum = heap_sz = 0;
}

Objsimplifier::~Objsimplifier()
{
    Clean();
}

int Objsimplifier::find(int a, int b)
{
    for(int i=en[a]; i; i=pre[i])
        if (v[i] == b) return i;
    return 0;
}

void Objsimplifier::Delete(int x, int y)
{
    int oldi = 0;
    for(int i=en[x];i;i=pre[i])
        if (getfa(v[i]) == y)
            if (i==en[x]) en[x] = pre[i];
                     else pre[oldi] = pre[i];
        else oldi = i;
}

void Objsimplifier::Sort_Up(int x)
{
    for(; x>1; x>>=1)
    {
        if (value[id[x]] > value[id[x>>1]] - eps) return;
        Exchange(x, x>>1);
    }
}

void Objsimplifier::Sort_Down(int x)
{
    for(;;x=small)
    {
        small = x;
        if (x+x   <= heap_sz && value[id[x+x]]   < value[id[small]] - eps) small = x+x;
        if (x+x+1 <= heap_sz && value[id[x+x+1]] < value[id[small]] - eps) small = x+x+1;
        if (small == x) return;
        Exchange(x, small);
    }
}

void Objsimplifier::Setup()
{
    n = vec3f_vector.size() - 1;
    Trinum = vid[0].size();
    en = new int[n + 1];
    fa = new int[n + 1];

    memset(en, 0, sizeof(int)*(n+1));
    memset(fa, 0, sizeof(int)*(n+1));

    u = new int[Trinum*6];
    v = new int[Trinum*6];
    pre = new int[Trinum*6];
    rep(x, Trinum)
    {
        rep(i,3)
        {
            int a = vid[i][x], b = vid[(i+1)%3][x];
            if (!find(a, b))
            {
                pre[++list_sz] = en[a]; en[a] = list_sz; u[list_sz] = a; v[list_sz] = b;
                pre[++list_sz] = en[b]; en[b] = list_sz; u[list_sz] = b; v[list_sz] = a;
            }
        }
    }
    heap_sz = list_sz >> 1;
    id = new int[heap_sz + 1];
    inid = new int[heap_sz + 1];
    value = new double[heap_sz + 1];
    visit = new int[heap_sz];
    memset(visit, 0, sizeof(int)*heap_sz);
    tri_exist = new bool[Trinum];
    memset(tri_exist, 1, sizeof(bool) * Trinum);
    limnum = Trinum * proportion;

}

void Objsimplifier::Shortest_Edge()
{
    For(i, heap_sz)
    {
        id[i] = inid[i] = i;
        value[i] = dist2(vec3f_vector[u[i<<1]], vec3f_vector[v[i<<1]]);
    }
    FORD(i, heap_sz>>1, 1) Sort_Down(i);

    times = 1;
    for(;Trinum > limnum; ++times)
    {
        int a = getfa(u[id[1]<<1]), b = getfa(v[id[1]<<1]);
        Delete(a,b);Delete(b,a);

        Exchange(1, heap_sz--);
        Sort_Down(1);
        Vec3f newv = (vec3f_vector[a] + vec3f_vector[b]) * 0.5;
        int oldi = 0, x;

        for(int i = en[a]; i; i=pre[i])
            if (visit[x = getfa(v[i])] < times)
            {
                visit[x] = times;
                value[i>>1] = dist2(vec3f_vector[x], newv);
                Sort_Up(inid[i>>1]);
                Sort_Down(inid[i>>1]);
            }

        oldi = -1;
        for(int i = en[b]; i; i=pre[i])
            if (visit[x = getfa(v[i])] < times)
            {
                visit[x] = times;
                value[i>>1] = dist2(vec3f_vector[x], newv);
                Sort_Up(inid[i>>1]);
                Sort_Down(inid[i>>1]);
                oldi = i;
            }
            else
            {
                Trinum--;
                int place = inid[i>>1];
                Exchange(place, heap_sz--);
                Sort_Up(place);
                Sort_Down(place);
                if (i == en[b]) en[b] = pre[i];
                           else pre[oldi] = pre[i];
                Delete(b,x);
                Delete(x,b);
            }

        vec3f_vector[a] = newv;
        fa[b] = a;
        if (oldi > -1)
        {
            pre[oldi] = en[a];
            en[a] = en[b];
        }

    }
    std::cout << Trinum << std::endl;
}

double Objsimplifier::Optimize(int u, int v, Vec3f& ans)
{
    bool Inv = 1;
    rep(i,3) rep(j,i+1) Mat[j][i] = Mat[i][j] = Q[u][i][j] + Q[v][i][j];
    rep(i,3) Mat[i][3] = -Q[u][i][3] - Q[v][i][3];
    rep(i,3)
    {
        int k = 0;
        For(j,2)
            if (fabs(Mat[i][j]) > fabs(Mat[i][k])) k = j;
        double w = Mat[i][k];
        if (fabs(w) < eps)
        {
            Inv = 0;
            break;
        }
        rep(j,4)
            Mat[i][j] /= w;
        rep(ii,3)
            if (ii != i)
            {
                w = Mat[ii][k];
                if (fabs(w) > eps)
                    rep(j,4)
                        Mat[ii][j] -= Mat[i][j] * w;
            }
    }
    q[3] = 1;
    Inv = 0;
    double cnt = 0;
    if (!Inv)
    {
        double best = inf;
        rep(t,trytimes+1)
        {
            double cnt = (double)t/trytimes;
            tmpv = vec3f_vector[u] * (cnt) + vec3f_vector[v] * (1-cnt);
            rep(i,3) q[i] = tmpv[i];
            cnt = 0;
            rep(i,4)
                rep(j,i+1)
                    cnt += (Q[u][i][j] + Q[v][i][j]) * q[i] * q[j] * ((i==j)?1:2);
            if (cnt < best)
            {
                best = cnt;
                ans[0] = q[0]; ans[1] = q[1]; ans[2] = q[2];
            }
        }
        return best;
    }
    else
        rep(i,3)
            rep(j,3)
                if (fabs(Mat[i][j]) > eps)
                {
                    q[j] = Mat[i][3];
                    break;
                }
    rep(i,4)
        rep(j,i+1)
            cnt += (Q[u][i][j] + Q[v][i][j]) * q[i] * q[j] * ((i==j)?1:2);
    ans[0] = q[0]; ans[1] = q[1]; ans[2] = q[2];
    return cnt;
}

void Objsimplifier::UpdateQ(int x)
{
    rep(i,4)
        rep(j,4)
            Q[x][i][j] = 0;
    rep(t, tri_belong[x].size())
    {
        int id = tri_belong[x][t];
        if (!exist(id))
        {
            tri_belong[x].erase(tri_belong[x].begin() + t);
            --t;
            continue;
        }
        Vec3f &p0 = vec3f_vector[vid[0][id]];
        Vec3f &p1 = vec3f_vector[vid[1][id]];
        Vec3f &p2 = vec3f_vector[vid[2][id]];
        Vec3f N = Cross(p1-p0, p2-p0).Normalize();
        q[0]=N[0]; q[1]=N[1]; q[2]=N[2];
        q[3] = -p0[0]*q[0]-p0[1]*q[1]-p0[2]*q[2];
        rep(i,4)
            rep(j,4)
                Q[x][i][j] += q[i]*q[j];
    }
}

void Objsimplifier::QEM()
{
    sk = new int[n+1];
    newv.resize(heap_sz+1);

    tri_belong = new std::vector<int>[n+1];
    rep(i, Trinum)
        rep(j,3)
            tri_belong[vid[j][i]].push_back(i);

    Q = new double**[n+1];
    For(i,n)
    {
        Q[i] = new double*[4];
        rep(j,4)
            Q[i][j] = new double[4];
        UpdateQ(i);
    }

    For(i, heap_sz)
    {
        id[i] = inid[i] = i;
        value[i] = Optimize(u[i<<1], v[i<<1], newv[i]);
    }
    FORD(i, heap_sz>>1, 1) Sort_Down(i);
    times = 1;
    for(;Trinum > limnum; ++times)
    {
        int a = getfa(u[id[1]<<1]), b = getfa(v[id[1]<<1]);
        Vec3f newp = newv[id[1]];

        Delete(a,b);Delete(b,a);

        Exchange(1, heap_sz--);
        Sort_Down(1);
        int oldi = -1, x; top = 0;
        for(int i = en[a]; i; i=pre[i])
            if (visit[x = getfa(v[i])] < times)
            {
                sk[++top] = x;
                visit[x] = times;
            }

        for(int i = en[b]; i; i=pre[i])
            if (visit[x = getfa(v[i])] < times)
            {
                sk[++top] = x;
                visit[x] = times;
                oldi = i;
            }
            else
            {
                Trinum--;
                int place = inid[i>>1];
                Exchange(place, heap_sz--);
                Sort_Up(place);
                Sort_Down(place);
                if (i == en[b]) en[b] = pre[i];
                           else pre[oldi] = pre[i];
                Delete(b,x);
                Delete(x,b);
            }
        vec3f_vector[a] = newp;
        fa[b] = a;
        ++times;
        rep(t, tri_belong[a].size())
            visit[tri_belong[a][t]] = times;
        rep(t, tri_belong[b].size())
            if (visit[tri_belong[b][t]] != times)
                tri_belong[a].push_back(tri_belong[b][t]);
        if (oldi > -1)
        {
            pre[oldi] = en[a];
            en[a] = en[b];
        }
        sk[++top] = a;
        For(t, top) UpdateQ(sk[t]);
        For(t, top)
        {
            a = sk[t];
            for(int i = en[a]; i; i=pre[i])
                if (a < (b = getfa(v[i])))
                {
                    value[i>>1] = Optimize(a, b, newv[i>>1]);
                    Sort_Up(inid[i>>1]);
                    Sort_Down(inid[i>>1]);
                }
        }
    }
    std::cout << Trinum << std::endl;

}

void Objsimplifier::Input(std::ifstream& fin)
{
    std::string str;
    while (fin >> str)
    {
        if (same(str, "input")) fin >> inputpath;
        else if (same(str, "output")) fin >> outputpath;
        else if (same(str, "ratio")) fin >> proportion;
        else if (same(str, "useQEM")) fin >> useQEM;
        else if (same(str, "trytimes")) fin >> trytimes;
        else if (same(str, "End")) break;
    }
    Clean();
    LoadFromObj();
    Setup();
    if (useQEM)
        QEM();
    else
        Shortest_Edge();
    SaveObj();
}

bool Objsimplifier::LoadFromObj()
{
    FILE* fp = fopen(inputpath.c_str(), "r");
    if (fp==NULL)
    {
        printf("Error: Loading %s failed.\n", inputpath.c_str());
        return false;
    }

    char buf[256];
    Vec3f vP;
    int vi[3];


    int lineNumber = 0;
    while (fscanf(fp, "%s", buf)!=EOF)
    {
        lineNumber ++;
        switch(buf[0])
        {
            case '#':				/* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), fp);
                break;
            case 'v':				/* v, vn, vt */
                switch(buf[1])
                {
                    case '\0':			    /* vertex */
                        if(fscanf(fp, "%lf%lf%lf", &vP.x, &vP.y, &vP.z)==3)
                            vec3f_vector.push_back(vP);
                        break;
                    default:
                        /* eat up rest of line */
	                    fgets(buf, sizeof(buf), fp);
                        break;
                }
                break;

            case 'f':				/* face */
                if(fscanf(fp, "%s", buf)!=1)
                {
                    printf("Error: Wrong Face at Line %d\n",lineNumber);
                    return false;
                }

                /* v */
                if( sscanf(buf, "%d", &vi[0]) ==1 &&
                    fscanf(fp, "%d",  &vi[1]) ==1 &&
                    fscanf(fp, "%d",  &vi[2]) ==1 )
                        rep(i,3) vid[i].push_back(vi[i]);
                else
                    {
                        printf("Error: Wrong Face at Line %d\n",lineNumber);
                        return false;
                    }
                break;

            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), fp);
                break;
        }

    }
    fclose(fp);
    return true;
}

void Objsimplifier::SaveObj()
{
    FILE* fp = fopen(outputpath.c_str(), "w");
    int sz = 0;
    For(i,n)
        if (getfa(i) == i)
        {
            id[i] = ++sz;
            fprintf(fp, "v %.12lf %.12lf %.12lf\n", vec3f_vector[i].x, vec3f_vector[i].y, vec3f_vector[i].z);
        }

    int dotnum = sz;
    int x = 0, y = 0, z = 0;
    Trinum = 0;
    rep(i,vid[0].size())
    {
        if (!exist(i)) continue;
        Trinum++;
        fprintf(fp, "f %d %d %d\n", id[getfa(vid[0][i])], id[getfa(vid[1][i])], id[getfa(vid[2][i])]);
    }
    std::cout << "fact:" << Trinum << std::endl;

    fprintf(fp, "#%d %d\n", dotnum, Trinum);
    fclose(fp);
}
