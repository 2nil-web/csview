
#include <iostream>

#include <map>
#include <string>

namespace emojicpp {
  static std::map<std::string, std::string> emojis = {
     #include "emoji_map.h"
  }; 

  std::string emojize(std::string s, bool escape=true) {
    int index = -1;
    int sLen = s.size();
    for (int i = 0; i < sLen; i++) {
      if (s[i] == *L":") {
        // check if colon is escaped
        if(escape && i!=0 && s[i-1]=='\\')
          continue;
        if (index == -1) {
          index = i;
        }
        else {
          if (i - index==1) {
            index = i;
            continue;
          }
          std::map<std::string, std::string>::iterator it;
          it = emojis.find(s.substr(index, i - index + 1));
          if (it == emojis.end()) {
            index = i;
            continue;
          }
          std::string emo = it->second;
          // replace from index to i
          //std::cout << s.substr(index, i - index + 1) << std::endl; // <---- uncomment to see what text is replaced, might be good for debugging
          s.replace(index, i - index + 1, emo);
          int goBack = i - index + 1 - emo.size();
          sLen -= goBack;
          i -= goBack;
          index = -1;
        }
      }
    }
    return s;
  }
}

int main() {
  for (auto e:emojicpp::emojis) std::cout << e.first << "=" << e.second << std::endl;
  return 0;
}

