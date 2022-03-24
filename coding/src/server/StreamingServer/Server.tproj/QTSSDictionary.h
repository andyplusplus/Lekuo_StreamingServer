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

������������: QTSSDictionary�Լ�QTSSDictionaryMap. ��������ʵ����QTSS API���ֵ�API����.
QTSSDictionary��Ӧ��QTSS_Object; QTSSDictionaryMap��Ӧ��QTSS_ObjectType
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
        
        // �ú���copyԪ�ص��������ṩ��Buffer
        // Returns:     QTSS_BadArgument, QTSS_NotPreemptiveSafe (if attribute is not preemptive safe),
        //              QTSS_BadIndex (if inIndex is bad)
        QTSS_Error GetValue(QTSS_AttributeID inAttrID, UInt32 inIndex, void* ioValueBuffer, UInt32* ioValueLen);


        //����ָ�����Ե��ڲ����棬ֻ��Ԥȡ��ȫʱ�ſ���ʹ��
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
		//tobe  �������Ĳ�������Ϊ�ض�����
        QTSS_Error SetValue(QTSS_AttributeID inAttrID, UInt32 inIndex,
                            const void* inBuffer,  UInt32 inLen, UInt32 inFlags = kNoFlags);
		//tobe ��SetValue���������???
        // Returns:     QTSS_BadArgument, QTSS_ReadOnly (if attribute is read only),
        QTSS_Error SetValuePtr(QTSS_AttributeID inAttrID,
                            const void* inBuffer,  UInt32 inLen, UInt32 inFlags = kNoFlags);
        
        //��̬����һ��QTSSDictionaryMap
		// Returns:     QTSS_BadArgument, QTSS_ReadOnly (if attribute is read only),
        QTSS_Error CreateObjectValue(QTSS_AttributeID inAttrID, UInt32* outIndex,
                                        QTSSDictionary** newObject, QTSSDictionaryMap* inMap = NULL, 
                                        UInt32 inFlags = kNoFlags);
        
		//tobeɾ��ָ��λ�õ�ֵ
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
        UInt32              GetNumValues(QTSS_AttributeID inAttrID);  //��ȡ����ID��������
        //��������ID��Ӧ�����������ú���ֻ�����ڼ���������
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
        //tobe����ʵ������
        QTSS_Error  AddInstanceAttribute(   const char* inAttrName,
                                            QTSS_AttrFunctionPtr inFuncPtr,
                                            QTSS_AttrDataType inDataType,
                                            QTSS_AttrPermission inPermission );
                                            
        QTSS_Error  RemoveInstanceAttribute(QTSS_AttributeID inAttr);
        //
        // MODIFIERS

        // �����������º��������ֵ����ԣ���������������

        // ����������fNumAttributes & fAllocatedInternally.
        void    SetVal(QTSS_AttributeID inAttrID, void* inValueBuffer, UInt32 inBufferLen);
        void    SetVal(QTSS_AttributeID inAttrID, StrPtrLen* inNewValue)
                    { this->SetVal(inAttrID, inNewValue->Ptr, inNewValue->Len); }

        // ����㲻������Է�������ռ�
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
        
		//ʵ���ϵ��ù��캯��
        virtual QTSSDictionary* CreateNewDictionary(QTSSDictionaryMap* inMap, OSMutex* inMutex);

    private:
    
        struct DictValueElement
        {// �ýṹ������ÿ������ֵ�����б�Ҫ��Ϣ
            
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
        
        DictValueElement*   fAttributes;		//��������: ָ��һ�����������Ĵ����������inAttrID & 0x80000000 =1�� 
        DictValueElement*   fInstanceAttrs;		//ʵ������:, ���inAttrID & 0x80000000 =0��
        UInt32              fInstanceArraySize;	//ʵ�����������С
        QTSSDictionaryMap*  fMap;				//ָ���캯�������QTSSDictionaryMap����
        QTSSDictionaryMap*  fInstanceMap;		//ʵ������:

        OSMutex*            fMutexP;			//���������ڶ����OSMutex��Ҳ���ڹ��캯������
		Bool16				fMyMutex;			//���ʹ�õ������ڶ����fMyMutex����TRUE
		Bool16				fLocked;			//������û��ʹ��
        
        void DeleteAttributeData(DictValueElement* inDictValues, UInt32 inNumValues);
};


//����Ϊ����ֵ���࣬���ֵ��void QTSSDictionaryMap::SetAttribute(..)������
class QTSSAttrInfoDict : public QTSSDictionary
{
    public:
    
        struct AttrInfo
        {
            // �ֵ����Ե������Ϣ
            char                    fAttrName[QTSS_MAX_ATTRIBUTE_NAME_SIZE + 1];	//������
            QTSS_AttrFunctionPtr    fFuncPtr;										//�ص�����, �����Ӧ��ȡ����ʱ, ���øûص�����
            QTSS_AttrDataType       fAttrDataType;									//��������
            QTSS_AttrPermission     fAttrPermission;								//����Ȩ��
        };

        QTSSAttrInfoDict();
        virtual ~QTSSAttrInfoDict();
        
    private:
        
