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
	// �׽�Ʈ ����
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
//ä�� �߰��ϱ�
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
//�ش� ä�ο��� ���� �޼��� ���
void AActor_PhotonChat::LoadRecievedMessages(const FString& channelName)
{
	//ä���� �޾ƿ�
	Common::JVector<Chat::Channel*> TestChannel = m_pChatClient->getPublicChannels();
	int ChannelLen = TestChannel.getSize(); //ä�� ����

	//ä�� �̸��� ������� ä���� ã��
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
		//�ش� ä���� �޽����� ��������
		const Common::JVector<Common::Object> ChannelMessages = targetChannel->getMessages();
		int MessageLen = ChannelMessages.getSize();

		//���� ä�� ���
		UE_LOG(LogTemp, Log, TEXT("Current Channel : %s"), *channelName);

		//ä�ο� ���� �޽����� ����
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
			//���� �޽��� �ʱ�ȭ
			CurrentMessagesArray->Empty();
			for (int j = 0; j < MessageLen; j++)
			{
				if (ChannelMessages[j].getType() == ExitGames::Common::TypeCode::STRING)
				{
					//���� �޽��� FString���� ��ȯ
					ExitGames::Common::JString Check = ExitGames::Common::ValueObject<ExitGames::Common::JString>(ChannelMessages[j]).getDataCopy();
					FString ReceivedMessage = FString(UTF8_TO_TCHAR(Check.UTF8Representation().cstr()));

					//���� �޽��� �迭�� ����
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
// �Ϲ� �޼���
void AActor_PhotonChat::SendMessageEvent(const FString& ChannelName, const FString& Message)
{
	if (m_pChatClient)
	{
		//�޽��� ���� �õ�
		if (m_pChatClient->opPublishMessage(TCHAR_TO_UTF8(*ChannelName), TCHAR_TO_UTF8(*Message)))
		{
			//�޽��� ���� ����
			UE_LOG(LogTemp, Log, TEXT("Message sent successfully."));
		}
		else
		{
			//�޽��� ���� ����
			UE_LOG(LogTemp, Error, TEXT("Failed to send message."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Chat client is not initialized."));
	}
	
}
//�ӼӸ�
void AActor_PhotonChat::SendPrivateMessage(const FString& username, const FString& message)
{
	m_pChatClient->opSendPrivateMessage(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*message));
}
//ä�� Ÿ�� ����
void AActor_PhotonChat::setTargetSubscribe(enum_PhotonChat target)
{
	if (target == enum_PhotonChat::World)
	{
		targetSubscribe = WorldChannel;
	}
	else {
		targetSubscribe = m_RoomName + "_" + FString::FromInt((int)target);
	}
	// ���濡 ���� UI ����
	// ChangeTargetSubscribe(target);
}
//ê ���� �Ϸ�
void AActor_PhotonChat::Chat_ConnectComplete(void)
{
	//�� �迭 ����
	JiwonRoom.Add("Global"); //��ü
	JiwonRoom.Add("Team"); //�� 
	JiwonRoom.Add("Private"); //����

	// Blueprint
	Blueprint_ConnectComplete(); // ���̽�ä�� ��������
	AddSubscribeEvent(JiwonRoom);
}
//�޼��� ���
void AActor_PhotonChat::Chat_getMessageEvent(const FString& ChannelName, const FString& sender, const FString& Message)
{
	// �޼��� ���� ���
	Blueprint_getMessageEvent(ChannelName, sender, Message);
}
//������ �߰� 
void AActor_PhotonChat::Chat_AddSubscribe(const FString& Channel)
{
	SubscribeList.AddUnique(Channel);
}
//������ ����
void AActor_PhotonChat::Chat_RemoveSubscribe(const FString& Channel)
{
	SubscribeList.Remove(Channel);
}
//ä�� ����
void AActor_PhotonChat::Chat_ResetJoinChannel(const FString& FullName)
{
	//��������Ʈ ������ �ִٸ� ���� �� �ʱ�ȭ
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
		FString ChannelName = m_RoomName + "_" + FString::FromInt(i); // �Ϲ�, �ӼӸ� ...
		SubscribeList.Add(ChannelName);
	}
	FString WorldName = WorldChannel;
	SubscribeList.Add(WorldName);
	AddSubscribeEvent(SubscribeList);

	// ����Ʈ ä�� ����
	setTargetSubscribe(enum_PhotonChat::Public);
}
