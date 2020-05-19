#include "core.h"
#include "fs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <iterator>

#ifdef WIN32
#include <fileapi.h>
#include <processenv.h>
#include <handleapi.h>
#define S_ISREG(model) ((model) & _S_IFREG)
#define S_ISDIR(model) ((model) & _S_IFDIR)
#else
#include <unistd.h>
#include <dirent.h>
#endif

NLUA_BEGIN

#if NLUA_SHELL
path path::getcwd() {
#ifdef WIN32
    char buf[FILENAME_MAX];
    GetCurrentDirectoryA(FILENAME_MAX, buf);
    return path(buf);
#else
    return path(get_current_dir_name());
#endif
}
#endif

bool path::exists(path const &p) {
    struct stat st = {0};
    return stat(p.c_str(), &st) == 0;
}

path path::absolute(path const &p) {
    char buf[FILENAME_MAX];
#ifdef WIN32
    char const* r = _fullpath(buf, p.c_str(), FILENAME_MAX);
#else
    char const *r = realpath(p.c_str(), buf);
#endif
    return r == nullptr ? path() : path(r);
}

bool path::isfile(path const &p) {
    struct stat st = {0};
    if (stat(p.c_str(), &st))
        return false;
    return S_ISREG(st.st_mode);
}

bool path::isdirectory(path const &p) {
    struct stat st = {0};
    if (stat(p.c_str(), &st))
        return false;
    return S_ISDIR(st.st_mode);
}

path path::operator+(string const &r) const {
    return _s + r;
}

path &path::operator+=(string const &r) {
    _s += r;
    return *this;
}

path_iterator path::walk(path const &p) {
    return path_iterator(p);
}

#ifdef WIN32
struct DIR {
    WIN32_FIND_DATAA fdata;
    HANDLE hfind;
};
#define pdir ((DIR*)dir)
#endif

path_iterator::path_iterator(string const &p) {
#ifdef WIN32
    dir = malloc(sizeof(DIR));
    pdir->hfind = FindFirstFileA(p.c_str(), &pdir->fdata);
    if (pdir->hfind == INVALID_HANDLE_VALUE) {
        free(dir);
        dir = nullptr;
    }
#else
    dir = opendir(p.c_str());
#endif
}

path_iterator::~path_iterator() {
#ifdef WIN32
    free(dir);
#else
    closedir((DIR *) dir);
#endif
    dir = nullptr;
}

path_iterator::iterator path_iterator::begin() {
    iterator r;

#ifdef WIN32
#else
    // 跳掉 . 和 ..
    r.entry = readdir((DIR *) dir);
    r.entry = readdir((DIR *) dir);
    r.entry = readdir((DIR *) dir);
#endif

    r.dir = dir;
    r._update();
    return r;
}

path_iterator::iterator path_iterator::end() {
    return iterator();
}

path_iterator::iterator::~iterator() {
    if (_path) {
        delete _path;
        _path = nullptr;
    }
}

path_iterator::iterator &path_iterator::iterator::operator++() {
#ifdef WIN32
#else
    entry = readdir((DIR *) dir);
#endif

    _update();
    return *this;
}

void path_iterator::iterator::_update() {
    if (entry) {
        auto ent = (struct dirent *) entry;
        if (!_path)
            _path = new path();
#ifdef WIN32
#else
        *_path = path(ent->d_name);
#endif
    } else if (_path) {
        delete _path;
        _path = nullptr;
    }
}

NLUA_END
