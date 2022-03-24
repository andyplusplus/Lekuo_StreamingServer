//#include "IVFile_Index.h"
//Index 版本的相关结构定义


#if !defined(AFX_IVFILE_Index_H_INCLUDED)
#define AFX_IVFILE_Index_H_INCLUDED

#define IndexFileVersion_1__DataFileExt  ".dat"
#define IndexFileVersion_1__IndexFileExt  ".idx"

struct ST_IndexFileVersion_1
{
	struct ST_IndexItem
	{
		unsigned long TimeOffSet;
		unsigned long FileOffSet;
	};

	enum{
		INDEX_FILE_HEAD_SIZE = 512,
		INDEX_FILE_FirstData_Loc = INDEX_FILE_HEAD_SIZE + sizeof(ST_IndexItem),
		FORMAT_Version = 1,

	};

};



#endif // !defined(AFX_IVFILE_Index_H_INCLUDED)
