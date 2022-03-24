#if !defined(LK_Mutex_H_INCLUDED_)
#define LK_Mutex_H_INCLUDED_


enum IMutex_Results
{
	IMutex_Results_NotExist = 0,	//if mutex is null, return this value
	IMutex_Results_Succeed = 1		//if mutex is success locked, return this value
};


class IMutex
{
public:
	IMutex()					{	::InitializeCriticalSection(&fMutex);	};
	~IMutex()					{	::DeleteCriticalSection(&fMutex);		};
	IMutex_Results Lock()		
	{	
		::EnterCriticalSection(&fMutex);
		return IMutex_Results_Succeed;
	};
	
	IMutex_Results UnLock()		
	{	
		::LeaveCriticalSection(&fMutex);
		return IMutex_Results_Succeed;
	};

private:
	CRITICAL_SECTION fMutex;
};
typedef IMutex* LPIMutex;


class IMutexLocker
{
public:

	IMutexLocker(LPIMutex inMutexP) : fMutex(inMutexP) { if (fMutex != NULL) fMutex->Lock(); }
	~IMutexLocker() {  if (fMutex != NULL) fMutex->UnLock(); }
	void Lock()         { if (fMutex != NULL) fMutex->Lock(); }
	void Unlock()       { if (fMutex != NULL) fMutex->UnLock(); }
private:
    LPIMutex  fMutex;
};


#endif // !defined(LK_Mutex_H_INCLUDED_)
