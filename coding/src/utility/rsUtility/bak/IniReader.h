// IniReader.h
// 读写初始化文件

#ifndef INIREADER_H
#define INIREADER_H
#define INIREADER_MAX_PATH 512

class CIniReader
{
public:
	CIniReader(); 
	static CIniReader* GetIniReader();
	static CIniReader* GetIniReader(char* szFileName);
	static char* GetExePath();
	
	static void SetIniFileName(char* szFileName);

	static int ReadInteger		(char* szSection, char* szKey, int iDefaultValue);
	static long ReadLong		(char* szSection, char* szKey, const char* szDefaultValue);
	static float ReadFloat(char* szSection, char* szKey, float fltDefaultValue);
	static bool ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue);
	//static SInt64 ReadLongLong	(char* szSection, char* szKey, const char* szDefaultValue);

	//if the caller set the ioszValue to 0, the Function will allocate the memory
	static char* ReadString(char* szSection, char* szKey, const char* szDefaultValue, char* ioszValue = 0);
	static char* ReadPath(char* szSection, char* szKey, const char* szDefaultValue, char* ioszValue = 0);
	static char* ReadPathName(char* szSection, char* szKey, const char* szDefaultValue, char* ioszValue = 0);

	static void WriteInteger(char* szSection, char* szKey, int inValue);
	static void WriteString(char* szSection, char* szKey, char* inStr);
private:

	static char s_szFileName[INIREADER_MAX_PATH];
	static char s_szPathName[INIREADER_MAX_PATH];

	static int s_PathNameGeted;	
	static int s_FileNameSeted;	//是否已经设置文件名
	static CIniReader* s_IniReader;

};
#endif//INIREADER_H