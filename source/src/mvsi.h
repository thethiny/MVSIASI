#pragma once
#include "../includes.h"
#include "../utils/Trampoline.h"
#include <unordered_map>
#include "mvs.h"
#include "mvsiutils.h"

using namespace UE;

namespace MVSI {

	extern uint64_t EXEHash;

	namespace Proxies {
		//__int64									__fastcall	ReadFString(__int64, __int64);
		//MVSGame::FName*							__fastcall	ReadFNameToWStr(MVSGame::FName&, char*);
		HANDLE									__stdcall	CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
		const char**							__fastcall	OverrideGameEndpoint(int64_t*, const char*);
		int64_t*								__fastcall	OverrideProdEndpoint(int64_t*, const wchar_t*);
		bool									__fastcall  MVSIOfflineModeChecker(int32_t*);
		MVSGame::UFighterGameInstance*			__fastcall CopyFighterInstance(MVSGame::UFighterGameInstance*, uint64_t*);
	};

	namespace Hooks {
		bool DisableSignatureCheck(Trampoline*);
		bool OverrideGameEndpointsData(Trampoline*);
		bool OverrideProdEndpointsData(Trampoline*);
		bool PatchSunsetSetterIntoMVSIChecker(Trampoline*);
		bool DialogHooks(Trampoline*);
		bool HookUEFuncs(Trampoline*);
	};

	extern bool IsMVSIUpdateRequired;

	template<typename T>
	MVSGame::UMvsDialog* ShowDialog(const T* promptText, const T* description = nullptr, const T* button1 = nullptr, const T* button2 = nullptr, const T* button3 = nullptr, const int selectedButton = -1, bool ShowExitButton = false, bool ShowSpinner = false, bool ShowSolidBackground = true, bool HideActionBar = false);
}

namespace HookMetadata { //Namespace for helpers for game functions
	struct ActiveMods {
		bool bAntiSigCheck		= false;
		bool bGameEndpointSwap	= false;
		bool bProdEndpointSwap  = false;
		bool bSunsetDate		= false;
		bool bUEFuncs			= false;
		bool bDialog			= false;
	};

	struct LibMapsStruct {
		LibFuncStruct ModLoader;
		LibFuncStruct AntiCheatEngine;
		LibFuncStruct CurlSetOpt;
		LibFuncStruct CurlPerform;
	};

	extern ActiveMods			sActiveMods;
	extern LibMapsStruct		sLFS;
	extern HHOOK				KeyboardProcHook;
	extern HMODULE				CurrentDllModule;
	extern HANDLE				Console;
};