// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Player.h"
#include "demo_particle_common/LoadBalancingListener.h"
#include "GameFramework/Actor.h"
#include "Actor_Photon.generated.h"

UCLASS()
class PHOTONDEMOPARTICLE_API AActor_Photon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_Photon();

	// 포톤 서버 접속 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString serverAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString AppID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString appVersion = FString("1.2.0");

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//포톤 클라이언트
	ExitGames::LoadBalancing::Client* m_pClient;
	LoadBalancingListener* m_pListener;

	//플레이어 유저
	class APawn_Player* LocalPlayer;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	//UFUNCTION(BlueprintCallable)
		//void ConnectLogin(const FString& username);

	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_RemovePlayer(int PlayerNr);
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_AddPlayer(int PlayerNr, const FString& name);
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_ResetPlayer();

};
