// Fill out your copyright notice in the Description page of Project Settings.

#include "PhotonDemoParticle.h"
#include "PhotonChatListener.h"
#include "Common-cpp/inc/Common.h"
#include "LoadBalancingListener.h"

#include "Console.h"

using namespace ExitGames::LoadBalancing;

PhotonChatListener::PhotonChatListener(SH_PhotonChatBasic* basicView) : m_pView(basicView)
{
}

PhotonChatListener::~PhotonChatListener()
{
}

void PhotonChatListener::debugReturn(int debugLevel, const Common::JString& string)
{
	Console::get().writeLine(Common::JString("// PhotonChat debugReturn :: ") + debugLevel + Common::JString(" // ") + string);
}

void PhotonChatListener::onStateChange(int state)
{
	Console::get().writeLine(Common::JString("// PhotonChat onStateChange :: ") + state);
}

void PhotonChatListener::connectionErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat connectionErrorReturn :: ") + errorCode);
}

void PhotonChatListener::clientErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat clientErrorReturn :: ") + errorCode);
}

void PhotonChatListener::warningReturn(int warningCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat warningReturn :: ") + warningCode);
}

void PhotonChatListener::serverErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat serverErrorReturn :: ") + errorCode);
}

// callbacks for operations on server
void PhotonChatListener::connectReturn(int errorCode, const Common::JString& errorString)
{
	//UE_LOG(LogTemp, Log, TEXT("//Chat connectReturn::"));
	Console::get().writeLine(Common::JString("// PhotonChat errorCode ConnectReturn :: ") + errorCode);
	if (errorCode == ErrorCode::OK)
	{
		m_pView->Chat_ConnectComplete();
		Console::get().writeLine(Common::JString("// // ChatConnect Return :: True"));
	}
	else
	{
		Console::get().writeLine(Common::JString("// // Chat errorCode Message :: ") + errorString);
	}
}
//접속이 끊김
void PhotonChatListener::disconnectReturn(void)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: disconnectReturn "));
}

void PhotonChatListener::subscribeReturn(const Common::JVector<Common::JString>& channels, const Common::JVector<bool>& results)
{
	// 채팅 시스템은 정해진 구독 채널 안에서 활용됨.
	// 전체, 지역, 그룹 .. 
	Console::get().writeLine(Common::JString("// PhotonChat :: subscribeReturn"));
	int size = channels.getSize();
	for (int i = 0; i < size; i++)
	{
		Console::get().writeLine(Common::JString("// // Channels :: ") + channels[i] + Common::JString(" / result :: ") + results[i]);
		if (results[i])
		{
			m_pView->Chat_AddSubscribe(UTF8_TO_TCHAR(channels[i].UTF8Representation().cstr()));
		}
	}
}

void PhotonChatListener::unsubscribeReturn(const Common::JVector<Common::JString>& channels)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: unsubscribeReturn"));
	for (unsigned int i = 0; i < channels.getSize(); i++)
	{
		Console::get().writeLine(Common::JString("// // Channels :: ") + channels[i]);
		m_pView->Chat_RemoveSubscribe(UTF8_TO_TCHAR(channels[i].UTF8Representation().cstr()));
	}
}

void PhotonChatListener::onStatusUpdate(const Common::JString& user, int status, bool gotMessage, const Common::Object& message)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: onStatusUpdate ") + user);
}

//일반 메시지 수신부
void PhotonChatListener::onGetMessages(const Common::JString& channelName, const Common::JVector<Common::JString>& senders, const Common::JVector<Common::Object>& messages)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: onGetMessages ChannelName ") + channelName);
	int size = senders.getSize();

	for (int i = 0; i < size; i++)
	{
		if (messages[i].getType() == ExitGames::Common::TypeCode::STRING)
		{
			ExitGames::Common::JString Check = ExitGames::Common::ValueObject<ExitGames::Common::JString>(messages[i]).getDataCopy();
			// JString* data = ((ValueObject<JString*>*)obj)->getDataCopy();
			// Console::get().writeLine(Common::JString("// // Senders :: ") + senders[i] + Common::JString("// Message :: ") + Check);
			m_pView->Chat_getMessageEvent(FString(UTF8_TO_TCHAR(channelName.UTF8Representation().cstr())), FString(UTF8_TO_TCHAR(senders[i].UTF8Representation().cstr())), FString(UTF8_TO_TCHAR(Check.UTF8Representation().cstr())));
		}
		else
		{
			Console::get().writeLine(Common::JString("// // Senders :: ") + senders[i] + Common::JString("// Not String  "));
		}
	}
}
//귓속말 수신부
void PhotonChatListener::onPrivateMessage(const Common::JString& sender, const Common::Object& message, const Common::JString& channelName)
{
	Console::get().writeLine(Common::JString("// onPrivateMessage sender :: ") + sender);
	Console::get().writeLine(Common::JString("// // message		:: "));
	Console::get().writeLine(Common::JString("// // channelName :: ") + channelName);
}

void PhotonChatListener::onReceiveBroadcastMessage(const Common::JString& channelName, nByte* inBuff, int inBuffBodyLength)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: onReceiveBroadcastMessage channelName ") + channelName);
}
