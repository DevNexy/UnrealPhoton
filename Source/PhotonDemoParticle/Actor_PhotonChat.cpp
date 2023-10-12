// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor_PhotonChat.h"
#include <algorithm>

#define CHAT_AppID "7906ca2a-48b7-4ac7-8763-8ebcf7112886"
#define WorldChannel "World"
using namespace ExitGames;

// Sets default values
AActor_PhotonChat::AActor_PhotonChat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pListener = new PhotonChatListener(this);
	m_pChatClient = new ExitGames::Chat::Client(*m_pListener, Common::JString(CHAT_AppID), Common::JString("ns.photonengine.io"));
	m_pListener->setChatClient(m_pChatClient);
}
// Called when the game starts or when spawned
void AActor_PhotonChat::BeginPlay()
{
	Super::BeginPlay();
}

void AActor_PhotonChat::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (m_pListener)
		delete m_pListener;
	if (m_pChatClient)
		delete m_pChatClient;
}

// Called every frame
void AActor_PhotonChat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//E_LOG(LogTemp, Log, TEXT("Room Name : %s"), *targetSubscribe);
	if (m_pChatClient)
		m_pChatClient->service();
}
//connect
void AActor_PhotonChat::Connect_Chat(const FString& playfabid)
{
	// 테스트 접속
	FString UserID = playfabid;
	if (m_pChatClient)
	{
		ExitGames::Chat::AuthenticationValues AuthenticationValue;
		AuthenticationValue.setUserID(TCHAR_TO_UTF8(*UserID));

		m_pChatClient->connect(AuthenticationValue);
		UE_LOG(LogTemp, Log, TEXT("Connect_Chat Playfab ID : %s"), *playfabid);
	}
}
//Message
//채널 추가하기
void AActor_PhotonChat::AddSubscribeEvent(TArray<FString> sub)
{
	Common::JVector<Common::JString> Subscribechannels;

	for (auto it : sub)
	{
		FString Subscribe = it;
		Subscribechannels.addElement(TCHAR_TO_UTF8(*Subscribe));
	}
	
	m_pChatClient->opSubscribe(Subscribechannels);
}
//해당 채널에서 받은 메세지 출력
void AActor_PhotonChat::LoadRecievedMessages(const FString& channelName)
{
	//채널을 받아옴
	Common::JVector<Chat::Channel*> TestChannel = m_pChatClient->getPublicChannels();
	int ChannelLen = TestChannel.getSize(); //채널 길이

	//채널 이름을 기반으로 채널을 찾기
	Chat::Channel* targetChannel = nullptr;
	for (int j = 0; j < ChannelLen; j++)
	{
		Chat::Channel* channel = TestChannel[j];
		FString channelNameUTF8 = FString(UTF8_TO_TCHAR(channel->getName().UTF8Representation().cstr()));
		if (channelName == *channelNameUTF8)
		{
			targetChannel = channel;
			break;
		}
	}

	if (targetChannel)
	{
		//해당 채널의 메시지를 가져오기
		const Common::JVector<Common::Object> ChannelMessages = targetChannel->getMessages();
		int MessageLen = ChannelMessages.getSize();

		//현재 채널 출력
		UE_LOG(LogTemp, Log, TEXT("Current Channel : %s"), *channelName);

		//채널에 따라 메시지를 저장
		TArray<FString>* CurrentMessagesArray = nullptr;
		if (channelName == "Global")
		{
			CurrentMessagesArray = &GlobalMessages;
		}
		else if (channelName == "Team")
		{
			CurrentMessagesArray = &TeamMessages;
		}
		else if (channelName == "Private")
		{
			CurrentMessagesArray = &PrivateMessages;
		}

		if (CurrentMessagesArray)
		{
			//기존 메시지 초기화
			CurrentMessagesArray->Empty();
			for (int j = 0; j < MessageLen; j++)
			{
				if (ChannelMessages[j].getType() == ExitGames::Common::TypeCode::STRING)
				{
					//받은 메시지 FString으로 변환
					ExitGames::Common::JString Check = ExitGames::Common::ValueObject<ExitGames::Common::JString>(ChannelMessages[j]).getDataCopy();
					FString ReceivedMessage = FString(UTF8_TO_TCHAR(Check.UTF8Representation().cstr()));

					//받은 메시지 배열에 저장
					CurrentMessagesArray->Add(ReceivedMessage);
				}
			}
			Bluerprint_receivedMessageEvent(*CurrentMessagesArray);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid channel name: %s"), *channelName);
	}
}
// 일반 메세지
void AActor_PhotonChat::SendMessageEvent(const FString& ChannelName, const FString& Message)
{
	if (m_pChatClient)
	{
		//메시지 전송 시도
		if (m_pChatClient->opPublishMessage(TCHAR_TO_UTF8(*ChannelName), TCHAR_TO_UTF8(*Message)))
		{
			//메시지 전송 성공
			UE_LOG(LogTemp, Log, TEXT("Message sent successfully."));
		}
		else
		{
			//메시지 전송 실패
			UE_LOG(LogTemp, Error, TEXT("Failed to send message."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Chat client is not initialized."));
	}
	
}
//귓속말
void AActor_PhotonChat::SendPrivateMessage(const FString& username, const FString& message)
{
	m_pChatClient->opSendPrivateMessage(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*message));
}
//채팅 타겟 변경
void AActor_PhotonChat::setTargetSubscribe(enum_PhotonChat target)
{
	if (target == enum_PhotonChat::World)
	{
		targetSubscribe = WorldChannel;
	}
	else {
		targetSubscribe = m_RoomName + "_" + FString::FromInt((int)target);
	}
	// 변경에 따른 UI 수정
	// ChangeTargetSubscribe(target);
}
//챗 연결 완료
void AActor_PhotonChat::Chat_ConnectComplete(void)
{
	//방 배열 생성
	JiwonRoom.Add("Global"); //전체
	JiwonRoom.Add("Team"); //팀 
	JiwonRoom.Add("Private"); //개인

	// Blueprint
	Blueprint_ConnectComplete(); // 보이스채팅 스폰액터
	AddSubscribeEvent(JiwonRoom);
}
//메세지 출력
void AActor_PhotonChat::Chat_getMessageEvent(const FString& ChannelName, const FString& sender, const FString& Message)
{
	// 메세지 위젯 출력
	Blueprint_getMessageEvent(ChannelName, sender, Message);
}
//구독자 추가 
void AActor_PhotonChat::Chat_AddSubscribe(const FString& Channel)
{
	SubscribeList.AddUnique(Channel);
}
//구독자 제거
void AActor_PhotonChat::Chat_RemoveSubscribe(const FString& Channel)
{
	SubscribeList.Remove(Channel);
}
//채널 접속
void AActor_PhotonChat::Chat_ResetJoinChannel(const FString& FullName)
{
	//기존리스트 가지고 있다면 삭제 후 초기화
	if (SubscribeList.IsValidIndex(0))
	{
		Common::JVector<Common::JString> Subscribechannels;
		for (auto it : SubscribeList)
		{
			Subscribechannels.addElement(TCHAR_TO_UTF8(*it));
		}
		m_pChatClient->opUnsubscribe(Subscribechannels);
	}
	SubscribeList.Reset();


	m_RoomName = FullName;

	for (int i = 0; i < (int)enum_PhotonChat::Max; i++)
	{
		FString ChannelName = m_RoomName + "_" + FString::FromInt(i); // 일반, 귓속말 ...
		SubscribeList.Add(ChannelName);
	}
	FString WorldName = WorldChannel;
	SubscribeList.Add(WorldName);
	AddSubscribeEvent(SubscribeList);

	// 디폴트 채널 설정
	setTargetSubscribe(enum_PhotonChat::Public);
}
