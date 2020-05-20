#include "core.h"
#include "stringt.h"

NLUA_BEGIN

vector<std::string> explode(string const &str, string const &delimeter) {
    vector<std::string> ret;
    int sid = 0, eid = 0;
    while ((eid = str.find(delimeter, sid)) < str.size()) {
        string val = str.substr(sid, eid - sid);
        ret.push_back(val);
        sid = eid + delimeter.size();
    }
    if (sid < str.size()) {
        string val = str.substr(sid);
        ret.push_back(val);
    }
    return ret;
}

string implode(vector<string> const &str, string const &delimeter) {
    string r;
    for (auto &i:str)
        r += delimeter + i;
    return r;
}

bool endwith(string const &str, string const &tgt) {
    if (str.length() < tgt.length())
        return false;
    return str.substr(str.length() - tgt.length(), tgt.length()) == tgt;
}

NLUA_END
