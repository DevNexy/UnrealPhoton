// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/Voice/Public/VoiceModule.h"
#include "PhotonVoice-cpp/inc/VoiceAudio.h"
#include "Actor_PhotonAudioIn.generated.h"

class IAudioInFactory
{
public:
	virtual ~IAudioInFactory() {}
	virtual class AActor_PhotonAudioIn* createAudioIn() = 0;
	virtual void destroyAudioIn(class AActor_PhotonAudioIn*) = 0;
};

class AudioIn : public ExitGames::Voice::IAudioPusher<short>
{
public:
	AudioIn(class IAudioInFactory*);
	virtual ~AudioIn();

	//IAudioPusher
	virtual void setCallback(void*, void(*)(void*, const ExitGames::Voice::Buffer<short>&));
	//1초 동안 샘플링 한 횟수를 얻는다
	virtual int getSamplingRate(void) const;
	//사운드 채널을 얻는다.
	virtual int getChannels(void) const;
	virtual ExitGames::Common::JString getError(void) const;
	virtual ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes = false) const;

public:
	virtual void MuteInputSound(bool bMute);
private:

	class IAudioInFactory* mpAudioInFactory;

	class AActor_PhotonAudioIn* mpActor;
	ExitGames::Common::JString mError;
};
UCLASS()
class PHOTONDEMOPARTICLE_API AActor_PhotonAudioIn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_PhotonAudioIn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	//캡처한 사운드
	TSharedPtr<IVoiceCapture> mspCapture;
	FTimerHandle mTimer;

	//mute
	bool mute = false;

	//콜백함수
	void setCallback(void* callbackOpaque, void(*callback)(void*, const ExitGames::Voice::Buffer<short>&));

	//마이크 입력 평균값
	short aveMike = 0.0f;

private:
	//타이머 쓰레드: 계속 입력된 사운드를 캡처해서 갱신
	void onTimer();

	void* mpCallbackOpaque;
	void(*mpCallback)(void*, const ExitGames::Voice::Buffer<short>&);
	ExitGames::Common::JString mError;

	void AverageMike(const short* buf);
};
