#ifndef __NLUA_FS_H_INCLUDED
#define __NLUA_FS_H_INCLUDED

// github.com/wybosys/nnt.cross

NLUA_BEGIN

extern const string PATH_DELIMITER;

extern string replace(string const &, string const &match, string const &tgt);

extern string normalize(string const &);

extern bool mkdir(string const &);

extern bool mkdirs(string const &);

extern bool exists(string const &);

extern bool isfile(string const &);

extern bool isdirectory(string const &);

extern vector <string> listdir(string const &);

extern bool rmfile(string const &);

extern bool rmtree(string const &);

extern bool rmdir(string const &);

extern string absolute(string const &);

NLUA_END

#endif
