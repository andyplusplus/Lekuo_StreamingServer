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
    File:       osfilesource.h

    Contains:   simple file abstraction. This file abstraction is ONLY to be
                used for files intended for serving 
                    
    
*/

#ifndef __OSFILE_H_
#define __OSFILE_H_

#include <stdio.h>
#include <time.h>

#include "OSHeaders.h"
#include "StrPtrLen.h"
#include "OSQueue.h"

#define READ_LOG 0

//该类实际是文件数据块的数据结构，所有的成员变量都是公有的
class FileBlockBuffer 
{

 public:
    FileBlockBuffer(): fArrayIndex(-1),fBufferSize(0),fBufferFillSize(0),fDataBuffer(NULL),fDummy(0){}
    ~FileBlockBuffer(void);
    void AllocateBuffer(UInt32 buffSize);
    void TestBuffer(void);
    void CleanBuffer() { ::memset(fDataBuffer,0, fBufferSize); }
    void SetFillSize(UInt32 fillSize) {fBufferFillSize = fillSize;}
    UInt32 GetFillSize(void) { return fBufferFillSize;}
    OSQueueElem *GetQElem() { return &fQElem; }
    SInt64              fArrayIndex;	//该文件块在FileMap::fFileMapArray数组中的位置
    UInt32              fBufferSize;	//缓存大小
    UInt32              fBufferFillSize;//??
    char                *fDataBuffer;	//指向缓存的内存区
    OSQueueElem         fQElem;			//FileBlockPool::fQueue队列的元素
    UInt32              fDummy;			//
};


//该类维护了FileBlockBuffer的队列结构
class FileBlockPool
{
    enum {
            kDataBufferUnitSizeExp      = 15,// base 2 exponent
            kBufferUnitSize             = (1 << kDataBufferUnitSizeExp ) // 32Kbytes
    };

    public:
        FileBlockPool(void) :  fMaxBuffers(1),  fNumCurrentBuffers(0), fBufferUnitSizeBytes(kBufferUnitSize){}
        ~FileBlockPool(void);
        
        void SetMaxBuffers(UInt32 maxBuffers) { if (maxBuffers > 0) fMaxBuffers = maxBuffers; }

        void SetBuffIncValue(UInt32 bufferInc) { if (bufferInc > 0) fBufferInc = bufferInc;}
        void IncMaxBuffers(void) { fMaxBuffers += fBufferInc; }
        void DecMaxBuffers(void) { if (fMaxBuffers > fBufferInc) fMaxBuffers-= fBufferInc; }
        void DecCurBuffers(void) { if (fNumCurrentBuffers > 0) fNumCurrentBuffers--; }
        
        void SetBufferUnitSize  (UInt32 inUnitSizeInK)      { fBufferUnitSizeBytes = inUnitSizeInK * 1024; }
        UInt32 GetBufferUnitSizeBytes()     { return fBufferUnitSizeBytes; }
        UInt32 GetMaxBuffers(void)  { return fMaxBuffers; }
        UInt32 GetIncBuffers()      { return fBufferInc; }
        UInt32 GetNumCurrentBuffers(void)   { return fNumCurrentBuffers; }	
        void DeleteBlockPool();
        FileBlockBuffer* GetBufferElement(UInt32 bufferSizeBytes);
        void MarkUsed(FileBlockBuffer* inBuffPtr);	//文件块使用了以后，把文件块放在队列的尾部，实现时，数据块先出队列，然后再重新加入队列。

    private:
        OSQueue fQueue;			//文件池队列
        UInt32  fMaxBuffers;	//FileBlockPool可申请的最大FileBlock数
        UInt32  fNumCurrentBuffers;		//当前的FileBlock数
        UInt32  fBufferInc;				//每次缓存的增量
        UInt32  fBufferUnitSizeBytes;	//该值默认设置为32K，好象该值没什么用
        UInt32  fBufferDataSizeBytes;	//Not Use

};

class FileMap 
{

    public:
        FileMap(void):fFileMapArray(NULL),fDataBufferSize(0),fMapArraySize(0),fNumBuffSizeUnits(0) {}
        ~FileMap(void) {fFileMapArray = NULL;}

		//按输入的参数分配一定的缓存
        void    AllocateBufferMap(UInt32 inUnitSizeInK, UInt32 inNumBuffSizeUnits, UInt32 inBufferIncCount, 
								  UInt32 inMaxBitRateBuffSizeInBlocks, UInt64 fileLen, UInt32 inBitRate);

        //获得指定索引的FileBlockBuffer，如果数组的该索引为空，则临时申请空间
		char*   GetBuffer(SInt64 bufIndex, Bool16 *outIsEmptyBuff);
        void    TestBuffer(SInt32 bufIndex) {Assert (bufIndex >= 0); fFileMapArray[bufIndex]->TestBuffer();};
        void    SetIndexBuffFillSize(SInt32 bufIndex, UInt32 fillSize) { Assert (bufIndex >= 0); fFileMapArray[bufIndex]->SetFillSize(fillSize);}
        UInt32  GetMaxBufSize(void) {return fDataBufferSize;}
        UInt32  GetBuffSize(SInt64 bufIndex)    { Assert (bufIndex >= 0); return fFileMapArray[bufIndex]->GetFillSize(); }
        UInt32  GetIncBuffers(void) { return fBlockPool.GetIncBuffers(); }
        void    IncMaxBuffers()     {fBlockPool.IncMaxBuffers(); }
        void    DecMaxBuffers()     {fBlockPool.DecMaxBuffers(); }
        Bool16  Initialized()       { return fFileMapArray == NULL ? false : true; }
        void    Clean(void);		//指fFileMapArray指向的数组请空
        void    DeleteMap(void);	//删除fFileMapArray数组
        void    DeleteOldBuffs(void);	//删除多余的缓存数
        SInt64  GetBuffIndex(UInt64 inPosition) {   return inPosition / this->GetMaxBufSize();  }
        SInt64  GetMaxBuffIndex() { Assert(fMapArraySize > 0); return fMapArraySize -1; }
        UInt64  GetBuffOffset(SInt64 bufIndex) { return (UInt64) (bufIndex * this->GetMaxBufSize() ); }

        
		FileBlockPool fBlockPool;				//包含FileBlockPool对象

