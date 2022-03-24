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
    File:       QTSSDictionary.h

    Contains:   Definitions of two classes: QTSSDictionary and QTSSDictionaryMap.
                Collectively, these classes implement the "dictionary" APIs in QTSS
                API. A QTSSDictionary corresponds to a QTSS_Object,
                a QTSSDictionaryMap corresponds to a QTSS_ObjectType.

定义了两个类: QTSSDictionary以及QTSSDictionaryMap. 这两个类实现了QTSS API的字典API部分.
QTSSDictionary对应于QTSS_Object; QTSSDictionaryMap对应于QTSS_ObjectType
    Created: Tue, Mar 2, 1999 @ 4:23 PM
*/



#ifndef _QTSSDICTIONARY_H_
#define _QTSSDICTIONARY_H_

#include <stdlib.h>
#include "SafeStdLib.h"
#include "QTSS.h"
#include "OSHeaders.h"
#include "OSMutex.h"
#include "StrPtrLen.h"
#include "MyAssert.h"
#include "QTSSStream.h"

class QTSSDictionary;
class QTSSDictionaryMap;
class QTSSAttrInfoDict;

#define __DICTIONARY_TESTING__ 0

//
// Function prototype for attr functions
typedef void* (*QTSS_AttrFunctionPtr)(QTSSDictionary* , UInt32* );

class QTSSDictionary : public QTSSStream
{
    public:
    
        //
        // CONSTRUCTOR / DESTRUCTOR
        
        QTSSDictionary(QTSSDictionaryMap* inMap, OSMutex* inMutex = NULL);
        virtual ~QTSSDictionary();
        
        //
        // QTSS API CALLS
        
        // Flags used by internal callers of these routines
        enum
        {
            kNoFlags = 0,
            kDontObeyReadOnly = 1,
            kDontCallCompletionRoutine = 2
        };
        
        // 该函数copy元素到调用者提供的Buffer
        // Returns:     QTSS_BadArgument, QTSS_NotPreemptiveSafe (if attribute is not preemptive safe),
        //              QTSS_BadIndex (if inIndex is bad)
        QTSS_Error GetValue(QTSS_AttributeID inAttrID, UInt32 inIndex, void* ioValueBuffer, UInt32* ioValueLen);


        //返回指向属性的内部缓存，只有预取安全时才可以使用
        //
        // Returns:     Same as above, but also QTSS_NotEnoughSpace, if value is too big for buffer.
        QTSS_Error GetValuePtr(QTSS_AttributeID inAttrID, UInt32 inIndex, void** outValueBuffer, UInt32* outValueLen)
                        { return GetValuePtr(inAttrID, inIndex, outValueBuffer, outValueLen, false); }
        
        // This version of GetValue converts the value to a string before returning it. Memory for
        // the string is allocated internally.
        //
        // Returns: QTSS_BadArgument, QTSS_BadIndex, QTSS_ValueNotFound
        QTSS_Error GetValueAsString(QTSS_AttributeID inAttrID, UInt32 inIndex, char** outString);
        
        // Returns:     QTSS_BadArgument, QTSS_ReadOnly (if attribute is read only),
        //              QTSS_BadIndex (attempt to set indexed parameter with param retrieval)
		//tobe  把索引的参数设置为特定数据
        QTSS_Error SetValue(QTSS_AttributeID inAttrID, UInt32 inIndex,
                            const void* inBuffer,  UInt32 inLen, UInt32 inFlags = kNoFlags);
		//tobe 与SetValue的区别何在???
        // Returns:     QTSS_BadArgument, QTSS_ReadOnly (if attribute is read only),
        QTSS_Error SetValuePtr(QTSS_AttributeID inAttrID,
                            const void* inBuffer,  UInt32 inLen, UInt32 inFlags = kNoFlags);
        
        //动态创建一个QTSSDictionaryMap
		// Returns:     QTSS_BadArgument, QTSS_ReadOnly (if attribute is read only),
        QTSS_Error CreateObjectValue(QTSS_AttributeID inAttrID, UInt32* outIndex,
                                        QTSSDictionary** newObject, QTSSDictionaryMap* inMap = NULL, 
                                        UInt32 inFlags = kNoFlags);
        
