/*
 *
 * APPLE_LICENSE_HEADER_START@
 *
 * Copyright c 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 the 'License'. You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */

/*
 *
对象引用类似于COM/DCOM组件编程中IUNKOWN接口功能的数据结构，它首先为每个对象建立了一个字符串形式的ID，
以便于通过这个ID找到对象（类似于QueryInterface）；另外OSRef类还为每个对象实例建立了引用计数，只有一个
对象不再被任何人引用，才可能被释放（类似于AddRef和Release）。

在OSRef.h/ OSRef.cpp中定义了以下五个主要类，
"	OSRefTableUtils，该函数只有一个成员函数HashString(StrPtrLen* inString)，用于把字符串inString哈希成整形变量。
"	OSRef: 该类是一个引用对象，包括对象的键，值，引用计数器等
"	OSRefKey: 键值的对应关系???
"	OSRefTable: OSRefTable的最主要部分，
"	OSRefReleaser

 *
 */
/*
    File:       OSRef.h

    Contains:   Class supports creating unique string IDs to object pointers. A grouping
                of an object and its string ID may be stored in an OSRefTable, and the
                associated object pointer can be looked up by string ID.
                
                Refs can only be removed from the table when no one is using the ref,
                therefore allowing clients to arbitrate access to objects in a preemptive,
                multithreaded environment. 
                    
    
    

*/

#ifndef _OSREF_H_
#define _OSREF_H_


#include "StrPtrLen.h"
#include "OSHashTable.h"
#include "OSCond.h"

class OSRefTableUtils
{
    private:

        static UInt32   HashString(StrPtrLen* inString);    

        friend class OSRef;
        friend class OSRefKey;
};

class OSRef
{
    public:

        OSRef() :   fObjectP(NULL), fRefCount(0), fNextHashEntry(NULL)
            {
#if DEBUG
                fInATable = false;
                fSwapCalled = false;
#endif          
            }

		//引用关键字，关键字对应的对象
        OSRef(const StrPtrLen &inString, void* inObjectP)
                                : fRefCount(0), fNextHashEntry(NULL)
                                    {   Set(inString, inObjectP); }
        ~OSRef() {}
        
        void Set(const StrPtrLen& inString, void* inObjectP)
            { 
#if DEBUG
                fInATable = false;
                fSwapCalled = false;
#endif          
                fString = inString; fObjectP = inObjectP;
                fHashValue = OSRefTableUtils::HashString(&fString);
            }
        
#if DEBUG
        Bool16  IsInTable()     { return fInATable; }
#endif
        void**  GetObjectPtr()  { return &fObjectP; }
        void*   GetObject()     { return fObjectP; }
        UInt32  GetRefCount()   { return fRefCount; }
        StrPtrLen *GetString()  { return &fString; }
    private:
        
        //value
        void*   fObjectP;		//引用的值
        //key
        StrPtrLen   fString;	//引用键
        
        //refcounting
        UInt32  fRefCount;		//引用计数器
#if DEBUG
        Bool16  fInATable;
        Bool16  fSwapCalled;
#endif
        OSCond  fCond;			//用于阻止等待ref的线程	//to block threads waiting for this ref.
        
        UInt32              fHashValue;			//该值由OSRefTableUtils::HashString(&fString);获得，并插入Hash表
        OSRef*              fNextHashEntry;		//下一个哈希表项
        
        friend class OSRefKey;
        friend class OSHashTable<OSRef, OSRefKey>;
        friend class OSHashTableIter<OSRef, OSRefKey>;
        friend class OSRefTable;

};


class OSRefKey
{
public:

    //CONSTRUCTOR / DESTRUCTOR:
    OSRefKey(StrPtrLen* inStringP)			//构造/析构函数
        :   fStringP(inStringP)
         { fHashValue = OSRefTableUtils::HashString(inStringP); }
            
    ~OSRefKey() {}							
    
    
    //ACCESSORS:
    StrPtrLen*  GetString()         { return fStringP; }	//读取索引串
    
    
private:

    //PRIVATE ACCESSORS:    
    SInt32      GetHashKey()        { return fHashValue; }	//读取键

    //these functions are only used by the hash table itself. This constructor will break the "Set" functions.
	//以下函数只被哈希表使用，构造函数将break "Set"函数
    OSRefKey(OSRef *elem) : fStringP(&elem->fString),
                            fHashValue(elem->fHashValue) {}
                                    
    friend int operator ==(const OSRefKey &key1, const OSRefKey &key2)
    {
        if (key1.fStringP->Equal(*key2.fStringP))
            return true;
        return false;
    }
    
    //data:
    StrPtrLen *fStringP;
    UInt32  fHashValue;

    friend class OSHashTable<OSRef, OSRefKey>;
};

typedef OSHashTable<OSRef, OSRefKey> OSRefHashTable;
typedef OSHashTableIter<OSRef, OSRefKey> OSRefHashTableIter;

class OSRefTable
{
    public:
    
