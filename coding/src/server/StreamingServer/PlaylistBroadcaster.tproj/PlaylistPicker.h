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

#ifndef __playlist_picker__
#define __playlist_picker__

/*
   PlaylistPicker.h
*/


#include "PLDoubleLinkedList.h"
#include "SimplePlayListElement.h"

#include "OSHeaders.h"

#include "NoRepeat.h"

class PlaylistPicker
{

    public:
        enum { kMaxBuckets = 10 };

        //表示是否是SequentialLooping(即是否循环选取节目), 默认mBuckets= 2, 默认顺序选取, 不定义NoRepeat对象
        PlaylistPicker(bool doLoop);

		//根据以下两项参数构造PlaylistPicker选择对象
		//    numBuckets: 列表数
		//    numNoRepeats: 每个列表的非重复节目数
        PlaylistPicker( UInt32 numBuckets, UInt32 numNoRepeats );
        
		virtual ~PlaylistPicker();
        
		void        CleanList();	//删除相关列表中的元素
        bool        AddToList( const char *name, int weight );						//把值为name:weight的值对加入链表
        bool        AddNode( PLDoubleLinkedListNode<SimplePlayListElement> *node );	//把结点加入双向链表
        char*       PickOne();		//按照一个的选取策略选择一个节目, 具体执行过程如下
        char*       LastPick() { return fLastPick; }
        UInt32      GetNumBuckets() { return mBuckets; }
/* changed by emil@popwire.com (see relaod.txt for info) */
        UInt32      GetNumMovies() { return mNumToPickFrom; }

        bool        mRemoveFlag;					//是否要删除节目, 如果是, 每次选取节目后mNumToPickFrom--
        bool        mStopFlag;
/* ***************************************************** */
        long*       mPickCounts;					//和mElementLists数组相对应, 特定PLDoubleLinkedList中的结点数
        long        mNumToPickFrom;					//可以选取的节目数
        UInt32      mRecentMoviesListSize;			//双向列表mElementLists的最大长度
        char*       fLastPick;
        PLDoubleLinkedList<SimplePlayListElement>*  GetBucket( UInt32 myIndex ) { return mElementLists[myIndex]; }
        
        char*       GetFirstFile() { return mFirstElement; }		

    protected:
        
        bool        mIsSequentialPicker;    // picker picks sequentially?
        bool        mIsSequentialLooping;   // loop over and over?
        int         mWhichSequentialBucket; // sequential picker picks  from list0 or list1?, 表示当前正在使用的桶
        
        UInt32      Random();				//生成一个随机数
        UInt32      mLastResult;
        
        char*       PickFromList( PLDoubleLinkedList<SimplePlayListElement>* list, UInt32 elementIndex );  //按Index, 从列表中选择一个元素
        
        PLDoubleLinkedList<SimplePlayListElement>* mElementLists[kMaxBuckets];	//双向链表
        
        UInt32                                  mBuckets;						//mElementLists的实际长度
        NoRepeat                                *mUsedElements;					//不重复的双向链表
        char*       mFirstElement;
};



#endif