        AttrInfo fAttrInfo;					//ÿ����������ͬ����������������ѵ�����
        QTSS_AttributeID fID;				//��������QTSSDictionaryMap��fAttrArray������±꣬���λΪ1��ʾʵ������
        
        static AttrInfo sAttributes[];		//��̬�ṹ���������Թ���, ���������ԵĹ���
        
        friend class QTSSDictionaryMap;

};

//QTSSAttrInfoDict::AttrInfo  QTSSAttrInfoDict::sAttributes[] =
//{
//    /* 0 */ { "qtssAttrName",       NULL,       qtssAttrDataTypeCharArray,  qtssAttrModeRead | qtssAttrModePreempSafe },
//    /* 1 */ { "qtssAttrID",         NULL,       qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe },
//    /* 2 */ { "qtssAttrDataType",   NULL,       qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe },
//    /* 3 */ { "qtssAttrPermissions",NULL,       qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe }
//};

//ÿ������һ���ֵ�ӳ��QTSSDictionaryMap��ÿ������һ��QTSSAttrInfoDict
class QTSSDictionaryMap
{
    public:
    
        // ������ʹ��QTSSDictionary��QTSSDictionaryMap ����ǰ���øú���
		// �ú����������еľ�̬�ֵ�����(����15��)������qtssAttrInfoNumParams�ǹ���������Ϣ���ֵ����ͣ�
		// Ӧ���ȳ�ʼ��
		// �����ڲ�������QTSSDictionaryMap�Ĺ��캯��
        static void Initialize();
        
        //
		// �����������Ե�Ԫ��Ϣ
		//
        
        // ���캯��ʹ�õı�־����ʲô��?
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
        //�����������������ú�������һ��������������Ŀռ�, ����ʼ����Ա����
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
		// �ú�����Ҫ�ж������±��Ƿ�Ϸ�������Ĳ���Ϊ�ڼ�������
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
        
		//ΪfAttrArray[inID] = NEW QTSSAttrInfoDict ����ռ䣬������ֵ;
        
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
            kServerDictIndex                = 0,	//qtssServerObjectType, ����������, ���ڱ���������ĳ�Ա����
            kPrefsDictIndex                 = 1,	//qtssPrefsObjectType, ���ò���, ���ڱ���QTSServerPrefs�еĳ�Ա����
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
            kAttrInfoDictIndex              = 12,	//qtssAttrInfoObjectType	����������Ϣ�Ķ���
            kQTSSUserProfileDictIndex       = 13,	//qtssUserProfileObjectType
            kQTSSConnectedUserDictIndex     = 14,	//qtssConnectedUserObjectType

            kNumDictionaries                = 15,
            
            kNumDynamicDictionaryTypes      = 500,
            kIllegalDictionary              = kNumDynamicDictionaryTypes + kNumDictionaries
        };
        
        //��QTSS_ObjectType��������ת��Ϊ�ֵ�����
        static UInt32                   GetMapIndex(QTSS_ObjectType inType);    //

        // Using one of the above predefined indexes, this returns the corresponding map
        static QTSSDictionaryMap*       GetMap(UInt32 inIndex)
            { Assert(inIndex < kNumDynamicDictionaryTypes + kNumDictionaries); return sDictionaryMaps[inIndex]; }

        static QTSS_ObjectType          CreateNewMap();    

    private:

        //
        // dictionary maps֪ʶ�⣬ע�����µĳ�Ա�����ľ�̬����
        //

		//sDictionaryMapsΪָ���ֵ��ָ�룬���е���������ʹ�ø��ֵ䣬ÿ�������Ӧ�����һ��Ԫ��, 
		//sDictionaryMapsǰ�沿���Ǿ�̬������, ���沿���Ƕ�̬ʹ�õ�
        static QTSSDictionaryMap*       sDictionaryMaps[kNumDictionaries + kNumDynamicDictionaryTypes];
		//������һ����̬ӳ�䣬Ҳ����һ���ֵ��������Ĭ��kNumDictionaries;
        static UInt32                   sNextDynamicMap;
            
        enum
        {
            kMinArraySize = 20			//����������С����
        };

        UInt32                          fNextAvailableID;		//�����������һ���±�, ��������ʱ, ����һ���±꿪ʼ
        UInt32                          fNumValidAttrs;			//��Ч��������
        UInt32                          fAttrArraySize;			//���������С
        QTSSAttrInfoDict**              fAttrArray;				//ָ����������
        UInt32                          fFlags;					//���캯��ʹ�õı�־���ɹ��캯������
        
        friend class QTSSDictionary;
};

// �ú�����Ҫ�ж������±��Ƿ�Ϸ�������Ĳ���Ϊ�ڼ�������
inline SInt32   QTSSDictionaryMap::ConvertAttrIDToArrayIndex(QTSS_AttributeID inAttrID)
{
    SInt32 theIndex = inAttrID & 0x7FFFFFFF;	//theID |= 0x80000000��ʾʵ������
    if ((theIndex < 0) || (theIndex >= (SInt32)fNextAvailableID))
        return -1;
    else
        return theIndex;
}


#endif
