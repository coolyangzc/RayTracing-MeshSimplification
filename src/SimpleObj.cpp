#include "SimpleObj.h"
#include "common.h"
#include <cmath>
#include <cstring>

SimpleObj::SimpleObj()
{
    Texture tmp;
    text_vector.push_back(tmp);
    obj_vector = NULL;
    scale = 1;
    rep(i,3) order[i] = i;
    angle = 0;
    axis = 0;
}

SimpleObj::~SimpleObj()
{
}

void SimpleObj::Input(std::ifstream& fin, std::vector <Object*>* object_vector, bool Smooth)
{
    obj_vector = object_vector;
    Smooth_N = Smooth;
    std::string str;
    while (fin >> str)
    {
        if (same(str, "scale")) fin >> scale;
        else if (same(str, "place")) fin >> place;
        else if (same(str, "file")) fin >> filepath;
        else if (same(str, "order")) rep(i,3) fin >> order[i];
        else if (same(str, "rotate")) fin >> axis >> angle;
        else if (text_vector[0].Input(fin, str)) continue;
        else if (same(str, "End")) break;
    }
    cosv = cos(angle/180*pi);
    sinv = sin(angle/180*pi);
    LoadFromObj();
}

bool SimpleObj::LoadFromObj()
{
    FILE* fp = fopen(filepath.c_str(), "r");
    if (fp==NULL)
    {
        printf("Error: Loading %s failed.\n", filepath.c_str());
        return false;
    }

    char buf[256];
    Triangle* new_Tri = NULL;
    std::vector <Vec3f> v, vt, vn;
    std::vector <int> vid[3];
    int start = obj_vector->size();
    int vi[4], vti[4], vni[4];
    bool vtflag, vnflag;
    int face4, textid = 0;
    Vec3f vP, vQ;
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
                        {
                            rep(i,3) vQ[i] = vP[order[i]];
                            vQ = vQ*scale + place;
                            short x = (axis+1)%3, y = (axis+2)%3;
                            vP[x] = cosv * vQ[x] - sinv * vQ[y];
                            vP[y] = sinv * vQ[x] + cosv * vQ[y];
                            vP[axis] = vQ[axis];
                            v.push_back(vP);
                        }
                        break;
                    case 't':
                        vP.z = 0;
                        if(fscanf(fp, "%lf%lf%lf", &vP.x, &vP.y, &vP.z)>=2)
                            vt.push_back(vP);
                        break;
                    case 'n':
                        if(fscanf(fp, "%lf%lf%lf", &vP.x, &vP.y, &vP.z)==3)
                        {
                            rep(i,3) vQ[i] = vP[order[i]];
                            short x = (axis+1)%3, y = (axis+2)%3;
                            vP[x] = cosv * vQ[x] - sinv * vQ[y];
                            vP[y] = sinv * vQ[x] + cosv * vQ[y];
                            vP[axis] = vQ[axis];
                            vn.push_back(vP);
                        }
                        break;
                    default:
                        /* eat up rest of line */
	                    fgets(buf, sizeof(buf), fp);
                        break;
                }
                break;

            case 'f':				/* face */
                {
                    vtflag = vnflag = face4 = 0;
                    if(fscanf(fp, "%s", buf)!=1)
                    {
                        printf("Error: Wrong Face at Line %d\n",lineNumber);
                        return false;
                    }

                    /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                    if (strstr(buf, "//"))
                    {
                        /* v//n */
                        if( sscanf(buf, "%d//%d", &vi[0], &vni[0]) ==2  &&
                            fscanf(fp, "%d//%d",  &vi[1], &vni[1]) ==2  &&
                            fscanf(fp, "%d//%d",  &vi[2], &vni[2]) ==2)
                        {
                            face4 = fscanf(fp, "%d//%d",  &vi[3], &vni[3]);
                            vnflag = 1;
                        }

                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }

                    }
                    else if (sscanf(buf, "%d/%d/%d", &vi[0], &vti[0], &vni[0]) == 3)
                    {
                        /* v/t/n */
                        if( fscanf(fp, "%d/%d/%d", &vi[1], &vti[1], &vni[1]) ==3 &&
                            fscanf(fp, "%d/%d/%d", &vi[2], &vti[2], &vni[2]) ==3 )
                        {
                            face4 = fscanf(fp, "%d/%d/%d", &vi[3], &vti[3], &vni[3]);
                            vtflag = vnflag = 1;
                        }
                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    else if (sscanf(buf, "%d/%d", &vi[0], &vti[0]) == 2)
                    {
                        /* v/t */
                        if( fscanf(fp, "%d/%d", &vi[1], &vti[1]) ==2 &&
                            fscanf(fp, "%d/%d", &vi[2], &vti[2]) ==2 )
                        {
                            face4 = fscanf(fp, "%d/%d", &vi[3], &vti[3]);
                            vtflag = 1;
                        }

                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    else
                    {
                        /* v */
                        if( sscanf(buf, "%d", &vi[0]) ==1 &&
                            fscanf(fp, "%d",  &vi[1]) ==1 &&
                            fscanf(fp, "%d",  &vi[2]) ==1 )
                            face4 = fscanf(fp, "%d",  &vi[3]);
                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }
                    }

                    new_Tri = new Triangle;
                    delete new_Tri->texture;
                    new_Tri->texture = &text_vector[textid];
                    rep(i,3)
                    {
                        new_Tri->v[i] = v[vi[i]-1];
                        if (vtflag) new_Tri->vt[i] = vt[vti[i]-1];
                        if (vnflag) new_Tri->vn[i] = vn[vni[i]-1];
                        vid[i].push_back(vi[i]-1);
                    }
                    new_Tri->Calc();
                    obj_vector->push_back(new_Tri);

                    if(face4 <= 0) break;

                    new_Tri = new Triangle;
                    delete new_Tri->texture;
                    new_Tri->texture = &text_vector[textid];
                    rep(i,3)
                    {
                        int j = (i==0)?i:i+1;
                        new_Tri->v[i] = v[vi[j]-1];
                        if (vtflag) new_Tri->vt[i] = vt[vti[j]-1];
                        if (vnflag) new_Tri->vn[i] = vn[vni[j]-1];
                        vid[i].push_back(vi[j]-1);
                    }
                    new_Tri->Calc();
                    obj_vector->push_back(new_Tri);
                }

                break;

            case 'm':
                {
                    if (strcmp(buf, "mtllib") != 0)
                    {
                        fgets(buf, sizeof(buf), fp);
                        break;
                    }
                    fscanf(fp, "%s", buf);
                    LoadFromMtl(buf);
                }
                break;

            case 'u':
                {
                    if (strcmp(buf, "usemtl") != 0)
                    {
                        fgets(buf, sizeof(buf), fp);
                        break;
                    }
                    fscanf(fp, "%s", buf);
                    textid = 0;
                    For(i, text_vector.size()-1)
                        if (text_vector[i].name == buf)
                        {
                            textid = i; break;
                        }
                }

            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), fp);
                break;
        }

    }
    std::cout << "Triangle num:" << obj_vector->size() - start << std::endl;
    std::cout << "SimpleObj success" << std::endl;

    fclose(fp);
    if (!Smooth_N || vn.size()) return true;
    vn.resize(v.size());
    vt.clear();
    vt.resize(v.size());
    Vec3f N;
    Triangle* tri;

    rep(i, vid[0].size())
    {
        tri = (Triangle*)(*obj_vector)[start+i];
        N = tri->Normal * Cross(v[vid[1][i]]-v[vid[0][i]], v[vid[2][i]]-v[vid[1][i]]).length();
        rep(j,3)
            vn[vid[j][i]] += N;

    }
    rep(i, v.size()) vn[i].Normalize();
    rep(i, vid[0].size())
    {
        tri = (Triangle*)(*obj_vector)[start+i];
        rep(j,3)
            tri->vn[j] = vn[vid[j][i]];
    }

    return true;
}

