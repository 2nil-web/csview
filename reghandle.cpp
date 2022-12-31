
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winreg.h>

#include "util.h"
#include "reghandle.h"

#define THE_HKEY HKEY_CURRENT_USER
#define SUBKEY "SoftWare"
#define MAX_STR 2048

HKEY RegOpen(const wchar_t *subKey)
{
  HKEY hkRes;
	LONG ret;
  DWORD dwDisp;

  ret=RegCreateKeyEx(THE_HKEY, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hkRes, &dwDisp);
	if (ret != ERROR_SUCCESS) std::cerr << "RegCreateKeyEx" << std::endl;

  return hkRes;
}

HKEY RegOpen(const char *subKey) { return RegOpen(s2ws(subKey).c_str()); }

//errno_t strncpy_s(char *StrDest, size_t LenStrDest, const char *StrSource, size_t count);

void GetRegString(const wchar_t *subKey, const wchar_t *var, const char *defval, char *val, DWORD lval)
{
  HKEY hk=RegOpen(subKey);
  DWORD type;

  if (RegQueryValueEx(hk, var, NULL, &type, (LPBYTE)val, &lval) != ERROR_SUCCESS || type != REG_SZ)
    strncpy_s(val, lval, defval, lval);

	RegCloseKey(hk);
}

std::string GetRegString(const std::string subKey, const std::string var, const std::string defval) {
  std::string val("");
  HKEY hk=RegOpen(subKey.c_str());
  DWORD type, lval;

  RegQueryValueEx(hk, s2ws(var).c_str(), NULL, &type, NULL, &lval);

  if (lval > 0 && type == REG_SZ) {
    BYTE *bval=new BYTE[lval];

    RegQueryValueEx(hk, s2ws(var).c_str(), NULL, &type, bval, &lval);
    val=std::string((char *)bval);
  } else val=defval;

	RegCloseKey(hk);
  return val;
}

void GetRegExpString(const wchar_t *subKey, const wchar_t *var, const char *defval, char *val, DWORD lval)
{
  HKEY hk=RegOpen(subKey);
  DWORD type=REG_EXPAND_SZ;

  if (RegQueryValueEx(hk, var, NULL, &type, (LPBYTE)val, &lval) != ERROR_SUCCESS || type != REG_EXPAND_SZ)
    strncpy_s(val, lval, defval, lval);

	RegCloseKey(hk);
}

int GetRegInt(const wchar_t *subKey, const wchar_t *var, int defval)
{
  LONG ret;
  HKEY hk=RegOpen(subKey);
  DWORD type, val, lval=sizeof(DWORD);

  ret=RegQueryValueEx(hk, var, NULL, &type, (LPBYTE)&val, &lval);
	RegCloseKey(hk);

  if (ret != ERROR_SUCCESS || type != REG_DWORD) return defval;
  return val;
}

// size contient la taille attendue ou zéro si indifférent
int GetRegBin(const wchar_t *subKey, const wchar_t *var, DWORD size, BYTE *val)
{
  HKEY hk=RegOpen(subKey);
  DWORD type, gsize;

  if (RegQueryValueEx(hk, var, NULL, &type, NULL, &gsize) != ERROR_SUCCESS || type != REG_BINARY || size != gsize) {
    RegCloseKey(hk);
    return 0;
  }

  RegQueryValueEx(hk, var, NULL, &type, val, &size);
	RegCloseKey(hk);
  return 1;
}

int *GetRegIntArray(const wchar_t *subKey, const wchar_t *var, int *size)
{
  HKEY hk=RegOpen(subKey);
  int *val, lsize=0;
  size_t i;
  char str[MAX_STR], ctx[MAX_STR], *tok;
  DWORD lstr=MAX_STR;
  DWORD type;

  if (RegQueryValueEx(hk, var, NULL, &type, (LPBYTE)str, &lstr) != ERROR_SUCCESS || type != REG_SZ) {
    *size=-1;
    return NULL;
  }

	RegCloseKey(hk);

  for(i=0; i < strlen(str); i++) if (str[i] == ' ') lsize++;
//  lsize++;

  val=(int *)malloc(lsize*sizeof(int));

  i=0;
  tok=strtok_s(str, " ", (char **)&ctx);
  while (tok) {
    val[i]=atoi(tok);
    i++;
    tok=strtok_s(NULL, " ", (char **)&ctx);
  }

  *size=(int)i;

  return val;
}

