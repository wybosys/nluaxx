#ifndef __NLUA_FS_H_INCLUDED
#define __NLUA_FS_H_INCLUDED

NLUA_BEGIN

class path;

class path_iterator {
public:

class iterator : public ::std::iterator<forward_iterator_tag, path> {
    public:

        ~iterator();

        inline bool operator!=(iterator const &r) const {
            return entry != r.entry;
        }

        inline bool operator==(iterator const &r) const {
            return entry == r.entry;
        }

        iterator &operator++();

        inline path const &operator*() const {
            return *_path;
        }

        void *entry = nullptr;
        void *dir = nullptr;

    protected:

        void _update();

        path *_path = nullptr;

        friend class path_iterator;
    };

    iterator begin();

    iterator end();

    explicit path_iterator(string const &);

    ~path_iterator();

    void *dir = nullptr;
};

class path {
public:

    path() = default;

    path(string const &s) : _s(s) {}

    path(char const *s) : _s(s) {}

#if NLUA_SHELL
    // 获得当前目录
    static path getcwd();
#endif

    // 是否存在
    static bool exists(path const &);

    // 转换成绝对目录
    static path absolute(path const &);

    // 是否是文件
    static bool isfile(path const &);

    // 是否是目录
    static bool isdirectory(path const &);

    // 遍历目录
    static path_iterator walk(path const &);

    inline char const *c_str() const {
        return _s.c_str();
    }

    inline operator string const &() const {
        return _s;
    }

    inline operator char const *() const {
        return _s.c_str();
    }

    inline bool empty() const {
        return _s.empty();
    }

    path operator+(string const &) const;

    path &operator+=(string const &);

    inline bool operator==(path const &r) const {
        return _s == r._s;
    }

private:

    string _s;
};

NLUA_END

#endif
