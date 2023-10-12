// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

// https://wiki.unrealengine.com/Linking_Static_Libraries_Using_The_Build_System

using System;
using System.IO;
using UnrealBuildTool;

public class PhotonDemoParticle : ModuleRules
{
	public PhotonDemoParticle(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "PhotonDemoParticle.h";
		MinFilesUsingPrecompiledHeaderOverride = 1;
		//bool bFasterWithoutUnity = true;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Voice",
			// 외부 플러그인
			"AudioExtensions",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.Add("Slate");

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }

		LoadPhoton(Target);
	}

	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string PhotonPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "..", "Photon")); }
	}

	private string PlatformString
	{
		get { return Target.Platform == UnrealTargetPlatform.Win64 ? "x64" : "Win32"; }
	}

	private delegate void AddLibFunction(ReadOnlyTargetRules target, string name);
	private readonly string[] libraries = { "Common", "Photon", "LoadBalancing", "Chat", "PhotonVoice"};
	private readonly string[] Phton3rdlibraries = { "crypto", "websockets", "ssl" };

	private readonly string[] PhotonlibArm = { "arm64-v8a_libc++.a" }; // "armeabi-v7a_libc++.a", 
	private void AddPhotonLibPathWin(ReadOnlyTargetRules Target, string name)
	{
		string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows",
			name + "-cpp_vc16_release_windows_md_" + PlatformString + ".lib"));
		/*
		#if UE_4_25_OR_LATER
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows", name + "-cpp_vc16_release_windows_md_" + PlatformString + ".lib"));
		#else
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows", name + "-cpp_vc15_release_windows_md_" + PlatformString + ".lib"));
		#endif*/
	}

	private void AddPhotonLibPathAndroid(ReadOnlyTargetRules Target, string name)
	{
		#if UE_4_25_OR_LATER
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "lib" + name + "_debug_android_armeabi-v7a_libc++_no-rtti.a"));
		#else
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "lib" + name + "_debug_android_armeabi-v7a_no-rtti.a"));
		#endif
	}
	/*
	private void AddPhotonLibPathIOS(ReadOnlyTargetRules Target, string name)
	{
		string archStr = (Target.Architecture == "-simulator") ? "iphonesimulator" : "iphoneos";
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "iOS", "lib" + name + "_debug_" + archStr + ".a"));
	}*/

	private void AddPhotonLibPathMac(ReadOnlyTargetRules Target, string name)
	{
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Mac", "lib" + name + "_debug_macosx.a"));
	}

	private void AddPhotonLibPathPS4(ReadOnlyTargetRules Target, string name)
	{
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "PS4", "lib" + name + "-cpp_debug_ps4_ORBIS.a"));
	}

	private void AddPhotonLibPathHTML5(ReadOnlyTargetRules Target, string name)
	{
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "HTML5", name + "-cpp_release_emscripten_Emscripten_unreal.bc"));
	}

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
}

