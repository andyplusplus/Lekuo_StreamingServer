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

//对XMLTag进行解析, 每个XMLTag包括一个fAttributes队列, 队列包含各种属性. 同时还包含一些子标签, 每个子标签又是一个队列
//                   
class XMLTag
{
public:
    XMLTag();
    XMLTag(char* tagName);
    ~XMLTag();

    //***解析XML文档
	//	1. 跳过前面的注释
	//	2. 进入XML文件标签解析, 解析XML标签
	//	3. 处理Tag的属性
	//	4. 解析Tag，及其子Tag
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

    //生成待写入文件的XML数据
    void FormatData(ResizeableStringFormatter* formatter, UInt32 indent);

private:
	//If there is comments in parser，then consume the comments；for example <!-- ...... -->
    void ConsumeIfComment(StringParser* parser);

    char* fTag;				//当前的标签, 如SERVER
    char* fValue;			//Tag的值，如<VALUE>554</VALUE>，fValue = "554"
    OSQueue fAttributes;	//XML Tag的各种属性, 队列元素的类型为XMLAttribute
    OSQueue fEmbeddedTags;	//子标签队列
    
    OSQueueElem fElem;		//该Tag对应的Element对象

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
    Bool16  DoesFileExist();			//指定的文件是否存在
    Bool16  DoesFileExistAsDirectory();	//文件是否是目录类型
    Bool16  CanWriteFile();				//是否可写

	//该文件解析XML文件, 主要包括以下步骤
	// 1. 打开文件, 并按文件长度申请内存fileData, 从文件读入数据
	// 2. 从数据构造字符串, 及其解析器StringParser
	// 3. 构造XMLTag对象fRootTag, 并用该对象解析读入的字符串
	// 4. 关闭文件    
    Bool16  ParseFile(char* errorBuffer = NULL, int errorBufferSize = 0);		//errorBuffer是输向的错误信息
										//读入文件, 解析标签, 关闭文件
    XMLTag* GetRootTag() { return fRootTag; }
    void SetRootTag(XMLTag* tag);		//fRootTag = tag
    
    void WriteToFile(char** fileHeader);	//fileHeader指向字符串数组, 在每个一数组元素后加上"\r\n", 并且把数据写入文件
    
private:
    XMLTag*         fRootTag;	//根标签, XMLTag是一个递归的数据结构
        
    OSFileSource    fFile;		//系统文件
    char*           fFilePath;	//文件路径
    DTDVerifier*    fVerifier;	//DTD校验器
};

#endif
