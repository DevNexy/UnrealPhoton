This is a guide on how to integrate Photon with an existing UE4 project

1. Only UE4 'C++' projects are supported.

2. Unpack the Photon SDK of your choice (Windows, Android or iOS) in 'Photon' folder inside 'Source' folder of UE4 project.
Only the header files and pre-built libraries are required. You may want to add libraries for multiple different platforms.
Sample folders layout:
\---Source
	+---Photon
	|	+---Common-cpp
	|	|	\---inc
	|	|		(*.h)
	|	+---LoadBalancing-cpp
	|	|	\---inc
	|	|		(*.h)
	|	|---Photon-cpp
	|	|	\---inc
	|	|		(*.h)
	|	+---lib
	|	|	+---Android 
	|	|		(*.a)
	|	|	+---iOS
	|	|		(*.a)
	|	|	\---Windows
	|	|		(*.lib)

3. Modify the Photon headers to workaround an incompatibility with the UE4 headers (rename FLOAT to EG_FLOAT, or apply Source\Photon\photon.patch from the reference project):

Photon/Common-cpp/inc/Enums/TypeCode.h
===================================================================
@@ -18,7 +18,7 @@
-			static const nByte FLOAT              = 'f'; /**<float*/
+			static const nByte EG_FLOAT           = 'f'; /**<float*/

Photon/Common-cpp/inc/Helpers/ConfirmAllowed.h
===================================================================
@@ -76,7 +76,7 @@
-				static const nByte typeName = TypeCode::FLOAT;
+				static const nByte typeName = TypeCode::EG_FLOAT;

Photon/Common-cpp/inc/Helpers/ConfirmAllowedKey.h
===================================================================
@@ -43,7 +43,7 @@
-				static const nByte typeName = TypeCode::FLOAT;
+				static const nByte typeName = TypeCode::EG_FLOAT;

4. Modify the Photon headers to fix an RTTI detection issue that would otherwise appear when compiling with UE4:

Photon/Common-cpp/inc/Helpers/TypeName.h
===================================================================
@@ -14,7 +14,7 @@
-#if defined _CPPRTTI || defined __GXX_RTTI || (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 3 || (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ < 2))))
+#if defined _CPPRTTI || defined __GXX_RTTI 

5. Edit the *.Build.cs file of the UE4 project to load the libraries for a given platform and to set Photons platform defines (see Source/PhotonDemoParticle/PhotonDemoParticle.Build.cs in the reference project and UE4 documentation):
..
	private string PhotonPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "..", "Photon")); }
	}
..
		if ( Target.Platform == UnrealTargetPlatform.Android)
		{
			Definitions.Add("_EG_ANDROID_PLATFORM");
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "libcommon-cpp-static_debug_android_armeabi_no-rtti.a")); 
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "libphoton-cpp-static_debug_android_armeabi_no-rtti.a")); 
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "libloadbalancing-cpp-static_debug_android_armeabi_no-rtti.a")); 
		}
..
Set _EG_WINDOWS_PLATFORM for Windows and _EG_IPHONE_PLATFORM for iOS

6. Include the appropriate Photon API headers in project sources with some workarounds (Source\photon-import.h in the reference project):
#ifdef __clang__
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#if _EG_WINDOWS_PLATFORM
#include "AllowWindowsPlatformTypes.h"
#endif
#pragma warning (disable: 4263)
#pragma warning (disable: 4264)
#include "LoadBalancing-cpp/inc/Client.h"
#pragma warning (default: 4263)
#pragma warning (default: 4264)
#if _EG_WINDOWS_PLATFORM
#include "HideWindowsPlatformTypes.h"
#endif

6. Use the imported Photon API in the source code of your project.

7. Build your UE4 project for your platform of choice.

Notes:
- Some hints regarding UE4 iOS builds can be found at https://answers.unrealengine.com/questions/21222/steps-for-ios-build-with-unrealremotetool.html