void SimpleObj::LoadFromMtl(char* file)
{
    unsigned found = filepath.find_last_of("/\\");
    filepath = filepath.substr(0,found+1);

    std::string str = filepath + std::string(file);
    std::ifstream fin(str.c_str());
    if (!fin.is_open())
    {
        printf("Error: Loading %s failed\n",str.c_str());
        return;
    }
    Texture* newtexture = NULL;

    while (fin >> str)
    {
        if (same(str, "newmtl"))
        {
            if (newtexture)
            {
                if (newtexture->isBmp())
                    newtexture->color = 1;
                text_vector.push_back(*newtexture);
                delete newtexture;
            }
            newtexture = new Texture(text_vector[0]);
            fin >> newtexture->name;
        }
        else if(same(str, "kd")) fin >> newtexture->color;
        else if(same(str, "map_Kd"))
        {
            fin >> str;
            newtexture->bmp = new Bmp;
            newtexture->bmp->ReadBMP(filepath + str);
        }
        else if (same(str, "bump"))
        {
            fin >> str;
            newtexture->bump = new Bmp;
            newtexture->bump->ReadBMP(filepath + str);
        }
    }
    if (newtexture)
    {
        if (newtexture->isBmp())
            newtexture->color = 1;
        text_vector.push_back(*newtexture);
        delete newtexture;
    }
}
