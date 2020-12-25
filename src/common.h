#ifndef COMMON_H_
#define COMMON_H_

//≥£”√∫Í
#define rep(i,n) for(int i=0; i<n; ++i)
#define For(i,n) for(int i=1; i<=n; ++i)
#define FOR(i,a,b) for(int i=a; i<=b; ++i)
#define FORD(i,a,b) for(int i=a; i>=b; --i)
#define sqr(x) ((x)*(x))
#define Random(n) ((rand() << 15 ^ rand())%(n))
#define fRandom() ((double)rand()/32767)
#define getmin(a,b) (((a)<(b))?(a):(b))
#define getmax(a,b) (((a)>(b))?(a):(b))

#include <cmath>
#include <string>

const double eps = 1e-8;
const double eps10 = eps * 10;
const double inf = 1e10;
const double pi = 3.1415926536;

bool same(const std::string& input, const std::string& tar);

#endif // COMMON_H_