		//tobe删除指定位置的值
        // Returns:     QTSS_BadArgument, QTSS_ReadOnly, QTSS_BadIndex
        QTSS_Error RemoveValue(QTSS_AttributeID inAttrID, UInt32 inIndex, UInt32 inFlags = kNoFlags);
        
        // Utility routine used by the two external flavors of GetValue
        QTSS_Error GetValuePtr(QTSS_AttributeID inAttrID, UInt32 inIndex,
                                            void** outValueBuffer, UInt32* outValueLen,
                                            Bool16 isInternal);

        //
        // ACCESSORS
        
        QTSSDictionaryMap*  GetDictionaryMap() { return fMap; }
        
        // Returns the Instance dictionary map for this dictionary. This may return NULL
        // if there are no instance attributes in this dictionary
        QTSSDictionaryMap*  GetInstanceDictMap() { return fInstanceMap; }
        
        // Returns the number of values associated with a given attribute
        UInt32              GetNumValues(QTSS_AttributeID inAttrID);  //获取属性ID的属性数
        //设置属性ID对应的属性数，该函数只可用于减少属性数
		void                SetNumValues(QTSS_AttributeID inAttrID, UInt32 inNumValues);  
        
        // Meant only for internal server use. Does no error checking,
        // doesn't invoke the param retrieval function.
        StrPtrLen*  GetValue(QTSS_AttributeID inAttrID) 
                    {   return &fAttributes[inAttrID].fAttributeData;   }
                    
        OSMutex*    GetMutex() { return fMutexP; }
		
		void		SetLocked(Bool16 inLocked) { fLocked = inLocked; }
		Bool16		IsLocked() { return fLocked; }

        //
        // GETTING ATTRIBUTE INFO
        QTSS_Error GetAttrInfoByIndex(UInt32 inIndex, QTSSAttrInfoDict** outAttrInfoDict);
        QTSS_Error GetAttrInfoByName(const char* inAttrName, QTSSAttrInfoDict** outAttrInfoDict);
        QTSS_Error GetAttrInfoByID(QTSS_AttributeID inAttrID, QTSSAttrInfoDict** outAttrInfoDict);
        

        //
        // INSTANCE ATTRIBUTES
        //tobe增加实例属性
        QTSS_Error  AddInstanceAttribute(   const char* inAttrName,
                                            QTSS_AttrFunctionPtr inFuncPtr,
                                            QTSS_AttrDataType inDataType,
                                            QTSS_AttrPermission inPermission );
                                            
        QTSS_Error  RemoveInstanceAttribute(QTSS_AttributeID inAttr);
        //
        // MODIFIERS

        // 服务器用以下函数构建字典属性，函数不做错误检查

        // 函数不设置fNumAttributes & fAllocatedInternally.
        void    SetVal(QTSS_AttributeID inAttrID, void* inValueBuffer, UInt32 inBufferLen);
        void    SetVal(QTSS_AttributeID inAttrID, StrPtrLen* inNewValue)
                    { this->SetVal(inAttrID, inNewValue->Ptr, inNewValue->Len); }

        // 如果你不想给属性分配存贮空间
        void    SetEmptyVal(QTSS_AttributeID inAttrID, void* inBuf, UInt32 inBufLen);
        
#if __DICTIONARY_TESTING__
        static void Test(); // API test for these objects
#endif

    protected:
    
        // Derived classes can provide a completion routine for some dictionary functions
        virtual void    RemoveValueComplete(UInt32 /*inAttrIndex*/, QTSSDictionaryMap* /*inMap*/, UInt32 /*inValueIndex*/) {}
        
        virtual void    SetValueComplete(UInt32 /*inAttrIndex*/, QTSSDictionaryMap* /*inMap*/,
                                    UInt32 /*inValueIndex*/,  void* /*inNewValue*/, UInt32 /*inNewValueLen*/) {}
        virtual void    RemoveInstanceAttrComplete(UInt32 /*inAttrindex*/, QTSSDictionaryMap* /*inMap*/) {}
        
		//实际上调用构造函数
        virtual QTSSDictionary* CreateNewDictionary(QTSSDictionaryMap* inMap, OSMutex* inMutex);

    private:
    
        struct DictValueElement
        {// 该结构存贮了每个属性值的所有必要信息
            
