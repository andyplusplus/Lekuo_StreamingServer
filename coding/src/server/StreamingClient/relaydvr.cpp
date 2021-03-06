#include "stdAfx.h"
#include "RelayDVR.h"
#include "ClientSession.h"
#include "LkStrParser.h"
#include "Log.h"
#include "ILkTokenParser.h"


#pragma region Public Functions ====================================================

	RelayDVR::RelayDVR(rsString inStr): m_CreateDVRStr(inStr), m_isLogin(FALSE)
	{
		m_CreateDVRStr += TOKEN_Sep;
		CLog::LogFun_SS("RelayDVR::RelayDVR()", inStr.c_str());
	}


	RelayDVR::~RelayDVR()
	{
		DestroyAllChannel();
	}


	LK_Error RelayDVR::Login(const char* inIP, int inPort, const char* inUserName, const char* inPassWord)
	{
		CLog::LogFun_SSISS("RelayDVR::Login", inIP, inPort, inUserName, inPassWord);

		m_IP = inIP;
		m_LoginStr = str(	boost::format("%1%=%2%;%3%=%4%;%5%=%6%;%7%=%8%;") % 
								LK_KEYWORD_STR[LK_KEYWORD_Device_IP] % inIP % 
								LK_KEYWORD_STR [LK_KEYWORD_Device_Port] % inPort	%
								LK_KEYWORD_STR[LK_KEYWORD_Device_UserName] % inUserName %
								LK_KEYWORD_STR[LK_KEYWORD_Device_Password]%inPassWord				);
	
		m_isLogin = TRUE;
		return LK_NoErr;
	}


	int RelayDVR::IsLogedin()
	{
		CLog::LogFun_SS("RelayDVR::isLogin", m_IP.c_str());

		return m_isLogin;
	}



	LK_Error RelayDVR::Logout()
	{
		CLog::LogFun_SS("RelayDVR::LogoutDVR", m_IP.c_str());

		DestroyAllChannel();
		m_LoginStr = "";
		m_isLogin = FALSE;
		return LK_NoErr;
	}



	LPIChannel RelayDVR::CreateChannel(const char* inChannelStr, Tag inTag, IMutex* inMutex)
	{
		CLog::LogFun_SSTI("RelayDVR::CreateChannel", inChannelStr, inTag, inMutex);

		rsString theCreateChannelStr = m_CreateDVRStr;
		theCreateChannelStr += 	inChannelStr;//m_CreateDVRStr + inChannelStr;

		LPIChannel theChannel = getChannel(theCreateChannelStr);
		if(!theChannel)
		{
			theChannel = new ClientSession(theCreateChannelStr, inTag, inMutex, this);
			rsString theChanKey = getChannelKey(theCreateChannelStr);
			m_Channel_map[theChanKey] = theChannel;
		}



		return theChannel;
	}

	//Note:
	//      Channel必须先rerase, 于Teardown，因为ClientSession::~ClientSession会先调用isChannelExist，然后再调用DestroyChannel
	LK_Error RelayDVR::DestroyChannel(LPIChannel inChannel)
	{
		CLog::LogFun_SI("RelayDVR::DestroyChannel", inChannel);

	  /*
			if(m_MsgReceiver)
			{
				m_MsgReceiver->ReceiveMsg((HANDLE) this, 0, MSG_TYPE_Channel_ChannelDeleted, 
					(unsigned char*) inChannel, sizeof(inChannel)  );
			}
    */
		Channel_map_iter thePos;
		for(thePos = m_Channel_map.begin(); thePos != m_Channel_map.end(); ++thePos)
		{
			if (thePos->second == inChannel)
				break;
		}

		int theRemoved = 0;
		if (thePos != m_Channel_map.end()) 
		{
			//theRemoved = m_Channel_map.erase(thePos);
			m_Channel_map.erase(thePos);
			((ClientSession*) inChannel)->TearDown();
		}else
		{
			qtss_printf("RelayDVR::DestroyChannel(%d) --> channel is not deleted\n", inChannel);
		}

		return LK_NoErr;
	}



	LK_Error RelayDVR::DestroyAllChannel()
	{
		CLog::LogFun_S("RelayDVR::DestroyAllChannel");

		m_Channel_map.clear();

		for(Channel_map_iter theChan = m_Channel_map.begin(); theChan != m_Channel_map.end(); ++theChan)
		{
			((ClientSession*) theChan->second)->TearDown();
			//DelChanTask(theChan->second);
		}

		return LK_NoErr;
	}

#pragma endregion Public Functions ====================================================



rsString RelayDVR::getChannelKey(rsString inCreateStr)
{
	std::auto_ptr<ILkTokenParser> theTokenParser(CreateTokenParser(inCreateStr.c_str()));

	char theToken[NORM_CHAR_SIZE];
	theTokenParser->getToken(LK_KEYWORD_STR[LK_KEYWORD_Channel_ID], theToken, NORM_CHAR_SIZE, NULL);

	rsString theChanKey = theToken;

	return theChanKey;
}



LPIChannel RelayDVR::getChannel(rsString inCreateStr)
{
	LPIChannel theChannel = NULL;

	rsString theChanKey = getChannelKey(inCreateStr);
	Channel_map_iter thePos = m_Channel_map.find(theChanKey);
	BOOL isKeyExist = (thePos != m_Channel_map.end());
	if(isKeyExist)
	{
		theChannel = thePos->second;
	}

	return theChannel;
}



//void RelayDVR::DelChanTask(LPIChannel inChannel)
//{
//	ClientSession* theSession = (ClientSession*) inChannel;
//	theSession->Destroy();				//Ask the main thread to Kill the task, we could not delete the task explicitally
//}

void RelayDVR::ReceiveMsg( HANDLE hSender, Tag inSenderTag, unsigned long inMsgType, unsigned char* inMsgData, unsigned long inLength, int isChanDeleted)
{
	if(this->m_MsgReceiver)
	{
		m_MsgReceiver->ReceiveMsg(hSender, inSenderTag, inMsgType, inMsgData, inLength, isChanDeleted);
	}

}