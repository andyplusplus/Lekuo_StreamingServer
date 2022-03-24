/*
    File:       _OSArrayObjectDeleter.h

    Contains:   Auto object for deleting arrays.



*/

#ifndef ___OS_ARRAY_OBJECT_DELETER_H__
#define ___OS_ARRAY_OBJECT_DELETER_H__

template <class T>
class _OSArrayObjectDeleter
{
    public:
        _OSArrayObjectDeleter(T* victim) : fT(victim)  {}
        ~_OSArrayObjectDeleter() { delete [] fT; }
        
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
class _OSPtrDeleter
{
    public:
        _OSPtrDeleter(T* victim) : fT(victim)  {}
        ~_OSPtrDeleter() { delete fT; }
        
        void ClearObject() { fT = NULL; }

        void SetObject(T* victim) 
        {   Assert(fT == NULL);
            fT = victim; 
        }
            
    private:
    
        T* fT;
};


typedef _OSArrayObjectDeleter<char*> _OSCharPointerArrayDeleter;
typedef _OSArrayObjectDeleter<char> _OSCharArrayDeleter;

#endif //___OS_OBJECT_ARRAY_DELETER_H__