            DictValueElement() :    fAllocatedLen(0), fNumAttributes(0),
                                    fAllocatedInternally(false), fIsDynamicDictionary(false) {}
                                    
            // Does not delete! You Must call DeleteAttributeData for that
            ~DictValueElement() {}
            
            StrPtrLen   fAttributeData;		// The data
            UInt32      fAllocatedLen;		// How much space do we have allocated?
            UInt32      fNumAttributes;		// If this is an iterated attribute, how many?
            Bool16      fAllocatedInternally;	//Should we delete this memory?
            Bool16      fIsDynamicDictionary;	//is this a dictionary object?
        };
        
        DictValueElement*   fAttributes;		//对象属性: 指向一定属性数量的存贮区，如果inAttrID & 0x80000000 =1。 
        DictValueElement*   fInstanceAttrs;		//实例属性:, 如果inAttrID & 0x80000000 =0。
        UInt32              fInstanceArraySize;	//实例属性数组大小
        QTSSDictionaryMap*  fMap;				//指向构造函数传入的QTSSDictionaryMap对象
        QTSSDictionaryMap*  fInstanceMap;		//实例属性:

        OSMutex*            fMutexP;			//可以是类内定义的OSMutex，也可在构造函数传入
		Bool16				fMyMutex;			//如果使用的是类内定义的fMyMutex，则TRUE
		Bool16				fLocked;			//该属性没有使用
        
        void DeleteAttributeData(DictValueElement* inDictValues, UInt32 inNumValues);
};


//该类为属性值的类，相关值在void QTSSDictionaryMap::SetAttribute(..)中设置
class QTSSAttrInfoDict : public QTSSDictionary
{
    public:
    
        struct AttrInfo
        {
            // 字典属性的相关信息
            char                    fAttrName[QTSS_MAX_ATTRIBUTE_NAME_SIZE + 1];	//属性名
            QTSS_AttrFunctionPtr    fFuncPtr;										//回调函数, 如果对应来取属性时, 调用该回调函数
            QTSS_AttrDataType       fAttrDataType;									//数据类型
            QTSS_AttrPermission     fAttrPermission;								//访问权限
        };

        QTSSAttrInfoDict();
        virtual ~QTSSAttrInfoDict();
        
    private:
        
        AttrInfo fAttrInfo;					//每个类有所不同，各类可以设置自已的属性
        QTSS_AttributeID fID;				//该属性在QTSSDictionaryMap的fAttrArray数组的下标，最高位为1表示实例属性
        
        static AttrInfo sAttributes[];		//静态结构，所有属性共享, 定义了属性的构成
        
        friend class QTSSDictionaryMap;

};

//QTSSAttrInfoDict::AttrInfo  QTSSAttrInfoDict::sAttributes[] =
//{
//    /* 0 */ { "qtssAttrName",       NULL,       qtssAttrDataTypeCharArray,  qtssAttrModeRead | qtssAttrModePreempSafe },
//    /* 1 */ { "qtssAttrID",         NULL,       qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe },
//    /* 2 */ { "qtssAttrDataType",   NULL,       qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe },
//    /* 3 */ { "qtssAttrPermissions",NULL,       qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe }
//};

//每个对象一个字典映射QTSSDictionaryMap，每个属性一个QTSSAttrInfoDict
class QTSSDictionaryMap
{
    public:
    
        // 必须在使用QTSSDictionary或QTSSDictionaryMap 函数前调用该函数
		// 该函数创建所有的静态字典类型(共有15种)，其中qtssAttrInfoNumParams是关于属性信息的字典类型，
		// 应该先初始化
		// 函数内部调用了QTSSDictionaryMap的构造函数
        static void Initialize();
        
        //
		// 存贮所有属性的元信息
		//
        
        // 构造函数使用的标志，做什么用?
        enum
        {
            kNoFlags        = 0,
            kAllowRemoval   = 1,
            kIsInstanceMap  = 2,
            kInstanceAttrsAllowed = 4,
            kCompleteFunctionsAllowed = 8
        };
        
        //
        // CONSTRUCTOR / DESTRUCTOR
        //函数带两个参数，该函数申请一定数量属性所需的空间, 并初始化成员变量
        QTSSDictionaryMap(UInt32 inNumReservedAttrs, UInt32 inFlags = kNoFlags);  
        ~QTSSDictionaryMap(){ delete fAttrArray; }

