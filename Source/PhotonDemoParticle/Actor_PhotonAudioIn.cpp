// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_PhotonAudioIn.h"
#include "Runtime/Launch/Resources/Version.h"

// Sets default values
AActor_PhotonAudioIn::AActor_PhotonAudioIn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_PhotonAudioIn::BeginPlay()
{
	Super::BeginPlay();
	
#if ENGINE_MAJOR_VERSION < 5 && ENGINE_MINOR_VERSION < 25
	mspCapture = FVoiceModule::Get().CreateVoiceCapture();
#else
	mspCapture = FVoiceModule::Get().CreateVoiceCapture(TEXT("")); //빈 문자열은 기본 음성 캡처 장치가 된다.
#endif
	if (mspCapture)
		mspCapture->Start();
	GetWorldTimerManager().SetTimer(mTimer, this, &AActor_PhotonAudioIn::onTimer, 0.01f, true);

	IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionThreshold"))->Set(0);

	//처음엔 사운드 비활성화 상태
	mute = true;
}
//음성 캡처 값들을 추출
void AActor_PhotonAudioIn::onTimer()
{
	if (!mpCallback)
		return;
	uint32 bytesAvailable = 0;
	if (!mspCapture)
		return;

	EVoiceCaptureState::Type captureState = mspCapture->GetCaptureState(bytesAvailable);

	if (captureState == EVoiceCaptureState::Ok && bytesAvailable > 0)
	{
		ExitGames::Voice::Buffer<short> b(bytesAvailable / sizeof(short));

		//마이크 입력 평균값
		aveMike = 0.0f;

		uint32 readBytes = 0;
		short* buf = b.getArray();

		if (mspCapture)
			mspCapture->GetVoiceData(reinterpret_cast<uint8*>(buf), bytesAvailable / sizeof(short) * sizeof(short), readBytes);
		if (!readBytes)
			return;

		if (readBytes > 10)
		{
			AverageMike(buf);
			
			//음소거 상태면 비어있는 버퍼를 보내줌
			if (mute) 
			{
				mpCallback(mpCallbackOpaque, b.getEmpty());
			}
			else
			{
				mpCallback(mpCallbackOpaque, b);
			}
		}
	}
}
void AActor_PhotonAudioIn::setCallback(void* callbackOpaque, void(*callback)(void*, const ExitGames::Voice::Buffer<short>&))
{
	mpCallbackOpaque = callbackOpaque;
	mpCallback = callback;
}
//마이크 입력의 임의(10)의 평균값을 구한다.
void AActor_PhotonAudioIn::AverageMike(const short* buf)
{
	for (int i = 0; i < 10; i++)
	{
		aveMike += buf[i];
	}
	aveMike /= 10;
}

// AActor_PhotonAudioIn
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// AudioIn // Photon Audio Sample 
AudioIn::AudioIn(IAudioInFactory* factory)
	: mpAudioInFactory(factory)
	, mpActor(factory->createAudioIn())
{
}

AudioIn::~AudioIn()
{
	mpAudioInFactory->destroyAudioIn(mpActor);
}

void AudioIn::setCallback(void* callbackOpaque, void(*callback)(void*, const ExitGames::Voice::Buffer<short>&))
{
	mpActor->setCallback(callbackOpaque, callback);
}

int AudioIn::getSamplingRate(void) const
{
	return UVOIPStatics::GetVoiceSampleRate();
}

int AudioIn::getChannels(void) const
{
#if ENGINE_MAJOR_VERSION <5 && ENGINE_MINOR_VERSION < 25
	return DEFAULT_NUM_VOICE_CHANNELS;
#else
	return UVOIPStatics::GetVoiceNumChannels();
#endif
}

ExitGames::Common::JString AudioIn::getError(void) const
{
	return mError;
}

ExitGames::Common::JString& AudioIn::toString(ExitGames::Common::JString& retStr, bool withTypes) const
{
	return retStr += L"{" + ExitGames::Common::Helpers::TypeName::get(this) + L"}";
}

void AudioIn::MuteInputSound(bool bMute)
{
	if (mpActor)
	{
		mpActor->mute = bMute;
	}
}
