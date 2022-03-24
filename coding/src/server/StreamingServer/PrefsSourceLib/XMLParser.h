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
 *  XMLParser.h
 *
 */


#ifndef __XMLParser_h__
#define __XMLParser_h__

#include "StringParser.h"
#include "OSQueue.h"
#include "OSFileSource.h"
#include "ResizeableStringFormatter.h"

class DTDVerifier
{
public:
    virtual bool IsValidSubtag(char* tagName, char* subTagName) = 0;
    virtual bool IsValidAttributeName(char* tagName, char* attrName) = 0;
    virtual bool IsValidAttributeValue(char* tagName, char* attrName, char* attrValue) = 0;
    virtual char* GetRequiredAttribute(char* tagName, int index) = 0;
    virtual bool CanHaveValue(char* tagName) = 0;
};

//��XMLTag���н���, ÿ��XMLTag����һ��fAttributes����, ���а�����������. ͬʱ������һЩ�ӱ�ǩ, ÿ���ӱ�ǩ����һ������
//                   
class XMLTag
{
public:
    XMLTag();
    XMLTag(char* tagName);
    ~XMLTag();

    //***����XML�ĵ�
	//	1. ����ǰ���ע��
	//	2. ����XML�ļ���ǩ����, ����XML��ǩ
	//	3. ����Tag������
	//	4. ����Tag��������Tag
    bool ParseTag(StringParser* parser, DTDVerifier* verifier, char* errorBuffer = NULL, int errorBufferSize = 0);
    
    char* GetAttributeValue(const char* attrName);
    char* GetValue() { 
		return fValue; 
	}
    char* GetTagName() { return fTag; }
    
    UInt32 GetNumEmbeddedTags() { return fEmbeddedTags.GetLength(); }
    
    XMLTag* GetEmbeddedTag(const UInt32 index = 0);
    XMLTag* GetEmbeddedTagByName(const char* tagName, const UInt32 index = 0);
    XMLTag* GetEmbeddedTagByAttr(const char* attrName, const char* attrValue, const UInt32 index = 0);
    XMLTag* GetEmbeddedTagByNameAndAttr(const char* tagName, const char* attrName, const char* attrValue, const UInt32 index = 0);
    
    void AddAttribute(char* attrName, char* attrValue);
    void RemoveAttribute(char* attrName);
    void AddEmbeddedTag(XMLTag* tag);
    void RemoveEmbeddedTag(XMLTag* tag);
    
    void SetTagName( char* name);
    void SetValue( char* value);

    //���ɴ�д���ļ���XML����
    void FormatData(ResizeableStringFormatter* formatter, UInt32 indent);

private:
	//If there is comments in parser��then consume the comments��for example <!-- ...... -->
    void ConsumeIfComment(StringParser* parser);

    char* fTag;				//��ǰ�ı�ǩ, ��SERVER
    char* fValue;			//Tag��ֵ����<VALUE>554</VALUE>��fValue = "554"
    OSQueue fAttributes;	//XML Tag�ĸ�������, ����Ԫ�ص�����ΪXMLAttribute
    OSQueue fEmbeddedTags;	//�ӱ�ǩ����
    
    OSQueueElem fElem;		//��Tag��Ӧ��Element����

    static UInt8 sNonNameMask[];        // stop when you hit a word
};

class XMLAttribute
{
public:
    XMLAttribute();
    ~XMLAttribute();
    
    char* fAttrName;
    char* fAttrValue;
    
    OSQueueElem fElem;
};

class XMLParser
{
public:
    XMLParser( char* inPath, DTDVerifier* verifier = NULL);
    ~XMLParser();
    
    // Check for existence, man.
    Bool16  DoesFileExist();			//ָ�����ļ��Ƿ����
    Bool16  DoesFileExistAsDirectory();	//�ļ��Ƿ���Ŀ¼����
    Bool16  CanWriteFile();				//�Ƿ��д

	//���ļ�����XML�ļ�, ��Ҫ�������²���
	// 1. ���ļ�, �����ļ����������ڴ�fileData, ���ļ���������
	// 2. �����ݹ����ַ���, ���������StringParser
	// 3. ����XMLTag����fRootTag, ���øö������������ַ���
	// 4. �ر��ļ�    
    Bool16  ParseFile(char* errorBuffer = NULL, int errorBufferSize = 0);		//errorBuffer������Ĵ�����Ϣ
										//�����ļ�, ������ǩ, �ر��ļ�
    XMLTag* GetRootTag() { return fRootTag; }
    void SetRootTag(XMLTag* tag);		//fRootTag = tag
    
    void WriteToFile(char** fileHeader);	//fileHeaderָ���ַ�������, ��ÿ��һ����Ԫ�غ����"\r\n", ���Ұ�����д���ļ�
    
private:
    XMLTag*         fRootTag;	//����ǩ, XMLTag��һ���ݹ�����ݽṹ
        
    OSFileSource    fFile;		//ϵͳ�ļ�
    char*           fFilePath;	//�ļ�·��
    DTDVerifier*    fVerifier;	//DTDУ����
};

#endif
