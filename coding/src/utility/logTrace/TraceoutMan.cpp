#pragma warning(disable:4996)
#pragma warning(disable:4786)

#include "string"
#include "stdafx.h"
#include <cassert>
#include <list>
#include <algorithm>
#include <time.h>
#include "traceoutman.h"
#include "string"


namespace TraceoutNM {


	const int INDENT_NUM = 4;
	const int TRACE_FILE_QUATO = 2*1024*1024;
	inline static bool TestAndCreateDir( const char* szPath )
	{
		return ::CreateDirectoryA(szPath, NULL) || (GetLastError() == ERROR_ALREADY_EXISTS);
	}
	class AutoLock
	{
		CRITICAL_SECTION& cs_;
		AutoLock(const AutoLock&);
		AutoLock& operator=(const AutoLock&);
	public:
		AutoLock(CRITICAL_SECTION& cs):cs_(cs){ ::EnterCriticalSection(&cs); }
		~AutoLock(){ ::LeaveCriticalSection(&cs_); }
	};

	struct MessageBlock
	{
		struct CompareDepth {
			unsigned int depth_;
			explicit CompareDepth(unsigned int depth):depth_(depth){}
			bool operator()(const MessageBlock& left) {
				return left.depth == depth_;
			}
		};
		unsigned int depth;
		std::string text;
	};

	class TlsData {
	public:
		explicit TlsData(int index);
		~TlsData();
		TlsData( const TlsData& rhs );
		TlsData& operator=( const TlsData& rhs );
		void Write( const char* pbuf, size_t size, TR_FLAG flag = TR_QUEUED, int deltaDepth = 0 );
	protected:
		void CreateLogFile(const char* mode);
		void MutantCopy(const TlsData& rhs);
		void WriteStartInfo(const char* event_text);
		void WriteToOutput( const char* pbuf, size_t size, int depth = 0 );
		void CheckFileQuato();
	private:
		//
		//Modified by Andy
		FILE* f_;
		//static FILE* f_;
		//
		//
		typedef std::list<MessageBlock> MessageList;
		MessageList m_msgQueue;
		int depth_;
		TR_FLAG status_;
		int index_;
	};

	//added by andy
	//FILE* TlsData::f_ = NULL;

	class TlsTraceMan
	{
	public:
		TlsTraceMan();
		~TlsTraceMan();
		TlsData* GetTlsDataHandle();
	protected:
		TlsData* CreateTlsData();
	private:
		DWORD tlsIndex_;
		std::list<TlsData> tlsDataList_;
		CRITICAL_SECTION cs_;
	};
	
	void TlsData::CreateLogFile(const char* mode)
	{
		char szPath[MAX_PATH+100], szShortName[MAX_PATH];
		DWORD c = ::GetModuleFileNameA( NULL, szPath, sizeof(szPath) );
		char* p = strrchr( szPath, '\\' );
		if( p == NULL ) return;
		strncpy( szShortName, p, sizeof(szShortName)-1 );
		szShortName[sizeof(szShortName)-1] = '\0';
		strcpy(p+1, "log");

		if( !TestAndCreateDir(szPath) ) return;
		strcat( szPath, szShortName );

		p = strrchr( szPath, '.' );
		if( p == NULL )
			p = szPath + c;

		_snprintf( p, szPath+sizeof(szPath)-p,
			"_st_%d_%d.log", GetCurrentProcessId(), index_ );    //获得文件名附加值
	//	f_ = fopen(szPath, mode);

		// Modified by andy
		if (! f_)
		{
			f_ = fopen(szPath, mode);
			WriteStartInfo("started");
		}
	}



	TlsData::TlsData(int index)
		:f_(NULL)
		,depth_(0)
		,status_(TR_QUEUED)
		,index_(index)
	/*  Modified by andy
	TlsData::TlsData(int index)
		:depth_(0)
		,status_(TR_QUEUED)
		,index_(index)
	*/
	{
		CreateLogFile("a+");
	}

	TlsData::~TlsData()
	{
		WriteStartInfo("stopped");
		if(f_)
		{
			fclose(f_);
		}
	}
	TlsData::TlsData( const TlsData& rhs )
	{
		MutantCopy(rhs);
	}
	TlsData& TlsData::operator=( const TlsData& rhs )
	{
		MutantCopy(rhs);
		return *this;
	}
	void TlsData::MutantCopy(const TlsData& rhs)
	{
		if( this != &rhs )
		{
			f_ = rhs.f_;
			depth_ = rhs.depth_;
			index_ = rhs.index_;
			status_ = rhs.status_;
			const_cast<TlsData&>(rhs).f_ = NULL;
		}
	}

