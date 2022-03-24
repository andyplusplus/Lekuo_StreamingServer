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
    File:       QTSSFile.h

    Contains:    
                    
 �����ģ�����, ����ģ������½�ɫQTSS_OpenFilePreProcess_Role, QTSS_OpenFile_Role, 
QTSS_CloseFile_Role, QTSS_ReadFile_Role, QTSS_AdviseFile_Role, QTSS_RequestEventFile_Role   
    
    
*/

#include "QTSSDictionary.h"
#include "QTSSModule.h"

#include "OSFileSource.h"
#include "EventContext.h"

class QTSSFile : public QTSSDictionary
{
    public:

		//�����Դ����ֵ�
        QTSSFile();	
        virtual ~QTSSFile() {}

        //�ֵ��ʼ��
        static void     Initialize();
        
        //
        // Opening & Closing
        QTSS_Error          Open(char* inPath, QTSS_OpenFileFlags inFlags);		//qtssOpenFileNoFlags, qtssOpenFileAsync, qtssOpenFileReadAhead
        void                Close();
        
        //
        // Implementation of stream functions.
        virtual QTSS_Error  Read(void* ioBuffer, UInt32 inLen, UInt32* outLen);
        
        virtual QTSS_Error  Seek(UInt64 inNewPosition);
        
        virtual QTSS_Error  Advise(UInt64 inPosition, UInt32 inAdviseSize);
        
        virtual QTSS_Error  RequestEvent(QTSS_EventType inEventMask);
        
    private:

        QTSSModule* fModule;	//��Ӧ��ģ��	qtssFlOjFileSysModuleName ����ǰ�ļ�������ļ�ϵͳģ�����ơ�
        UInt64      fPosition;	//��ǰλ��	qtssFlObjPosition �ļ�ָ��ĵ�ǰλ�ã����ļ�����ʼ���ֿ�ʼ����Ϊ���ֽڣ������ֽڼơ�
        QTSSFile*   fThisPtr;	//�ļ�ָ��		qtssFlObjStream ��ǰ�ļ������������
        
        //
        // File attributes
        UInt64      fLength;	//�ļ�����     qtssFlObjLength �ļ��ĳ��ȣ����ֽڼơ�
        time_t      fModDate;	//�޸�����    qtssFlObjModDate �ļ����һ�α��޸ĵ����ں�ʱ�䡣

        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
};

