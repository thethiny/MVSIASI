#pragma once
#include "../includes.h"
#include "../utils/Trampoline.h"
#include <unordered_map>
#include "mvs.h"
#include "mvsiutils.h"

namespace MVSI {
	namespace Proxies {
		//__int64									__fastcall	ReadFString(__int64, __int64);
		//MVSGame::FName*							__fastcall	ReadFNameToWStr(MVSGame::FName&, char*);
		HANDLE									__stdcall	CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
		const char**							__fastcall	OverrideGameEndpoint(int64_t*, const char*);
		int64_t*								__fastcall	OverrideProdEndpoint(int64_t*, const wchar_t*);
		bool									__fastcall  MVSIOfflineModeChecker(int32_t*);
	};

	namespace Hooks {
		bool DisableSignatureCheck(Trampoline*);
		bool OverrideGameEndpointsData(Trampoline*);
		bool OverrideProdEndpointsData(Trampoline*);
		bool PatchSunsetSetterIntoMVSIChecker(Trampoline*);
	};

	extern bool IsMVSIUpdateRequired;
}

namespace HookMetadata { //Namespace for helpers for game functions
	struct ActiveMods {
		bool bAntiSigCheck		= false;
		bool bGameEndpointSwap	= false;
		bool bProdEndpointSwap  = false;
		bool bSunsetDate		= false;
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