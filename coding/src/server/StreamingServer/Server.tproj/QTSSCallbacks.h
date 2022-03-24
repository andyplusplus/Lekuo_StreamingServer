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
    File:       QTSSCallbacks.h

    Contains:   All the QTSS callback functions
    QTSSCallbacks
    
*/


#ifndef __QTSSCALLBACKS_H__
#define __QTSSCALLBACKS_H__

#include "QTSS.h"

class QTSSCallbacks
{
    public:
    
        // MEMORY ROUTINES
        
        static void*    QTSS_New(FourCharCode inMemoryIdentifier, UInt32 inSize);
        static void     QTSS_Delete(void* inMemory);
        
        
        // ��ú�����
        static void QTSS_Milliseconds(SInt64* outMilliseconds);
        static void QTSS_ConvertToUnixTime(SInt64* inQTSS_MilliSecondsPtr, time_t* outSecondsPtr);

        // STARTUP ROUTINES
        //�ú�������theState->curModule->AddRole(inRole);���ӽ�ɫ
        static QTSS_Error   QTSS_AddRole(QTSS_Role inRole);

        // DICTIONARY ROUTINES
        
        // DICTIONARY LOCKING
		//�ú�������((QTSSDictionary*)inDictionary)->GetMutex()->Lock()����������fLocked
        static QTSS_Error   QTSS_LockObject(QTSS_Object inDictionary);
        static QTSS_Error   QTSS_UnlockObject(QTSS_Object inDictionary);
        
        // CREATE NEW OBJECT TYPE
		//�½��ֵ����ͨ��outType���أ�QTSSDictionaryMap::CreateNewMap()
        static QTSS_Error   QTSS_CreateObjectType(QTSS_ObjectType* outType);

        // ADD ATTRIBUTE
        static QTSS_Error   QTSS_AddAttribute(QTSS_ObjectType inType, const char* inTag, void* inUnused);  //��������ĺ���
        static QTSS_Error   QTSS_AddStaticAttribute(QTSS_ObjectType inObjectType, const char* inAttrName, void* inUnused, QTSS_AttrDataType inAttrDataType);
        static QTSS_Error   QTSS_AddInstanceAttribute(QTSS_Object inObject, const char* inAttrName, void* inUnused, QTSS_AttrDataType inAttrDataType);

        // REMOVE ATTRIBUTE����̬���Բ�����ȥ
        
        static QTSS_Error   QTSS_RemoveInstanceAttribute(QTSS_Object inObject, QTSS_AttributeID inID);

        // ATTRIBUTE INFO�����������ͣ��������ƣ��������ID
        static QTSS_Error   QTSS_IDForAttr(QTSS_ObjectType inType, const char* inTag, QTSS_AttributeID* outID);

        static QTSS_Error   QTSS_GetAttrInfoByName(QTSS_Object inObject, const char* inAttrName, QTSS_Object* outAttrInfoObject);
        static QTSS_Error   QTSS_GetAttrInfoByID(QTSS_Object inObject, QTSS_AttributeID inAttrID, QTSS_Object* outAttrInfoObject);
        static QTSS_Error   QTSS_GetAttrInfoByIndex(QTSS_Object inObject, UInt32 inIndex, QTSS_Object* outAttrInfoObject);

        static QTSS_Error   QTSS_GetNumAttributes(QTSS_Object inObject, UInt32* outNumValues);

        // TYPE INFO & TYPE CONVERSIONS������ת������
        
        static QTSS_Error   QTSS_TypeToTypeString(const QTSS_AttrDataType inType, char** outTypeString);
        static QTSS_Error   QTSS_TypeStringToType( char* inTypeString, QTSS_AttrDataType* outType);
        static QTSS_Error   QTSS_StringToValue( char* inValueAsString, const QTSS_AttrDataType inType, void* ioBuffer, UInt32* ioBufSize);
        static QTSS_Error   QTSS_ValueToString( void* inValue, const UInt32 inValueLen, const QTSS_AttrDataType inType, char** outString);

