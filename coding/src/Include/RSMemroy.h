// RSMemroy.h: interface for the RSMemroy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSMEMROY_H__E047C729_2614_4DD1_94E4_A780EBB573D8__INCLUDED_)
#define AFX_RSMEMROY_H__E047C729_2614_4DD1_94E4_A780EBB573D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RSMemroy  
{
public:
	RSMemroy();
	virtual ~RSMemroy();
	//
	//Memory Allocation
};

class RSCharMemroy
{
public:
	RSCharMemroy();
	virtual ~RSCharMemroy();
	//
	//Memory Allocation

	static char* newStrMemory();
	static char* newStrMemory_SetValue(const char* inStr);		//申请内存，并设置内存值
	static void zeroStrMemory(char* pStr);
	static void releaseMemroy(char** pStr);
};

#endif // !defined(AFX_RSMEMROY_H__E047C729_2614_4DD1_94E4_A780EBB573D8__INCLUDED_)
