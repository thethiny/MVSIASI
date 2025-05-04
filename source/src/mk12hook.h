#pragma once
#include "../includes.h"
#include "../utils/Trampoline.h"
#include <unordered_map>
#include "mk12.h"
#include "mkutils.h"

namespace MK12Hook {
	namespace Proxies {
		__int64									__fastcall	ReadFString(__int64, __int64);
		MK12::FName*							__fastcall	ReadFNameToWStr(MK12::FName&, char*);
		HANDLE									__stdcall	CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
		const char**							__fastcall	OverrideGameEndpoint(int64_t*, const char*);
	};

	namespace Hooks {
		bool DisableSignatureCheck(Trampoline*);
		bool OverrideGameEndpointsData(Trampoline*);
	};
}

namespace HookMetadata { //Namespace for helpers for game functions
	struct ActiveMods {
		bool bAntiSigCheck		= false;
		bool bGameEndpointSwap	= false;
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