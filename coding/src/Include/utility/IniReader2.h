//#include "IniReader2.h"

#ifndef CINIREADER2_H
#define CINIREADER2_H
#define INIREADER2_MAX_PATH 1024

class CIniReader2
{ 
public:
	CIniReader2(const char* szFileName); 
	 
	 int ReadInteger	(const char* szSection, const char* szKey, int iDefaultValue);
	 long ReadLong		(const char* szSection, const char* szKey, const char* szDefaultValue);
	 float ReadFloat	(const char* szSection, const char* szKey, float fltDefaultValue);
	 bool ReadBoolean	(const char* szSection, const char* szKey, bool bolDefaultValue);
	 __int64 ReadLongLong	(const char* szSection, const char* szKey, const char* szDefaultValue);

	//if the caller set the ioszValue to 0, the Function will allocate the memory
	 char* ReadString(const char* szSection, const char* szKey, const char* szDefaultValue, char* ioszValue = 0);
	 char* ReadPath(const char* szSection, const char* szKey, const char* szDefaultValue, char* ioszValue = 0);
	 char* ReadPathName(const char* szSection, const char* szKey, const char* szDefaultValue, char* ioszValue = 0);

	 void WriteInteger(const char* szSection, const char* szKey, int inValue);
	 void WriteString(const char* szSection, const char* szKey, const char* inStr);

	 static char* GetExePath(char* ioPath);
private:

	 char m_szFileName[INIREADER2_MAX_PATH];
};
#endif//CINIREADER2_H