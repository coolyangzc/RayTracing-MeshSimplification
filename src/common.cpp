#include "common.h"

bool same(const std::string& input, const std::string& tar)
{
    if (input.length() > tar.length()+1) return false;
    std::string::const_iterator p = input.begin();
    std::string::const_iterator q = tar.begin();

    while (p != input.end() && q != tar.end() && toupper(*p) == toupper(*q))
        ++p, ++q;
    return (p == input.end()) || (q == tar.end());

}
