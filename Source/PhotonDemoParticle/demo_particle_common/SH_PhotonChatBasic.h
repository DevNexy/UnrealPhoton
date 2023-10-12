// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Photon/Common-cpp/inc/Common.h"

class FString;

class SH_PhotonChatBasic
{

public:
	virtual ~SH_PhotonChatBasic() {};

	// Error Debug Message
	virtual void Chat_ErrorCheckMessage(const FString& message, int error) {}

	// Connect
	virtual void Chat_ConnectComplete(void) {}

	// Subscribe
	virtual void Chat_AddSubscribe(const FString& Channel) {}
	virtual void Chat_RemoveSubscribe(const FString& Channel) {}

	// Chat Mesage 
	virtual void Chat_getMessageEvent(const FString& ChannelName, const FString& sender, const FString& Message) {}

};
