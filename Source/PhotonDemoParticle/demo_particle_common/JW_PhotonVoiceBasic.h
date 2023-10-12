// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Photon/Common-cpp/inc/Common.h"

class FString;

class PHOTONDEMOPARTICLE_API JW_PhotonVoiceBasic
{
public:
	virtual ~JW_PhotonVoiceBasic();

	// Connect
	virtual void Voice_ConnectComplete(void) {}
};
