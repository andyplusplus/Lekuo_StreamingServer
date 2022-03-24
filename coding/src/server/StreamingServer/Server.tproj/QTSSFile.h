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
                    
 该类和模块关联, 处理模块的以下角色QTSS_OpenFilePreProcess_Role, QTSS_OpenFile_Role, 
QTSS_CloseFile_Role, QTSS_ReadFile_Role, QTSS_AdviseFile_Role, QTSS_RequestEventFile_Role   
    
    
*/

#include "QTSSDictionary.h"
#include "QTSSModule.h"

#include "OSFileSource.h"
#include "EventContext.h"

class QTSSFile : public QTSSDictionary
{
    public:

		//把属性存入字典
        QTSSFile();	
        virtual ~QTSSFile() {}

        //字典初始化
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

        QTSSModule* fModule;	//对应的模块	qtssFlOjFileSysModuleName 处理当前文件对象的文件系统模块名称。
        UInt64      fPosition;	//当前位置	qtssFlObjPosition 文件指针的当前位置，从文件的起始部分开始（作为０字节），以字节计。
        QTSSFile*   fThisPtr;	//文件指针		qtssFlObjStream 当前文件对象的流引用
        
        //
        // File attributes
        UInt64      fLength;	//文件长度     qtssFlObjLength 文件的长度，以字节计。
        time_t      fModDate;	//修改日期    qtssFlObjModDate 文件最近一次被修改的日期和时间。

        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
};

