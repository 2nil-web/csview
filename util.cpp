
#include <string>
// string to wstring
std::wstring s2ws(const std::string &s) {
    std::wstring res(s.begin(), s.end());
    return res;
}

// wstring to string
std::string ws2s(const std::wstring &s) {
    std::string res(s.begin(), s.end());
    return res;
}

