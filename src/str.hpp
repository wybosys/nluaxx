#ifndef __NLUA_STR_H_INCLUDED
#define __NLUA_STR_H_INCLUDED

// github.com/wybosys/nnt.cross

NLUA_BEGIN

extern vector <string> explode(string const &str, string const &delimeter, bool skipempty = false);

extern string implode(vector < string >
const&,
string const &delimeter
);

extern bool beginwith(string const &str, string const &tgt);

extern bool endwith(string const &str, string const &tgt);

extern int toInt(string const &);

extern float toFloat(string const &);

extern double toDouble(string const &);

NLUA_END

#endif
