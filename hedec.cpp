
#include <windows.h>

#include <iostream>
#include <string>
#include <vector>
 
#include "ent_sym.h"

// Retourne la chaine passée en paramétre en remplaçant toute ses sous-chaines étant une entité html symbole ou nombre par son équivalent caractére
// Exemple si il existe une ou plusieurs sous-chaine &#39; alors elle sera remplacé par ' (apostrophe)
// Exemple si il existe une ou plusieurs sous-chaine &apos; alors elle sera remplacé par ' (apostrophe)
std::string replace_html_all_entities(std::string s) {
  std::string sn;
  size_t i, i2, found;
  int n;
  char ch;
  std::string sym_srch[] = { "\"", "&quot;", "'", "&apos;", "&", "&amp;", "<", "&lt;", ">", "&gt;" };
  std::string sym_repl[] = { "\"", "\"", "'", "'", "&", "&", "<", "<", ">", ">" };

  // Remplace les entités symbole, s'il y en a ...
  for(i=0; !sym_srch[i].empty(); i++) {
    do {
      found=s.rfind(sym_srch[i]);
      if (found != std::string::npos) {
        s.replace(found, sym_srch[i].length(), sym_repl[i]);
      }
    } while (found != std::string::npos);
  }

  // Remplace les entités nombres, s'il y en a ...
  for(i=0; i < s.size(); i++) {
    // Quand on rencontre & (ampersand) alors ça peut-être une entité html nombre, donc on test
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

      // On a trouvé une entité nombre
      if (sn != "") {
        i=i2;
        n=std::stoi(sn);
        ch=char(n);
        /*if (isprint(ch)) */s.replace(i, i2-i, std::string(&ch));
      }
    }
  }

  return s;
}

std::wstring ws(const std::string &s) {
    std::wstring res(s.begin(), s.end());
    return res;
}

#ifdef _MSC_VER
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#else
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR /*lpCmdLine*/ , int nCmdShow)
#endif
{
  std::wstring s=L"";
  int i=1; for (auto &es : ent_sym) {
    s+=std::to_wstring(i)+L'('+std::wstring(&es.first)+L','+ws(es.second)+L')';
    if (i%4) s+=L','; 
    else s+=L'\n'; 
    i++;
  }
  s+=L'\n';
  MessageBoxW(NULL, s.c_str(), L"Titre", MB_OK);
  return 0;

  std::string ln;

  while (std::getline(std::cin, ln)) {
    std::cout << ln << std::endl;
    std::cout << '[' << replace_html_all_entities(ln) << "] ";
  }
  std::cout << std::endl;

  return 0;
}

