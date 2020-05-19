#include "core.h"
#include "fs.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iterator>
#include <dirent.h>

NLUA_BEGIN

#if NLUA_SHELL
path path::getcwd() {
    return path(get_current_dir_name());
}
#endif

bool path::exists(path const &p) {
    struct stat st = {0};
    return stat(p.c_str(), &st) == 0;
}

path path::absolute(path const &p) {
    char buf[FILENAME_MAX];
    char const *r = realpath(p.c_str(), buf);
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

path_iterator::path_iterator(string const &p) {
    dir = opendir(p.c_str());
}

path_iterator::~path_iterator() {
    closedir((DIR *) dir);
    dir = nullptr;
}

path_iterator::iterator path_iterator::begin() {
    iterator r;

    // 跳掉 . 和 ..
    r.entry = readdir((DIR *) dir);
    r.entry = readdir((DIR *) dir);
    r.entry = readdir((DIR *) dir);

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
    entry = readdir((DIR *) dir);
    _update();
    return *this;
}

void path_iterator::iterator::_update() {
    if (entry) {
        auto ent = (struct dirent *) entry;
        if (!_path)
            _path = new path();
        *_path = path(ent->d_name);
    } else if (_path) {
        delete _path;
        _path = nullptr;
    }
}

NLUA_END