	void TlsData::WriteStartInfo(const char* event_text)
	{
		char buf[1024], szProcessPath[MAX_PATH];
		time_t tm;
		time(&tm);
		char *ptmstr = ctime(&tm);

		//char bufferTime[512];
		//errno_t theError = ctime_s(bufferTime, 512, &tm);

		ptmstr[ strlen(ptmstr)-1] = '\0';
		GetModuleFileNameA( GetModuleHandle(NULL), szProcessPath, sizeof(szProcessPath) );
		int c = _snprintf( buf, sizeof(buf),
			"\nSCOPE TRACE %s at %s in module: %s(pid:%d, tid:%d)\n",
			event_text,
			ptmstr,
			szProcessPath,
			GetCurrentProcessId(),
			GetCurrentThreadId()
			);
		if( c > 0 )
			WriteToOutput( buf, c );

		//delete [] ptmstr; //WZD 2008.06.17 Modify
	}

	inline
	void TlsData::WriteToOutput( const char* pbuf, size_t size, int depth )
	{
		if( f_ )
		{
			if( depth != 0 )
			{
				char szSpace[128];
				int left = depth*INDENT_NUM;
				int num = left < sizeof(szSpace) ? left : sizeof(szSpace);
				memset( szSpace, ' ', num );
				do
				{
					if( num > left )
						num = left;
					fwrite( szSpace, num, 1, f_ ); // padding whitespace
					left -= num;
				}while(left > 0);
			}
			fwrite( pbuf, size, 1, f_ );
		}
	}
	void TlsData::CheckFileQuato()
	{
		if( f_ )
		{
			long pos = ftell(f_);
			if( pos > TRACE_FILE_QUATO ) // 2M Bytes
			{
				fclose(f_);
				f_ = NULL;
				CreateLogFile("w+");
			}
		}
	}

	//写出线程数据
	void TlsData::Write( const char* pbuf, size_t size, TR_FLAG flag, int deltaDepth )
	{
		int disp_depth = deltaDepth > 0 ? depth_ : depth_+deltaDepth;
		depth_ += deltaDepth;
		assert( disp_depth >= 0 && depth_ >= 0 );
		if( status_ == TR_OUTPUT )
		{
			WriteToOutput( pbuf, size, disp_depth );
		}
		else
		{
			switch(flag)
			{
			case TR_QUEUED:
				{
					MessageBlock mb;
					mb.depth = disp_depth;
					mb.text.assign(pbuf,size);
					m_msgQueue.push_back( mb );
					status_ = TR_QUEUED;
				}
				break;
			case TR_OUTPUT:
				{
					MessageList::const_iterator it = m_msgQueue.begin();
					MessageList::const_iterator end = m_msgQueue.end();
					for( ; it != end; ++it )
					{
						const MessageBlock& msg = *it;
						WriteToOutput( msg.text.c_str(), msg.text.length(), msg.depth );
					}
					WriteToOutput( pbuf, size, disp_depth );
					m_msgQueue.clear();
					status_ = TR_OUTPUT;
				}
				break;
			case TR_DISCARD:		// reset tls data
				{
					int d = -1;
					while( !m_msgQueue.empty() && d != disp_depth )
					{
						d = m_msgQueue.back().depth;
						m_msgQueue.pop_back();
					}
				
				}
				break;
			default:
				return;
			}
		}

		if( depth_ == 0 )
		{
			if( status_ == TR_OUTPUT && f_ )
				fflush(f_);
			CheckFileQuato();
			status_ = TR_QUEUED;
		}
	}

	TlsTraceMan::TlsTraceMan()
	{
		InitializeCriticalSection(&cs_);
		tlsIndex_ = TlsAlloc();
	}

	TlsTraceMan::~TlsTraceMan()
	{
		TlsFree(tlsIndex_);
		DeleteCriticalSection(&cs_);
	}

	TlsData* TlsTraceMan::CreateTlsData()
	{
		TlsData* p = NULL;

		AutoLock lock(cs_);

		static int logIndex = 0;
		tlsDataList_.push_back(TlsData(++logIndex));
		p = &tlsDataList_.back();

		return p;
	}

	TlsData* TlsTraceMan::GetTlsDataHandle()
	{
		TlsData* ptls = (TlsData*)TlsGetValue(tlsIndex_);
		if( ptls == NULL )
		{
			ptls = CreateTlsData();
			TlsSetValue(tlsIndex_, ptls);
		}
		return ptls;
	}

}

static TraceoutNM::TlsTraceMan traceMan_;


void TraceoutMan::OutputString( const char* text, TraceoutNM::TR_FLAG flag, int deltaDepth )
{
	TraceoutNM::TlsData* ptls = traceMan_.GetTlsDataHandle();
	if( ptls )
		ptls->Write(text, strlen(text), flag, deltaDepth );
};