        //
        // QTSS API CALLS
                
        // All functions either return QTSS_BadArgument or QTSS_NoErr
        QTSS_Error      AddAttribute(   const char* inAttrName,
                                        QTSS_AttrFunctionPtr inFuncPtr,
                                        QTSS_AttrDataType inDataType,
                                        QTSS_AttrPermission inPermission );
                                        
        //
        // Marks this attribute as removed
        QTSS_Error  RemoveAttribute(QTSS_AttributeID inAttrID);
        QTSS_Error  UnRemoveAttribute(QTSS_AttributeID inAttrID);
        QTSS_Error  CheckRemovePermission(QTSS_AttributeID inAttrID);

        //
        // Searching / Iteration. These never return removed attributes
        QTSS_Error  GetAttrInfoByName(const char* inAttrName, QTSSAttrInfoDict** outAttrInfoDict, Bool16 returnRemovedAttr = false);
        QTSS_Error  GetAttrInfoByID(QTSS_AttributeID inID, QTSSAttrInfoDict** outAttrInfoDict);
        QTSS_Error  GetAttrInfoByIndex(UInt32 inIndex, QTSSAttrInfoDict** outAttrInfoDict);
        QTSS_Error  GetAttrID(const char* inAttrName, QTSS_AttributeID* outID);
        
        //
        // PRIVATE ATTR PERMISSIONS
        enum
        {
            qtssPrivateAttrModeRemoved = 0x80000000
        };

        //
        // CONVERTING attribute IDs to array indexes. Returns -1 if inAttrID doesn't exist
		// 该函数主要判断属性下标是否合法，传入的参数为第几个属性
        inline SInt32                   ConvertAttrIDToArrayIndex(QTSS_AttributeID inAttrID);

        static Bool16           IsInstanceAttrID(QTSS_AttributeID inAttrID)
            { return (inAttrID & 0x80000000) != 0; }

        // ACCESSORS
        
        // These functions do no error checking. Be careful.
        
        // Includes removed attributes
        UInt32          GetNumAttrs()           { return fNextAvailableID; }
        UInt32          GetNumNonRemovedAttrs() { return fNumValidAttrs; }
        
        Bool16                  IsPreemptiveSafe(UInt32 inIndex) 
            { Assert(inIndex < fNextAvailableID); return (Bool16) (fAttrArray[inIndex]->fAttrInfo.fAttrPermission & qtssAttrModePreempSafe); }

        Bool16                  IsWriteable(UInt32 inIndex) 
            { Assert(inIndex < fNextAvailableID); return (Bool16) (fAttrArray[inIndex]->fAttrInfo.fAttrPermission & qtssAttrModeWrite); }
		
		Bool16                  IsCacheable(UInt32 inIndex) 
            { Assert(inIndex < fNextAvailableID); return (Bool16) (fAttrArray[inIndex]->fAttrInfo.fAttrPermission & qtssAttrModeCacheable); }

        Bool16                  IsRemoved(UInt32 inIndex) 
            { Assert(inIndex < fNextAvailableID); return (Bool16) (fAttrArray[inIndex]->fAttrInfo.fAttrPermission & qtssPrivateAttrModeRemoved) ; }

        QTSS_AttrFunctionPtr    GetAttrFunction(UInt32 inIndex)
            { Assert(inIndex < fNextAvailableID); return fAttrArray[inIndex]->fAttrInfo.fFuncPtr; }
            
        char*                   GetAttrName(UInt32 inIndex)
            { Assert(inIndex < fNextAvailableID); return fAttrArray[inIndex]->fAttrInfo.fAttrName; }
            
        QTSS_AttributeID        GetAttrID(UInt32 inIndex)
            { Assert(inIndex < fNextAvailableID); return fAttrArray[inIndex]->fID; }

        QTSS_AttrDataType       GetAttrType(UInt32 inIndex)
            { Assert(inIndex < fNextAvailableID); return fAttrArray[inIndex]->fAttrInfo.fAttrDataType; }
        
        Bool16                  InstanceAttrsAllowed() { return (Bool16) (fFlags & kInstanceAttrsAllowed); }
        Bool16                  CompleteFunctionsAllowed() { return (Bool16) (fFlags & kCompleteFunctionsAllowed) ; }