        FileBlockBuffer**   fFileMapArray;		//文件块数组
    
    private:

    UInt32              fDataBufferSize;	//数据缓存的字节数
    SInt64              fMapArraySize;		//fFileMapArray数组的大小fMapArraySize
    UInt32              fNumBuffSizeUnits;	//数据块单元数??
    
};

//对某一文件进行封装，以便于进行读写
class OSFileSource
{
    public:
    
        OSFileSource() :    fFile(-1), fLength(0), fPosition(0), fReadPos(0), fShouldClose(true), fIsDir(false), fCacheEnabled(false)                       
        {
        
        #if READ_LOG 
            fFileLog = NULL;
            fTrackID = 0;
            fFilePath[0]=0;
        #endif
        
        }
                
        OSFileSource(const char *inPath) :  fFile(-1), fLength(0), fPosition(0), fReadPos(0), fShouldClose(true), fIsDir(false),fCacheEnabled(false)
        {
         Set(inPath); 
         
        #if READ_LOG 
            fFileLog = NULL;
            fTrackID = 0;
            fFilePath[0]=0;
        #endif      
         
         }
        
        ~OSFileSource() { Close();  fFileMap.DeleteMap();}
        
        //Sets this object to reference this file
        void            Set(const char *inPath);		//打开文件，并赋值给fFile
        
        // Call this if you don't want Close or the destructor to close the fd
        void            DontCloseFD() { fShouldClose = false; }
        
        //Advise: this advises the OS that we are going to be reading soon from the
        //following position in the file
        void            Advise(UInt64 advisePos, UInt32 adviseAmt);	//该函数没有实现，是一个空函数
        
        OS_Error    Read(void* inBuffer, UInt32 inLength, UInt32* outRcvLen = NULL)	//从文件读取数据
                    {   return ReadFromDisk(inBuffer, inLength, outRcvLen);
                    }
        

		//从特定位置读取数据，如果没有初始化，则调用ReadFromPos,否则调用ReadFromCache
        OS_Error    Read(UInt64 inPosition, void* inBuffer, UInt32 inLength, UInt32* outRcvLen = NULL);	
		
        OS_Error    ReadFromDisk(void* inBuffer, UInt32 inLength, UInt32* outRcvLen = NULL);	//从磁盘的当前位置读取数据
        OS_Error    ReadFromCache(UInt64 inPosition, void* inBuffer, UInt32 inLength, UInt32* outRcvLen = NULL); //从fFileMap读取数据
        OS_Error    ReadFromPos(UInt64 inPosition, void* inBuffer, UInt32 inLength, UInt32* outRcvLen = NULL);	//实际最后调用ReadFromDisk
        void        EnableFileCache(Bool16 enabled) {OSMutexLocker locker(&fMutex); fCacheEnabled = enabled; }
        Bool16      GetCacheEnabled() { return fCacheEnabled; }
        void        AllocateFileCache(UInt32 inUnitSizeInK = 32, UInt32 bufferSizeUnits = 0, UInt32 incBuffers = 1, UInt32 inMaxBitRateBuffSizeInBlocks = 8, UInt32 inBitRate = 32768) 
                    {   fFileMap.AllocateBufferMap(inUnitSizeInK, bufferSizeUnits,incBuffers, inMaxBitRateBuffSizeInBlocks, fLength, inBitRate);
                    } 
        void        IncMaxBuffers()     {OSMutexLocker locker(&fMutex); fFileMap.IncMaxBuffers(); }
        void        DecMaxBuffers()     {OSMutexLocker locker(&fMutex); fFileMap.DecMaxBuffers(); }

        OS_Error    FillBuffer(char* ioBuffer, char *buffStart, SInt32 bufIndex);
                
        void            Close();
        time_t          GetModDate()                { return fModDate; }
        UInt64          GetLength()                 { return fLength; }
        UInt64          GetCurOffset()              { return fPosition; }
        void            Seek(SInt64 newPosition)    { fPosition = newPosition;  }
        Bool16 IsValid()                            { return fFile != -1;       }
        Bool16 IsDir()                              { return fIsDir; }
        
        // For async I/O purposes
        int             GetFD()                     { return fFile; }
        void            SetTrackID(UInt32 trackID);
        // So that close won't do anything
        void ResetFD()  { fFile=-1; }

        void SetLog(const char *inPath);		//把该参数设置为fFilePath
    
    private:

        int     fFile;		//打开的文件描述符
        UInt64  fLength;	//文件长度
        UInt64  fPosition;	//当前位置
        UInt64  fReadPos;	//下一次读取的位置，该变量好象总是和前一个变量相同
        Bool16  fShouldClose;//调用OSFileSource::Close()函数时是否需要关闭文件，如果fShouldClose为False，则不关闭文件，只是初始化相关变量。
        Bool16  fIsDir;		//是否是目录
        time_t  fModDate;	//修改日期
        
        
        OSMutex fMutex;
        FileMap fFileMap;
        Bool16  fCacheEnabled;
#if READ_LOG
        FILE*               fFileLog;
        char                fFilePath[1024];		//文件路径
        UInt32              fTrackID;				//由SetTrackID函数设置
#endif

};

#endif //__OSFILE_H_