        // ATTRIBUTE VALUES
        
        static QTSS_Error   QTSS_GetValuePtr (QTSS_Object inDictionary, QTSS_AttributeID inID, UInt32 inIndex, void** outBuffer, UInt32* outLen);
        static QTSS_Error   QTSS_GetValue (QTSS_Object inDictionary, QTSS_AttributeID inID, UInt32 inIndex, void* ioBuffer, UInt32* ioLen);
        static QTSS_Error   QTSS_GetValueAsString (QTSS_Object inDictionary, QTSS_AttributeID inID, UInt32 inIndex, char** outString);
        
        static QTSS_Error   QTSS_SetValue (QTSS_Object inDictionary, QTSS_AttributeID inID, UInt32 inIndex, const void* inBuffer,  UInt32 inLen);
        static QTSS_Error   QTSS_SetValuePtr (QTSS_Object inDictionary, QTSS_AttributeID inID, const void* inBuffer,  UInt32 inLen);
        static QTSS_Error   QTSS_CreateObject (QTSS_Object inDictionary, QTSS_AttributeID inID, QTSS_ObjectType inType, UInt32* outIndex, QTSS_Object* outCreatedObject);
        static QTSS_Error   QTSS_GetNumValues (QTSS_Object inObject, QTSS_AttributeID inID, UInt32* outNumValues); //���ĳһ������ض����Ե�ֵ������
        static QTSS_Error   QTSS_RemoveValue (QTSS_Object inObject, QTSS_AttributeID inID, UInt32 inIndex);

        // STREAM ROUTINES
        
        static QTSS_Error   QTSS_Write(QTSS_StreamRef inStream, void* inBuffer, UInt32 inLen, UInt32* outLenWritten, QTSS_WriteFlags inFlags);
        static QTSS_Error   QTSS_WriteV(QTSS_StreamRef inStream, iovec* inVec, UInt32 inNumVectors, UInt32 inTotalLength, UInt32* outLenWritten);
        static QTSS_Error   QTSS_Flush(QTSS_StreamRef inStream);
        static QTSS_Error   QTSS_Read(QTSS_StreamRef inRef, void* ioBuffer, UInt32 inBufLen, UInt32* outLengthRead);
        static QTSS_Error   QTSS_Seek(QTSS_StreamRef inRef, UInt64 inNewPosition);
        static QTSS_Error   QTSS_Advise(QTSS_StreamRef inRef, UInt64 inPosition, UInt32 inAdviseSize);//֪ͨ�ļ�ϵͳģ������ָ�����ֺܿ콫�ᱻ��ȡ

        // FILE SYSTEM ROUTINES
        
        static QTSS_Error   QTSS_OpenFileObject(char* inPath, QTSS_OpenFileFlags inFlags, QTSS_Object* outFileObject);
        static QTSS_Error   QTSS_CloseFileObject(QTSS_Object inFileObject);

        // SOCKET ROUTINES
        
        static QTSS_Error   QTSS_CreateStreamFromSocket(int inFileDesc, QTSS_StreamRef* outStream);
        static QTSS_Error   QTSS_DestroySocketStream(QTSS_StreamRef inStream);

        // SERVICE ROUTINES
		//QTSS_AddService�ص�����ʹָ���ķ�����Ա�����ģ����á�  
		//����ص�����ֻ����Register��ɫ�е��á�
        //
        static QTSS_Error   QTSS_AddService(const char* inServiceName, QTSS_ServiceFunctionPtr inFunctionPtr);
        static QTSS_Error   QTSS_IDForService(const char* inTag, QTSS_ServiceID* outID);    //ͨ��������������Ƶõ�����ID, �������������IDΪ��������QTSS_DoService������������serviceID����ķ���
        static QTSS_Error   QTSS_DoService(QTSS_ServiceID inID, QTSS_ServiceFunctionArgsPtr inArgs);  //���������Եõ���Ӧ������Ȼ����øú���

