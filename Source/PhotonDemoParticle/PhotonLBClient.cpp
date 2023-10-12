// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "PhotonDemoParticle.h"
#include "PhotonLBClient.h"
#include "demo_particle_common/Console.h"
#include "Engine/Engine.h"

APhotonLBClient::APhotonLBClient(const class FObjectInitializer& PCIP)
	: Super(PCIP)
	, serverAddress(TEXT("ns.photonengine.io"))
	, AppID(TEXT("be76b0d3-65ee-4f44-b0fd-3e8b4e7ab6dd"))
	, appVersion(TEXT("1.0"))
	, automove(true)
	, useGroups(true)
	, mpListener(new LoadBalancingListener(this))
	, mpClient(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

APhotonLBClient::~APhotonLBClient(void)
{
	delete mpListener;
}

void APhotonLBClient::BeginPlay(void)
{
	Super::BeginPlay();
	srand(GETTIMEMS());
	mpClient = new ExitGames::LoadBalancing::Client(*mpListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion));
	mpListener->setLBC(mpClient);
	mpClient->setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	ExitGames::Common::Base::setListener(mpListener);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base

	Console::get().writeLine(L"Connecting...");
	Console::get().writeLine(L"appID is set to " + ExitGames::Common::JString(TCHAR_TO_UTF8(*AppID)));
	mpListener->connect(ExitGames::Common::JString(L"UR") + GETTIMEMS(), TCHAR_TO_UTF8(*serverAddress));
	createRoom();
}

void APhotonLBClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	mpListener->disconnect();
	mpListener->setLBC(NULL);
	delete mpClient;
	Super::EndPlay(EndPlayReason);
}

void APhotonLBClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(mpClient)
	{
		mpClient->service();
		mpListener->service();
	}
}

void APhotonLBClient::RandomizeColor(void)
{
	mpListener->changeRandomColor();
}

void APhotonLBClient::NextGridSize(void)
{
	mpListener->nextGridSize();
}

void APhotonLBClient::NewGame(void)
{
	mpListener->createRoom();
}

void APhotonLBClient::Leave(void)
{
	if (mpClient->getIsInGameRoom()) {
		mpClient->opLeaveRoom();
		UE_LOG(LogTemp, Log, TEXT("Leave!!!!!!!!"));
	}
}

void APhotonLBClient::JoinRoom(FString gameId)
{
	mpClient->opJoinRoom(TCHAR_TO_UTF8(*gameId));
}

void APhotonLBClient::SetLocalPlayerPos(int32 x, int32 y)
{
	mpListener->setLocalPlayerPos(x, y);
}

bool APhotonLBClient::GetAutomove() const
{
	return automove;
}

void APhotonLBClient::SetAutomove(bool am)
{
	UE_LOG(LogTemp, Log, TEXT("SetAutoMove!!!!!!!!!!!!! AutoMove : %d"), am);
	mpListener->setAutomove(automove=am);
}

bool APhotonLBClient::GetUseGroups() const
{
	return useGroups;
}

void APhotonLBClient::SetUseGroups(bool ug)
{
	mpListener->setUseGroups(useGroups=ug);
}

void APhotonLBClient::createRoom()
{
	mpListener->createRoom();
	UE_LOG(LogTemp, Log, TEXT("Tick :: CreateRoom"));
}

bool APhotonLBClient::IsInLobby(void) const
{
	return mpClient->getState() == ExitGames::LoadBalancing::PeerStates::JoinedLobby || mpClient->getState() == ExitGames::LoadBalancing::PeerStates::AuthenticatedComingFromGameserver;
}

bool APhotonLBClient::IsInRoom(void) const
{
	return mpClient->getState() == ExitGames::LoadBalancing::PeerStates::Joined;
}

#if WITH_EDITOR
void APhotonLBClient::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.Property) ? e.Property->GetFName() : NAME_None;
	if(PropertyName == GET_MEMBER_NAME_CHECKED(APhotonLBClient, automove))
		SetAutomove(automove);
	else if(PropertyName == GET_MEMBER_NAME_CHECKED(APhotonLBClient, useGroups))
		SetUseGroups(useGroups);
}
#endif