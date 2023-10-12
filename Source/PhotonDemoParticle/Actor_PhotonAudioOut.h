// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/Voice/Public/VoiceModule.h"
#include "PhotonVoice-cpp/inc/AudioUtil.h"
#include "PhotonVoice-cpp/inc/Buffer.h"
#include "PhotonVoice-cpp/inc/AudioOutdelayControl.h"
#include "Actor_PhotonAudioOut.generated.h"

class IAudioOutFactory
{
public:
	virtual ~IAudioOutFactory(void){}
	virtual class AActor_PhotonAudioOut* createAudioOut(void) = 0;
	virtual void destroyAudioOut(class AActor_PhotonAudioOut*) = 0;
};
//사운드 출력 추가 클래스 정보
//사운드 웨이브 설정

UCLASS()
class PHOTONDEMOPARTICLE_API UProcedural :public USoundWaveProcedural
{
	GENERATED_BODY()
public:
	UProcedural(const FObjectInitializer& ObjectInitializer);

	void init(int ringBufSize);
	void write(ExitGames::Voice::Buffer<short>& buf, int offset);
	int64 getReadPos(void);
	
	virtual int32 OnGeneratePCMAudio(TArray <uint8>& OutAudio, int32 NumSamples) override;

protected:
	ExitGames::Voice::Buffer<short> mRingBuf;
	std::atomic<int64> mReadPos;
};
UCLASS()
class PHOTONDEMOPARTICLE_API AActor_PhotonAudioOut : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_PhotonAudioOut();
	void init(const ExitGames::Voice::ILogger& logger, ExitGames::Voice::AudioOutDelayConfig delayConfig, const ExitGames::Common::JString& logPrefix, bool debugInfo);
	void finit(void);
	ExitGames::Voice::IAudioOut<short>* getPlayer(void) const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UAudioComponent* mpAudioComponent;
	//포톤 보이스 플레이어 정보
	ExitGames::Voice::IAudioOut<short>* mpPlayer;
	bool bMmute = false;
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	void SetMute(bool bInput);
};