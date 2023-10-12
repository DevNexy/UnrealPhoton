// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SH_PhotonChatBasic.h"

#include "../../Photon/Chat-cpp/inc/Listener.h"
#include "../../Photon/Chat-cpp/inc/Client.h"

using namespace ExitGames;

class PHOTONDEMOPARTICLE_API PhotonChatListener : public Chat::Listener
{
public:
	PhotonChatListener(SH_PhotonChatBasic* basicView);
	~PhotonChatListener();

private:
	ExitGames::Chat::Client* m_pChatClient;

	SH_PhotonChatBasic* m_pView;
public:
	void setChatClient(ExitGames::Chat::Client* client) {
		m_pChatClient = client;
	}

public:
	/**
	Callback interface for Chat client side. Contains callback methods to notify your app about updates.
	Must be provided to new Chat::Client in constructor*/

	// receive and print out debug out here
	virtual void debugReturn(int debugLevel, const Common::JString& string);

	/**
		Notifies app that client state changed.
		@param state new client state
		@sa ClientState::ClientState*/
	virtual void onStateChange(int state);

	// implement your error-handling here
	virtual void connectionErrorReturn(int errorCode);
	virtual void clientErrorReturn(int errorCode);
	virtual void warningReturn(int warningCode);
	virtual void serverErrorReturn(int errorCode);

	////////////////////////////////////////////////////////////////////////
	// callbacks for operations on server

	/**
		Client is connected now.*/
	virtual void connectReturn(int errorCode, const Common::JString& errorString);

	/**
		Disconnection happened.*/
	virtual void disconnectReturn(void);

	/**
		The result of the subscribe operation. Returns per channel name if the channel is now subscribed.
		@param channels channel names
		@param results per channel result: true if subscribed*/
	virtual void subscribeReturn(const Common::JVector<Common::JString>& channels, const Common::JVector<bool>& results);

	/**
		Result of unsubscribe operation. Returns per channel name if the channel is now subscribed.
		@param channels channel names that are no longer subscribed*/
	virtual void unsubscribeReturn(const Common::JVector<Common::JString>& channels);

	/**
		The new status of another user (you get updates for users that are in your friends list).
		@param user name of the user
		@param status new status of that user
		@param gotMessage true if the status contains a message you should cache locally. False: This status update does not include a message (keep any you have).
		@param message message that user set*/
	virtual void onStatusUpdate(const Common::JString& user, int status, bool gotMessage, const Common::Object& message);

	/**
		Notifies the app that the client got new messages from the server
		Number of senders is equal to number of messages in 'messages'. Sender with number '0' corresponds to message with
		number '0', sender with number '1' corresponds to message with number '1' and so on
		@param channelName channel from where messages came
		@param senders list of users who sent messages
		@param messages list of messages it self*/
	virtual void onGetMessages(const Common::JString& channelName, const Common::JVector<Common::JString>& senders, const Common::JVector<Common::Object>& messages);

	/**
		Notifies the app about a private message
		@param sender user who sent this message
		@param message the message itself
		@param channelName the channel name for private messages (messages that you sent yourself get added to a channel per target username)*/
	virtual void onPrivateMessage(const Common::JString& sender, const Common::Object& message, const Common::JString& channelName);

	/**
		Notifies the app about a broadcast message
		@param channelName the channel name through which the message is broadcast
		@param inBuff the message buffer
		@param inBuffBodyLength the message buffer length*/
	virtual void onReceiveBroadcastMessage(const Common::JString& channelName, nByte* inBuff, int inBuffBodyLength);
};
