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
��������������COM/DCOM��������IUNKOWN�ӿڹ��ܵ����ݽṹ��������Ϊÿ����������һ���ַ�����ʽ��ID��
�Ա���ͨ�����ID�ҵ�����������QueryInterface��������OSRef�໹Ϊÿ������ʵ�����������ü�����ֻ��һ��
�����ٱ��κ������ã��ſ��ܱ��ͷţ�������AddRef��Release����

��OSRef.h/ OSRef.cpp�ж��������������Ҫ�࣬
"	OSRefTableUtils���ú���ֻ��һ����Ա����HashString(StrPtrLen* inString)�����ڰ��ַ���inString��ϣ�����α�����
"	OSRef: ������һ�����ö��󣬰�������ļ���ֵ�����ü�������
"	OSRefKey: ��ֵ�Ķ�Ӧ��ϵ???
"	OSRefTable: OSRefTable������Ҫ���֣�
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

		//���ùؼ��֣��ؼ��ֶ�Ӧ�Ķ���
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
        void*   fObjectP;		//���õ�ֵ
        //key
        StrPtrLen   fString;	//���ü�
        
        //refcounting
        UInt32  fRefCount;		//���ü�����
#if DEBUG
        Bool16  fInATable;
        Bool16  fSwapCalled;
#endif
        OSCond  fCond;			//������ֹ�ȴ�ref���߳�	//to block threads waiting for this ref.
        
        UInt32              fHashValue;			//��ֵ��OSRefTableUtils::HashString(&fString);��ã�������Hash��
        OSRef*              fNextHashEntry;		//��һ����ϣ����
        
        friend class OSRefKey;
        friend class OSHashTable<OSRef, OSRefKey>;
        friend class OSHashTableIter<OSRef, OSRefKey>;
        friend class OSRefTable;

};


class OSRefKey
{
public:

    //CONSTRUCTOR / DESTRUCTOR:
    OSRefKey(StrPtrLen* inStringP)			//����/��������
        :   fStringP(inStringP)
         { fHashValue = OSRefTableUtils::HashString(inStringP); }
            
    ~OSRefKey() {}							
    
    
    //ACCESSORS:
    StrPtrLen*  GetString()         { return fStringP; }	//��ȡ������
    
    
private:

    //PRIVATE ACCESSORS:    
    SInt32      GetHashKey()        { return fHashValue; }	//��ȡ��

    //these functions are only used by the hash table itself. This constructor will break the "Set" functions.
	//���º���ֻ����ϣ��ʹ�ã����캯����break "Set"����
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
            kDefaultTableSize = 1193 //UInt32		//UInt32����ʼ��ϣ��Ĵ�С 
        };
    
        //tableSize doesn't indicate the max number of Refs that can be added
        //(it's unlimited), but is rather just how big to make the hash table
        OSRefTable(UInt32 tableSize = kDefaultTableSize) : fTable(tableSize), fMutex() {}		//���캯��
        ~OSRefTable() {}		//��������
        
        //Allows access to the mutex in case you need to lock the table down
        //between operations
        OSMutex*    GetMutex()      { return &fMutex; }		//����Ҫ�ڽ��β���ʱ����mutexʱ
        OSRefHashTable* GetHashTable() { return &fTable; }	//�������ù�ϣ��
        
        //Registers a Ref in the table. Once the Ref is in, clients may resolve
        //the ref by using its string ID. You must setup the Ref before passing it
        //in here, ie., setup the string and object pointers
        //This function will succeed unless the string identifier is not unique,
        //in which case it will return QTSS_DupName
        //This function is atomic wrt this ref table.
		//�ڱ���ע�����ã�һ���Ѿ�ע�����ã��ͻ���ͨ����string ID�������ã���������ǰ�轨��Ref��
		//���磬�����ַ���������ָ�롣�ú����ɹ�ִ�У�����string ID����Ψһ����ʱ����QTSS_DupName��
		//�ú�����ԭ�ӿ�дOSRefTable��
        OS_Error        Register(OSRef* ref);
        
        // RegisterOrResolve
        // If the ID of the input ref is unique, this function is equivalent to
        // Register, and returns NULL.
        // If there is a duplicate ID already in the map, this funcion
        // leave it, resolves it, and returns it.
		// �������IDΨһ���ú����൱��Register, ������NULL. ���map���Ѵ��ڸ�ID������������ID�������ء�
        OSRef*              RegisterOrResolve(OSRef* inRef);
        
        //This function may block. You can only remove a Ref from the table
        //when the refCount drops to the level specified. If several threads have
        //the ref currently, the calling thread will wait until the other threads
        //stop using the ref (by calling Release, below)
        //This function is atomic wrt this ref table.
		//�ú������ܻᱻblock. ��refCount����ָ����levelʱ���ſɴӱ���ɾ��Ref��
		//������߳���ͬʱӵ�и�ref���������߳̽��ȴ������߳�ֹͣʹ�ø�ref(ͨ�����������Release) ��
		//�ú�����ԭ�ӵ�wrt OSRefTable
        void        UnRegister(OSRef* ref, UInt32 refCount = 0);
        
        // Same as UnRegister, but guarenteed not to block. Will return
        // true if ref was sucessfully unregistered, false otherwise
		//��UnRegister��ͬ��������block. ����ɹ�unregistered���򷵻�true�����򷵻�false
        Bool16      TryUnRegister(OSRef* ref, UInt32 refCount = 0);
        
        //Resolve. This function uses the provided key string to identify and grab
        //the Ref keyed by that string. Once the Ref is resolved, it is safe to use
        //(it cannot be removed from the Ref table) until you call Release. Because
        //of that, you MUST call release in a timely manner, and be aware of potential
        //deadlocks because you now own a resource being contended over.
        //This function is atomic wrt this ref table.
		//���øú�������һ�����ã��˺��ɰ�ȫʹ�ã�ֱ������Release���ú�����ԭ�ӵ�дOSRefTable
        OSRef*      Resolve(StrPtrLen*  inString);  //������Ӧ�ĵ�Realease(...)
        
        //Release. Release a Ref, and drops its refCount. After calling this, the
        //Ref is no longer safe to use, as it may be removed from the ref table.
		//ɾ��Ref, ������refCount. ���øú����Ժ�Ref���ܻᱻɾ���������ٰ�ȫ��ʹ��Ref
        void        Release(OSRef*  inRef);			//��˶�Ӧ����Resolve(...)
        
        // Swap. This atomically removes any existing Ref in the table with the new
        // ref's ID, and replaces it with this new Ref. If there is no matching Ref
        // already in the table, this function does nothing.
        //
        // Be aware that this creates a situation where clients may have a Ref resolved
        // that is no longer in the table. The old Ref must STILL be UnRegistered normally.
        // Once Swap completes sucessfully, clients that call resolve on the ID will get
        // the new OSRef object.
		//�ú����Զ��ӱ���ɾ�����е�Ref�������µ�Ref������������û��ƥ���Ref���ú��������κβ���
        void        Swap(OSRef* newRef);
        
        UInt32      GetNumRefsInTable() { UInt64 result =  fTable.GetNumEntries(); Assert(result < kUInt32_Max); return (UInt32) result; }
        
    private:
    
        
        //all this object needs to do its job is an atomic hashtable
		//�ö��������Ĺ�����ʵ��һ���Զ���ϣ��
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