        //
		// RTSP ROUTINES  ''''tobe����Routing��Ҫ�����ٿ�
        //
		//����Ϣ���ӵ�һ��RTSP��ͷ�С� QTSS_AppendRTSPHeader�ص����̽���ͷ��Ϣ���ӵ�RTSP��ͷ�С�
		//�ڵ���QTSS_AppendRTSPHeader����֮�󣬿��Խ����ŵ���QTSS_SendRTSPHeaders����������������ͷ
        static QTSS_Error   QTSS_AppendRTSPHeader(QTSS_RTSPRequestObject inRef, QTSS_RTSPHeader inHeader, char* inValue, UInt32 inValueLen);
        static QTSS_Error   QTSS_SendRTSPHeaders(QTSS_RTSPRequestObject inRef);  ////����һ��RTSP��ͷ��
        //��ͻ��˷���һ��RTSP��Ӧ��QTSS_SendStandardRTSPResponse�ص����̽�һ����׼����Ӧд�뵽��inRTSPRequest����ָ�����������巢��ʲô��Ӧȡ���ڲ�ͬ����Ӧ������
		//����μ�manual��QTSS_SendStandardRTSPResponse
		static QTSS_Error   QTSS_SendStandardRTSPResponse(QTSS_RTSPRequestObject inRTSPRequest, QTSS_Object inRTPInfo, UInt32 inFlags);

        // RTP ROUTINES
        //����һ������ʹһ��ģ�������ͻ��˷���RTP���ݰ�
        static QTSS_Error   QTSS_AddRTPStream(QTSS_ClientSessionObject inClientSession, QTSS_RTSPRequestObject inRTSPRequest, QTSS_RTPStreamObject* outStream, QTSS_AddStreamFlags inFlags);
        //��ʼ������ͻ��Ự�����������
		static QTSS_Error   QTSS_Play(QTSS_ClientSessionObject inClientSession, QTSS_RTSPRequestObject inRTSPRequest, QTSS_PlayFlags inPlayFlags);
        static QTSS_Error   QTSS_Pause(QTSS_ClientSessionObject inClientSession);
        static QTSS_Error   QTSS_Teardown(QTSS_ClientSessionObject inClientSession);
        static QTSS_Error   QTSS_RefreshTimeOut(QTSS_ClientSessionObject inClientSession);
        
        // ASYNC I/O ROUTINES
        static QTSS_Error   QTSS_RequestEvent(QTSS_StreamRef inStream, QTSS_EventType inEventMask);
        static QTSS_Error   QTSS_SignalStream(QTSS_StreamRef inStream);
        static QTSS_Error   QTSS_SetIdleTimer(SInt64 inMsecToWait);
        static QTSS_Error   QTSS_SetIdleRoleTimer(SInt64 inMsecToWait);
       

        static QTSS_Error   QTSS_RequestLockedCallback();
        static Bool16       QTSS_IsGlobalLocked();
        static QTSS_Error   QTSS_UnlockGlobalLock();
        
        // AUTHENTICATION AND AUTHORIZATION ROUTINE
        static QTSS_Error   QTSS_Authenticate(const char* inAuthUserName, const char* inAuthResourceLocalPath, const char* inAuthMoviesDir, QTSS_ActionFlags inAuthRequestAction, QTSS_AuthScheme inAuthScheme, QTSS_RTSPRequestObject ioAuthRequestObject);
		static QTSS_Error	QTSS_Authorize(QTSS_RTSPRequestObject inAuthRequestObject, char** outAuthRealm, Bool16* outAuthUserAllowed);
            
        static void   QTSS_LockStdLib();
        static void   QTSS_UnlockStdLib();
    private:
    
};

#endif //__QTSSCALLBACKS_H__