        enum
        {
            kDefaultTableSize = 1193 //UInt32		//UInt32，初始哈希表的大小 
        };
    
        //tableSize doesn't indicate the max number of Refs that can be added
        //(it's unlimited), but is rather just how big to make the hash table
        OSRefTable(UInt32 tableSize = kDefaultTableSize) : fTable(tableSize), fMutex() {}		//构造函数
        ~OSRefTable() {}		//析构函数
        
        //Allows access to the mutex in case you need to lock the table down
        //between operations
        OSMutex*    GetMutex()      { return &fMutex; }		//当需要在接任操作时访问mutex时
        OSRefHashTable* GetHashTable() { return &fTable; }	//返回引用哈希表
        
        //Registers a Ref in the table. Once the Ref is in, clients may resolve
        //the ref by using its string ID. You must setup the Ref before passing it
        //in here, ie., setup the string and object pointers
        //This function will succeed unless the string identifier is not unique,
        //in which case it will return QTSS_DupName
        //This function is atomic wrt this ref table.
		//在表中注册引用，一旦已经注册引用，客户可通过其string ID解析引用，传入引用前需建立Ref。
		//比如，设置字符串及对象指针。该函数成功执行，除非string ID不是唯一，此时返回QTSS_DupName。
		//该函数是原子可写OSRefTable。
        OS_Error        Register(OSRef* ref);
        
        // RegisterOrResolve
        // If the ID of the input ref is unique, this function is equivalent to
        // Register, and returns NULL.
        // If there is a duplicate ID already in the map, this funcion
        // leave it, resolves it, and returns it.
		// 如果输入ID唯一，该函数相当于Register, 并返回NULL. 如果map中已存在该ID，函数解析该ID，并返回。
        OSRef*              RegisterOrResolve(OSRef* inRef);
        
        //This function may block. You can only remove a Ref from the table
        //when the refCount drops to the level specified. If several threads have
        //the ref currently, the calling thread will wait until the other threads
        //stop using the ref (by calling Release, below)
        //This function is atomic wrt this ref table.
		//该函数可能会被block. 当refCount降到指定的level时，才可从表中删除Ref，
		//如果有线程正同时拥有该ref，调用者线程将等待其它线程停止使用该ref(通过调用下面的Release) 。
		//该函数是原子地wrt OSRefTable
        void        UnRegister(OSRef* ref, UInt32 refCount = 0);
        
        // Same as UnRegister, but guarenteed not to block. Will return
        // true if ref was sucessfully unregistered, false otherwise
		//与UnRegister相同，但不被block. 如果成功unregistered，则返回true，否则返回false
        Bool16      TryUnRegister(OSRef* ref, UInt32 refCount = 0);
        
        //Resolve. This function uses the provided key string to identify and grab
        //the Ref keyed by that string. Once the Ref is resolved, it is safe to use
        //(it cannot be removed from the Ref table) until you call Release. Because
        //of that, you MUST call release in a timely manner, and be aware of potential
        //deadlocks because you now own a resource being contended over.
        //This function is atomic wrt this ref table.
		//调用该函数后获得一个引用，此后便可安全使用，直到调用Release。该函数可原子地写OSRefTable
        OSRef*      Resolve(StrPtrLen*  inString);  //与此相对应的的Realease(...)
        
        //Release. Release a Ref, and drops its refCount. After calling this, the
        //Ref is no longer safe to use, as it may be removed from the ref table.
		//删除Ref, 并减少refCount. 调用该函数以后，Ref可能会被删除，不能再安全地使用Ref
        void        Release(OSRef*  inRef);			//与此对应的是Resolve(...)
        
        // Swap. This atomically removes any existing Ref in the table with the new
        // ref's ID, and replaces it with this new Ref. If there is no matching Ref
        // already in the table, this function does nothing.
        //
        // Be aware that this creates a situation where clients may have a Ref resolved
        // that is no longer in the table. The old Ref must STILL be UnRegistered normally.
        // Once Swap completes sucessfully, clients that call resolve on the ID will get
        // the new OSRef object.
		//该函数自动从表中删除现有的Ref，并以新的Ref替代。如果表中没有匹配的Ref，该函数不做任何操作
        void        Swap(OSRef* newRef);
        
        UInt32      GetNumRefsInTable() { UInt64 result =  fTable.GetNumEntries(); Assert(result < kUInt32_Max); return (UInt32) result; }
        
    private:
    
        
        //all this object needs to do its job is an atomic hashtable
		//该对象所做的工作是实现一个自动哈希表
        OSRefHashTable  fTable;
        OSMutex         fMutex;
};


class OSRefReleaser
{
    public:

        OSRefReleaser(OSRefTable* inTable, OSRef* inRef) : fOSRefTable(inTable), fOSRef(inRef) {}
        ~OSRefReleaser() { fOSRefTable->Release(fOSRef); }
        
        OSRef*          GetRef() { return fOSRef; }
        
    private:

        OSRefTable*     fOSRefTable;
        OSRef*          fOSRef;
};



#endif //_OSREF_H_
