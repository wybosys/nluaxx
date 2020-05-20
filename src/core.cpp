#include "core.h"

NLUA_BEGIN

int Exec() {
    // 等待退出
    string str;
    while (true) {
        cin >> str;
        if (str == "q")
            break;
    }
    return 0;
}

NLUA_END
