// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_PhotonAudioOut.h"
#include "Components/AudioComponent.h"
#include "PhotonVoice-cpp/inc/AudioOutDelayControl.h"

//1 - 빨리 감기 요청 시 전체 SoundWaveProcedural 버퍼 폐기, 0 - 입력 스트림에서 필요한 샘플 수 건너뛰기
#define FF_BY_BUFFER_RESET 0 // 1 - discard entire SoundWaveProcedural buffer when asked for fast forward, 0 - skip required number of samples in input stream

//재생 위치 및 기타 동기화 매개변수 인쇄
#define TRACE_SYNC 0 // print play position and other sync parameters

class AudioOut :public ExitGames::Voice::AudioOutDelayControl<short>
{
public:
	AudioOut(UAudioComponent* audioComponent, const ExitGames::Voice::ILogger& logger, ExitGames::Voice::AudioOutDelayConfig delayConfig, const ExitGames::Common::JString& logPrefix, bool debuginfo);
	// AudioOutDelayControl
	virtual int64 outGetPos(void) const;
	virtual void outCreate(int frequency, int channels, int bufferSamples);
	virtual void outStart(void);
	virtual void outWrite(ExitGames::Voice::Buffer<short> buf, int offsetSamples);
private:
	UAudioComponent* mpAudioComponent;
	UProcedural* mpProcedural;
};
AudioOut::AudioOut(UAudioComponent* audioComponent, const ExitGames::Voice::ILogger& logger, ExitGames::Voice::AudioOutDelayConfig delayConfig, const ExitGames::Common::JString& logPrefix, bool debugInfo)
	:AudioOutDelayControl(logger, delayConfig, logPrefix, debugInfo)
	, mpAudioComponent(audioComponent)
	, mpProcedural(NULL)
{

}
void AudioOut::outCreate(int frequency, int channels, int bufSamples)
{
	mpProcedural = NewObject<UProcedural>();
	mpProcedural->SetSampleRate(frequency);
	mpProcedural->NumChannels = channels;
	mpProcedural->Duration = INDEFINITELY_LOOPING_DURATION;
	mpProcedural->SoundGroup = SOUNDGROUP_Voice;
	mpProcedural->bLooping = false;
	mpProcedural->bProcedural = true;

	mpProcedural->init(bufSamples);
	mpProcedural->SoundClassObject = LoadObject<USoundClass>(nullptr, TEXT("SoundClass'/Geme/Sound/Voice.Voice'"));
	mpAudioComponent->SetSound(mpProcedural);
}

void AudioOut::outStart(void)
{
	mpAudioComponent->Play();
}

void AudioOut::outWrite(ExitGames::Voice::Buffer<short> buf, int offsetSamples)
{
	mpProcedural->write(buf, offsetSamples);
	if (GEngine && buf.getSize() > 10)
	{
		short* b = buf.getArray();
		//GEngine->AddOnScreenDebugMessage(3, 15.0f, FColor::Cyan, *FString::Printf(TEXT("AAudioOut push: %d: %d %d %d %d %d %d %d %d %d %d"), buf.getSize(), b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9]));
	}
}
int64 AudioOut::outGetPos(void) const
{
	return mpProcedural->getReadPos();
}
//사운드 웨이브
UProcedural::UProcedural(const FObjectInitializer& ObjectInitializer)
	:USoundWaveProcedural(ObjectInitializer)
	, mReadPos(0) 
{
	VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
}
void UProcedural::init(int bufSize)
{
	mRingBuf = ExitGames::Voice::Buffer<short>(bufSize * this->NumChannels);
	mReadPos = 0;
}
void UProcedural::write(ExitGames::Voice::Buffer<short>& buf, int offsetSamples)
{
	int offset = offsetSamples * this->NumChannels;
	int p2 = offset + buf.getSize() - mRingBuf.getSize();
	if (p2 > 0)
	{
		int p1 = mRingBuf.getSize() - offset;
		memcpy(mRingBuf.getArray() + offset, buf.getArray(), p1 * sizeof(short));
		memcpy(mRingBuf.getArray(), buf.getArray() + p1, p2 * sizeof(short));
	}
	else
	{
		memcpy(mRingBuf.getArray() + offset, buf.getArray(), buf.getSize() * sizeof(short));
	}
}
int64 UProcedural::getReadPos(void)
{
	return mReadPos % mRingBuf.getSize() / this->NumChannels;
}
int32 UProcedural::OnGeneratePCMAudio(TArray <uint8>& outAudio, int32 sizeSamples)
{
	outAudio.Reset();
	int size = sizeSamples;
	outAudio.AddZeroed(size * sizeof(short));
	short* buf = (short*)outAudio.GetData();
	int offset = mReadPos % mRingBuf.getSize();
	int p2 = offset + size - mRingBuf.getSize();
	if (p2 > 0)
	{
		int p1 = mRingBuf.getSize() - offset;
		memcpy(buf, mRingBuf.getArray() + offset, p1 * sizeof(short));
		memcpy(buf + p1, mRingBuf.getArray(), p2 * sizeof(short));
	}
	else
		memcpy(buf, mRingBuf.getArray() + offset, size * sizeof(short));

	mReadPos += size;
	return sizeSamples;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// AActor_PhotonAudioOut
// Sets default values
AActor_PhotonAudioOut::AActor_PhotonAudioOut()
	: mpPlayer(NULL)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	mpAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PhotonVoiceAudioComponent"));
	mpAudioComponent->SetupAttachment(RootComponent);
	SetRootComponent(mpAudioComponent);

	mpAudioComponent->bAutoActivate = true;
	mpAudioComponent->bAlwaysPlay = true;
}
// Called when the game starts or when spawned
void AActor_PhotonAudioOut::BeginPlay()
{
	Super::BeginPlay();

}
void AActor_PhotonAudioOut::init(const ExitGames::Voice::ILogger& logger, ExitGames::Voice::AudioOutDelayConfig delayConfig, const ExitGames::Common::JString& logPrefix, bool debugInfo)
{
	mpPlayer = new AudioOut(mpAudioComponent, logger, delayConfig, logPrefix, debugInfo);
}

void AActor_PhotonAudioOut::finit(void)
{
	delete mpPlayer;
}

ExitGames::Voice::IAudioOut<short>* AActor_PhotonAudioOut::getPlayer(void) const
{
	return mpPlayer;
}
void AActor_PhotonAudioOut::SetMute(bool bInput)
{
	bMmute = bInput;
	if (mpAudioComponent)
	{
		mpAudioComponent->SetActive(!bMmute);
	}
}
