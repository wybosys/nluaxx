#ifndef __NLUA_STRING_H_INCLUDED
#define __NLUA_STRING_H_INCLUDED

#include <vector>

NLUA_BEGIN

extern vector<string> explode(string const &str, string const &delimeter);

extern string implode(vector<string> const &, string const &delimeter);

extern bool endwith(string const &str, string const &tgt);

NLUA_END

#endif