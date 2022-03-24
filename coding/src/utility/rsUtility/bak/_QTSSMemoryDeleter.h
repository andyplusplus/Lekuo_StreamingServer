
/*    File:       _QTSSMemoryDeleter.h
      Contains:   Auto object for deleting memory allocated by QTSS API callbacks, such as QTSS_GetValueAsString
      删除内存的自动对象，用于删除QTSS API分配的内存，如QTSS_GetValueAsString，当该对象删除时，关联的内存自动删除
*/

#ifndef __QTSS__MEMORY_DELETER_H__
#define __QTSS__MEMORY_DELETER_H__

template <class T>
class _QTSSMemoryDeleter
{
    public:
        _QTSSMemoryDeleter(T* victim) : fT(victim)  {}
        ~_QTSSMemoryDeleter() { delete(fT); }
        
        void ClearObject() { fT = NULL; }

        void SetObject(T* victim) 
        {
            Assert(fT == NULL);
            fT = victim; 
        }
        T* GetObject() { return fT; }
        
        operator T*() { return fT; }
    
    private:
    
        T* fT;
};



template <class T>
class _QTSSMemoryPointDeleter
{
    public:
        _QTSSMemoryPointDeleter(T** victim) : fT(victim)  {}
        ~_QTSSMemoryPointDeleter() 
		{ 
			if(NULL != (*fT))
			{
				delete (*fT);
				(*fT) = NULL;
			}
		}
        
        void ClearObject() { 
			fT = NULL; 
		}
		
        void SetObject(T** victim) 
        {
            Assert(*fT == NULL);
            *fT = *victim; 
        }
        T** GetObject() { return (*fT); }
        
        operator T**() { return (*fT); }
		
    private:
		
        T** fT;
};


#endif //___QTSS_MEMORY_DELETER_H__


