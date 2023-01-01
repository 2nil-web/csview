
#include <iostream>

#include <map>
#include <string>

#define U8

namespace emojicpp {
  static std::map<std::string, std::string> EMOJIS = {
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
          it = EMOJIS.find(s.substr(index, i - index + 1));
          if (it == EMOJIS.end()) {
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
    //std::cout << emojicpp::emojize("Emoji :smile: for c++ :+1:") << std::endl;
    for (auto e:emojicpp::EMOJIS) {
      std::cout << e.first << "=" << e.second << std::endl;
    }
    return 0;
}

