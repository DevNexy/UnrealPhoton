## UnrealPhoton(Cloud, Chat, Voice) - 언리얼 포톤(클라우드, 챗, 보이스)
## Developed using UE5 and Photon SDK 

![image](https://github.com/DevNexy/UnrealPhoton/assets/92451281/c4cd10ea-a005-45f1-a5a3-2cf6e68ed78e)
[PhotonChat 시연](https://youtu.be/f85KT7glvdM)

---
<요약>
- Photon SDK를 활용한 채널 별 텍스트 채팅, 보이스 채팅 기능 구현
- 개인 프로젝트   
[Unreal Photon SDK](https://www.photonengine.com/sdks#realtime-sdkrealtimewindows)   
[Photon API Doc](https://doc-api.photonengine.com/en/cpp/current/index.html)


<성과>
- SDK를 이용하여 Photon 서버를 다룰 수 있는 시간이었습니다.
- 라이브러리 및 모듈 사용 방법에 대해 깊게 공부할 수 있었습니다.
  
---
<콘텐츠 소개>
- 게임에서 필수적인 텍스트 채팅과 보이스 채팅
- 채널 별로 채팅 가능

<Actor_PhotonChat.cpp>   
(주요 기능을 하는 일부 코드만 가져왔습니다.)   
- 생성자에 Chat AppID를 통해 클라이언트 연결
```cpp
#define CHAT_AppID "7906ca2a-48b7-4ac7-8763-8ebcf7112886"
// Sets default values
AActor_PhotonChat::AActor_PhotonChat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pListener = new PhotonChatListener(this);
	m_pChatClient = new ExitGames::Chat::Client(*m_pListener, Common::JString(CHAT_AppID), Common::JString("ns.photonengine.io"));
	m_pListener->setChatClient(m_pChatClient);
}
```
- 룸 생성
```cpp
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
```
- 채널 추가
```cpp
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
```
- 채널별 받은 메세지 출력
```cpp
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
```
- 메세지 전송 함수
```cpp
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
```

---
<오류 해결>
- 채널 정렬 오류 문제
  - 버그 현상 : 코드에서 채널을 인덱스 형식으로 받아와서 채널에 접근 및 출력했는데, 기존에 받은 메세지를 출력할 때 순서가 달라지는 현상 발생
  - 원인 : 채널을 인덱스 형식으로 구별하면 순서에 의존하기 때문에 문제가 발생
  - 해결 방법 : 채널의 순서에 따라 메시지를 올바르게 출력하기 위해 채널을 이름 또는 다른 고유한 식별자로 구별 하였습니다.
- SDK 및 라이브러리 설정 문제
  - 버그 현상 : SDK 및 라이브러리 설정 문제로 인한 링커 오류 발생
  - 해결 방법 : Build.cs 파일에서 포톤 경로 수정 및 라이브러리 추가 (코드 참고)

(Build.cs 파일의 일부 코드만 가져왔습니다.)
```cs
//플러그인 추가
public PhotonDemoParticle(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "PhotonDemoParticle.h";
		MinFilesUsingPrecompiledHeaderOverride = 1;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Voice",
			// 외부 플러그인
			"AudioExtensions",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
		LoadPhoton(Target);
	}

//포톤 경로
private string PhotonPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "..", "Photon")); }
	}

//라이브러리 추가
private readonly string[] libraries = { "Common", "Photon", "LoadBalancing", "Chat", "PhotonVoice" };

//플랫폼에 따른 라이브러리 로드
public bool LoadPhoton(ReadOnlyTargetRules Target)
	{
	#if UE_5_0_OR_LATER
		if (Target.Platform == UnrealTargetPlatform.Win64)
	#else
		if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
	#endif
		{
			PublicDefinitions.Add("_EG_WINDOWS_PLATFORM");
			for(int i = 0; i< libraries.Length; i++)
            {
				AddPhotonLibPathWin(Target, libraries[i]);

			}
			// 보이스챗 추가 라이브러리
			string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows", "opus_egpv_vc16_release_windows_md_" + PlatformString + ".lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicDefinitions.Add("_EG_ANDROID_PLATFORM");
			AddPhotonLibPathAndroid(Target, "common-cpp-static");
			AddPhotonLibPathAndroid(Target, "photon-cpp-static");
			AddPhotonLibPathAndroid(Target, "loadbalancing-cpp-static");
			AddPhotonLibPathAndroid(Target, "ssl");
			AddPhotonLibPathAndroid(Target, "websockets");
			AddPhotonLibPathAndroid(Target, "crypto");
		}
		else
		{
			throw new Exception("\nTarget platform not supported: " + Target.Platform);
		}

		// Include path
		PublicIncludePaths.Add(Path.Combine(PhotonPath, "."));

		return true;
	}
```
