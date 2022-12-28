
#include <windows.h>

#include <iostream>
#include <string>
#include <vector>
 
#include "ent_sym.h"

//#define trc std::cout << __LINE__ << std::endl;
#define trc 

// string to wstring
std::wstring ws(const std::string &s) {
    std::wstring res(s.begin(), s.end());
    return res;
}

// Retourne la chaine passée en paramétre en remplaçant toute ses sous-chaines étant une entité html symbole ou nombre par son équivalent caractére
// Exemple si il existe une ou plusieurs sous-chaine &#39; alors elle sera remplacé par ' (apostrophe)
// Exemple si il existe une ou plusieurs sous-chaine &apos; alors elle sera remplacé par ' (apostrophe)
size_t replace_html_all_entities(std::wstring &s, bool remove=false) {
  std::wstring sn;
  size_t i, i2, found;
  int n;
  wchar_t ch;
  int how_many=0;

  // Remplace les entités symbole, s'il y en a ...
  for (auto &es : ent_sym) {
    do {
      found=s.rfind(ws(es.second));
      if (found != std::wstring::npos) {
        if (remove) s.replace(found, es.second.length(), L"");
        else s.replace(found, es.second.length(), std::wstring(&es.first));
        how_many++;
      }
    } while (found != std::wstring::npos);
  }

  // Remplace les entités nombres, s'il y en a ...
  for(i=0; i < s.size(); i++) {
    // Quand on rencontre & (ampersand) alors ça peut-être une entité html nombre, donc on test
    if (s[i] == '&' && s.size() > i+1 && s[i+1] == '#') {
      sn=L"";

      for (i2=i+2; i2 < s.size(); i2++) {
        if (isdigit(s[i2])) sn+=s[i2];
        else if (s[i2] == ';') break;
        else {
          sn=L"";
          break;
        }
      }

      // On a trouvé une entité nombre
      if (sn != L"") {
        if (!remove) {
          n=std::stoi(sn);
          ch=wchar_t(n);
          s.replace(i, i2-i, std::wstring(&ch));
        }

        i=i2;
        how_many++;
      }
    }
  }

  return how_many;
}

std::wstring GetUserDefaultLang(LANGID &langid) {
  size_t l;
  langid=GetUserDefaultLangID();
  
  l=GetLocaleInfo(langid, LOCALE_SLANGUAGE,	NULL, 0);

  if (l > 0) {
    wchar_t *s=new wchar_t[l+1];
    GetLocaleInfo(langid, LOCALE_SLANGUAGE,	s, l);
    return std::wstring(s);
  }

  return L"";
}

#ifdef _MSC_VER
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#else
int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
  std::wstring ln=L"", res=L"";
  size_t how_many=0;

  LANGID lid;
  res+=L"UserDefaultLang "+GetUserDefaultLang(lid)+L", id "+std::to_wstring(lid)+L"\n";

  while (std::getline(std::wcin, ln)) {
    how_many += replace_html_all_entities(ln, false);
    res+=L'['+ln+L"]\n";
  }

  std::wstring title=ws("Found "+std::to_string(how_many)+" entities");
  MessageBox(NULL, res.c_str(), title.c_str(), MB_OK);
  return 0;
}