std::string GetRegExpString(const std::string subKey, const std::string var, const std::string defval) {
  std::string ret("");
  HKEY hk=RegOpen(subKey.c_str());
  DWORD type=REG_EXPAND_SZ, lval;

  if (RegQueryValueEx(hk, s2ws(var).c_str(), NULL, &type, NULL, &lval) == ERROR_SUCCESS && type == REG_EXPAND_SZ) {
    BYTE *val=new BYTE[lval];

    RegQueryValueEx(hk, s2ws(var).c_str(), NULL, &type, val, &lval);
    ret=std::string((char *)val);
  } else ret=defval;

	RegCloseKey(hk);
  return ret;
}


int GetRegInt(const std::string subKey, const std::string var, const int defval) {
  return GetRegInt(s2ws(subKey).c_str(), s2ws(var).c_str(), defval);
}

bool GetRegBin(const std::string subKey, const std::string var, DWORD size, BYTE *val) {
  return GetRegBin(s2ws(subKey).c_str(), s2ws(var).c_str(), size, val);
}

int *GetRegIntArray(const std::string subKey, const std::string var, int *size) {
  return GetRegIntArray(s2ws(subKey).c_str(), s2ws(var).c_str(), size);
}


void PutRegString(const wchar_t *subKey, const wchar_t *var, const char *val)
{
  HKEY hk=RegOpen(subKey);
  RegSetValueEx(hk, var, 0, REG_SZ, (LPBYTE)val, (DWORD)strlen(val)+1);
	RegCloseKey(hk);
}

void PutRegInt(const wchar_t *subKey, const wchar_t *var, DWORD val)
{
  HKEY hk=RegOpen(subKey);
  RegSetValueEx(hk, var, 0, REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
	RegCloseKey(hk);
}

/* Sauve un tableau d'entier comme une chaine dans la registry */
void PutRegIntArray(const wchar_t *subKey, const wchar_t *var, int size, int *val)
{
  std::string str="";
  int i;
  HKEY hk;

  for(i=0; i < size; i++) {
    if (str.size()+10 > MAX_STR) break;
    str=str+std::to_string(val[i]);
//    sprintf(str, "%s %d", str, val[i]);
  }

  hk=RegOpen(subKey);
  RegSetValueEx(hk, var, 0, REG_SZ, (LPBYTE)str.c_str(), (DWORD)str.size()+1);
	RegCloseKey(hk);
}

void PutRegBin(const wchar_t *subKey, const wchar_t *var, DWORD size, CONST BYTE *val)
{
  HKEY hk=RegOpen(subKey);
  RegSetValueEx(hk, var, 0, REG_BINARY, val, size);
	RegCloseKey(hk);
}

void PutRegString(const std::string subKey, const std::string var, const std::string val) {
  PutRegString(s2ws(subKey).c_str(), s2ws(var).c_str(), val.c_str());
}

void PutRegInt(const std::string subKey, const std::string var, DWORD val) {
  PutRegInt(s2ws(subKey).c_str(), s2ws(var).c_str(), val);
}

void PutRegIntArray(const std::string subKey, const std::string var, int size, int *val) {
  PutRegIntArray(s2ws(subKey).c_str(), s2ws(var).c_str(), size, val);
}

void PutRegBin(const std::string subKey, const std::string var, DWORD size, CONST BYTE *val) {
  PutRegBin(s2ws(subKey).c_str(), s2ws(var).c_str(), size, val);
}

