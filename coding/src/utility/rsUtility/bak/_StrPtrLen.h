

#ifndef ___STRPTRLEN_H__
#define ___STRPTRLEN_H__


#define STRPTRLENTESTING 0

class _StrPtrLen
{
    public:

    //CONSTRUCTORS/DESTRUCTOR
    //These are so tiny they can all be inlined
    _StrPtrLen() : Ptr(NULL), Len(0) {}
    _StrPtrLen(char* sp) : Ptr(sp), Len(sp != NULL ? strlen(sp) : 0) {}
    _StrPtrLen(char *sp, int len) : Ptr(sp), Len(len) {}

	//
	//
	//  Code Trimmed here
	//
	//  

    //This is a non-encapsulating interface. The class allows you to access its
    //data.
//
//
    void Delete() { delete [] Ptr; Ptr = NULL; Len = 0; }
    void Set(char* inPtr, int inLen) { Ptr = inPtr; Len = inLen; }
    void Set(char* inPtr) { Ptr = inPtr; Len = (inPtr) ?  ::strlen(inPtr) : 0; }

	//
	//
	//  Code Trimmed here
	//
	//  
	public:
		char*    Ptr;
		int      Len;

};



class _StrPtrLenDel : public _StrPtrLen
{

  public:
     _StrPtrLenDel() : _StrPtrLen() {}

     _StrPtrLenDel(char* sp) : _StrPtrLen(sp) {}

     _StrPtrLenDel(char *sp, int len) : _StrPtrLen(sp,len) {}

     ~_StrPtrLenDel() { Delete(); }

};

#endif // __STRPTRLEN_H__
