
#include <iostream>
 
std::string hedec(std::string str) {
  std::string subs[] = { "\"", "&quot;", "'", "&apos;", "&", "&amp;", "<", "&lt;", ">", "&gt;" };
  std::string reps[] = { "\"", "\"", "'", "'", "&", "&", "<", "<", ">", ">" };
	size_t found;
	
	for(int j = 0; j <= 10; j++) {
		do {
			found = str.rfind(subs[j]);
	  	if (found != std::string::npos) str.replace (found,subs[j].length(),reps[j]);
    } while (found != std::string::npos);
  }

  return str;
}

// Convertis toute sous-chaine de la forme '&#..;' ou '&#...;' en character
std::string dec_he(std::string s) {
  std::string sn, res="";
  size_t i, i2;
  int n;
  char ch;

  for(i=0; i < s.size(); i++) {
    if (s[i] == '&' && s.size() > i+1 && s[i+1] == '#') {
      sn="";
      for (i2=i+2; i2 < s.size(); i2++) {
        if (isdigit(s[i2])) sn+=s[i2];
        else if (s[i2] == ';') break;
        else {
          sn="";
          break;
        }
      }

      if (sn != "") {
        i=i2;
        n=std::stoi(sn);
        ch=char(n);
        if (isprint(ch)) res+=ch;
      }

    } else res+=s[i];
  }

  return res;
}

int main() {
  std::string ln;
  while (std::getline(std::cin, ln)) {
    std::cout << ln << std::endl;
    std::cout << '[' << hedec(ln) << "] ";
  }
  std::cout << std::endl;

  return 0;
}