        // MODIFIERS
        
		//为fAttrArray[inID] = NEW QTSSAttrInfoDict 申请空间，并填入值;
        
        void        SetAttribute(   QTSS_AttributeID inID, 
                                    const char* inAttrName,
                                    QTSS_AttrFunctionPtr inFuncPtr,
                                    QTSS_AttrDataType inDataType,
                                    QTSS_AttrPermission inPermission );

        
        //
        // DICTIONARY MAPS
        
        // All dictionary maps are stored here, and are accessable
        // through these routines
        
        // This enum allows all QTSSDictionaryMaps to be stored in an array 
        enum
        {
            kServerDictIndex                = 0,	//qtssServerObjectType, 服务器参数, 用于保存服务器的成员变量
            kPrefsDictIndex                 = 1,	//qtssPrefsObjectType, 设置参数, 用于保存QTSServerPrefs中的成员变量
            kTextMessagesDictIndex          = 2,	//qtssTextMessagesObjectType
            kServiceDictIndex               = 3,
            
            kRTPStreamDictIndex             = 4,	//qtssRTPStreamObjectType, 
            kClientSessionDictIndex         = 5,	//qtssClientSessionObjectType
            kRTSPSessionDictIndex           = 6,	//qtssRTSPSessionObjectType
            kRTSPRequestDictIndex           = 7,	//qtssRTSPRequestObjectType
            kRTSPHeaderDictIndex            = 8,	//qtssRTSPHeaderObjectType
            kFileDictIndex                  = 9,	//qtssFileObjectType
            kModuleDictIndex                = 10,	//qtssModuleObjectType
            kModulePrefsDictIndex           = 11,	//qtssModulePrefsObjectType
            kAttrInfoDictIndex              = 12,	//qtssAttrInfoObjectType	描述属性信息的对象
            kQTSSUserProfileDictIndex       = 13,	//qtssUserProfileObjectType
            kQTSSConnectedUserDictIndex     = 14,	//qtssConnectedUserObjectType

            kNumDictionaries                = 15,
            
            kNumDynamicDictionaryTypes      = 500,
            kIllegalDictionary              = kNumDynamicDictionaryTypes + kNumDictionaries
        };
        
        //把QTSS_ObjectType对象类型转化为字典索引
        static UInt32                   GetMapIndex(QTSS_ObjectType inType);    //

        // Using one of the above predefined indexes, this returns the corresponding map
        static QTSSDictionaryMap*       GetMap(UInt32 inIndex)
            { Assert(inIndex < kNumDynamicDictionaryTypes + kNumDictionaries); return sDictionaryMaps[inIndex]; }

        static QTSS_ObjectType          CreateNewMap();    

    private:

        //
        // dictionary maps知识库，注意以下的成员变量的静态变量
        //

		//sDictionaryMaps为指向字典的指针，所有的其它对象使用该字典，每个对象对应数组的一个元素, 
		//sDictionaryMaps前面部分是静态分析的, 后面部分是动态使用的
        static QTSSDictionaryMap*       sDictionaryMaps[kNumDictionaries + kNumDynamicDictionaryTypes];
		//存贮下一个动态映射，也即下一部字典的索引，默认kNumDictionaries;
        static UInt32                   sNextDynamicMap;
            
        enum
        {
            kMinArraySize = 20			//属性数组最小长度
        };

        UInt32                          fNextAvailableID;		//属性数组的下一个下标, 增加属性时, 从下一个下标开始
        UInt32                          fNumValidAttrs;			//有效的属性数
        UInt32                          fAttrArraySize;			//属性数组大小
        QTSSAttrInfoDict**              fAttrArray;				//指向属性数组
        UInt32                          fFlags;					//构造函数使用的标志，由构造函数传入
        
        friend class QTSSDictionary;
};

// 该函数主要判断属性下标是否合法，传入的参数为第几个属性
inline SInt32   QTSSDictionaryMap::ConvertAttrIDToArrayIndex(QTSS_AttributeID inAttrID)
{
    SInt32 theIndex = inAttrID & 0x7FFFFFFF;	//theID |= 0x80000000表示实例属性
    if ((theIndex < 0) || (theIndex >= (SInt32)fNextAvailableID))
        return -1;
    else
        return theIndex;
}


#endif
