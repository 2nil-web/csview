
#ifndef REGHANDLE_H
#define REGHANDLE_H

#include <string>

std::string GetRegString(const std::string subKey, const std::string var, const std::string defval);
std::string GetRegExpString(const std::string subKey, const std::string var, const std::string defval);

int GetRegInt(const std::string subKey, const std::string var, const int defval);
bool GetRegBin(const std::string subKey, const std::string var, DWORD size, BYTE *val);
int *GetRegIntArray(const std::string subKey, const std::string var, int *size);

void PutRegString(const std::string subKey, const std::string var, std::string val);
void PutRegInt(const std::string subKey, const std::string var, DWORD val);
void PutRegIntArray(const std::string subKey, const std::string var, int size, int *val);
void PutRegBin(const std::string subKey, const std::string var, DWORD size, CONST BYTE *val);

#endif /* REGHANDLE_H */

