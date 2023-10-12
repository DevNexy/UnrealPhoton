// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_PhotonVoice.h"
#include "demo_particle_common/PhotonVoiceListener.h"
#include "GameFramework/Actor.h"
#include "demo_particle_common/Console.h"
#include "Actor_PhotonAudioIn.h"
#include "Actor_PhotonAudioOut.h"
#include "Kismet/GameplayStatics.h"

using namespace ExitGames::Voice;

#define Voice_AppID "84a80dab-ccdf-4ac6-9fac-35bf6df57456"

// Sets default values
AActor_PhotonVoice::AActor_PhotonVoice() :
	serverAddress(TEXT("ns.exitgames.com")), // ns.exitgames.com // ns.photonengine.io
	AppID(TEXT(Voice_AppID)),
	appVersion(TEXT("1.0.1")),
	mpPhotonLib(NULL)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_PhotonVoice::BeginPlay(void)
{
	Super::BeginPlay();
	srand(GETTIMEMS());
	mpPhotonLib = new PhotonVoiceListener(ExitGames::Common::JString(TCHAR_TO_UTF8(*AppID)), ExitGames::Common::JString(TCHAR_TO_UTF8(*appVersion)), this, this, this);
}

void AActor_PhotonVoice::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
	delete mpPhotonLib;
}

// Called every frame
void AActor_PhotonVoice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (mpPhotonLib)
	{
		mpPhotonLib->update();
	}

}
//����ũ ĸó, �ν�
AActor_PhotonAudioIn* AActor_PhotonVoice::createAudioIn()
{
	return mPhotonAudioIn->GetWorld()->SpawnActor<AActor_PhotonAudioIn>();
}

void AActor_PhotonVoice::destroyAudioIn(AActor_PhotonAudioIn* a)
{
	GetWorld()->DestroyActor(a);
}

// ���̽� ê ��� 
AActor_PhotonAudioOut* AActor_PhotonVoice::createAudioOut()
{
	mPhotonAudioOut.Add(GetWorld()->SpawnActor<AActor_PhotonAudioOut>());
	return mPhotonAudioOut[mPhotonAudioOut.Num() - 1];
}

void AActor_PhotonVoice::destroyAudioOut(AActor_PhotonAudioOut* a)
{
	mPhotonAudioOut.Remove(a);
	GetWorld()->DestroyActor(a);
}
// ���� ���̽� ����. Playfab ID �����ϱ�.
void AActor_PhotonVoice::Connect(const FString& Vociename)
{
	Console::get().writeLine(L"// Voice Connecting...");

	mpPhotonLib->connect(TCHAR_TO_UTF8(*Vociename));
}

void AActor_PhotonVoice::Disconnect()
{
	mpPhotonLib->disconnect();
}

//�� ���带 �������� ��۷� ��ȯ�Ѵ�.
void AActor_PhotonVoice::ToggleEcho()
{
	mpPhotonLib->toggleEcho();
}

//���濡 ������ �Ϸ������ ȣ�����ش�.
void AActor_PhotonVoice::Voice_ConnectComplete(void)
{
	ConnectComplete();
}

//���� �Էº� ���Ұ�
void AActor_PhotonVoice::SetMuteIn(bool bInput)
{
	if (mPhotonAudioIn)
	{
		mPhotonAudioIn->mute = bInput;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("//couldn't find the PhotonAudioIn."));
	}
}
//���� ��º� ���Ұ�
void AActor_PhotonVoice::SetMuteOut(bool bInput)
{
	if (mPhotonAudioOut.Num())
	{
		for (auto it : mPhotonAudioOut)
		{
			it->SetMute(bInput);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("//couldn't find the PhotonAudioOut."));
	}
}

//���� Ȱ��ȭ/��Ȱ��ȭ
void AActor_PhotonVoice::SetMute(bool bInput)
{
	if (mpPhotonLib->isconnected)
	{
		SetMuteIn(bInput);
		SetMuteOut(bInput);
	}
}

//���� Ȱ��ȭ/��Ȱ��ȭ
uint8 AActor_PhotonVoice::GetInMikeVolume()
{
	if (mPhotonAudioIn)
	{
		return (uint8)mPhotonAudioIn->aveMike;
	}
	return 0;
}

bool AActor_PhotonVoice::getIsChanging()
{
	return mpPhotonLib->IsChanging;
}

void AActor_PhotonVoice::setIsChanging(bool Change)
{
	mpPhotonLib->IsChanging = Change;
}

void AActor_PhotonVoice::Voice_ChangeOrJoinRoom(const FString& RoomFullName)
{
	mpPhotonLib->SetRoomName(RoomFullName);
	if (getIsChanging()) // change���̶�� leaveRoom
	{
		mpPhotonLib->mLoadBalancingClient.opLeaveRoom();
	}
	else
	{
		mpPhotonLib->JoinRoom();
	}
}

#if WITH_EDITOR
void AActor_PhotonVoice::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.Property) ? e.Property->GetFName() : NAME_None;
	//if(PropertyName == GET_MEMBER_NAME_CHECKED(AActor_PhotonVoice, automove))
	//	SetAutomove(automove);
	//else if(PropertyName == GET_MEMBER_NAME_CHECKED(AActor_PhotonVoice, useGroups))
	//	SetUseGroups(useGroups);
}
#endif