#include "includes.h"
#include "src/mkutils.h"
#include "src/mk12.h"
#include "src/mk12hook.h"
#include "mk12sdk/sdk.h"
#include <tlhelp32.h> 
#include <VersionHelpers.h>

constexpr const char * CURRENT_HOOK_VERSION = "0.0.2";

Trampoline* GameTramp, * User32Tramp;

void CreateConsole();
void SpawnError(const char*);
void PreGameHooks();
void ProcessSettings();
bool OnInitializeHook();

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	bool state = lParam >> 31, transition = lParam & 0x40000000;
	auto RepeatCount = LOWORD(lParam);

	return CallNextHookEx(0, code, wParam, lParam);
}

void CreateConsole()
{
	FreeConsole();
	AllocConsole();

	FILE* fNull;
	freopen_s(&fNull, "CONOUT$", "w", stdout);
	freopen_s(&fNull, "CONOUT$", "w", stderr);

	std::string consoleName = "MVS Infinite Console";
	SetConsoleTitleA(consoleName.c_str());
	HookMetadata::Console = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(HookMetadata::Console, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(HookMetadata::Console, dwMode);

	printfCyan("MVSI - Maintained by ");
	printfInfo("MVSI Team");
	printfGreen("ESettingsManager::bEnableConsoleWindow = true\n");
}

void PreGameHooks()
{
	GameTramp = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));
	if (SettingsMgr->iLogLevel)
		printf("Generated Trampolines\n");
	 IATable = ParsePEHeader();	 


	if (SettingsMgr->bDisableSignatureCheck)
	{
		HookMetadata::sActiveMods.bAntiSigCheck			= MK12Hook::Hooks::DisableSignatureCheck(GameTramp);
	}
	if (SettingsMgr->bEnableServerProxy)
	{
		HookMetadata::sActiveMods.bGameEndpointSwap		= MK12Hook::Hooks::OverrideGameEndpointsData(GameTramp);
	}
	if (SettingsMgr->bEnableProdServerProxy)
	{
		HookMetadata::sActiveMods.bProdEndpointSwap		= MK12Hook::Hooks::OverrideProdEndpointsData(GameTramp);
	}
	
}

void ProcessSettings()
{
	// KeyBinds
	SettingsMgr->iVKCheats				= StringToVK(SettingsMgr->hkCheats);
	SettingsMgr->iVKMenuToggle			= StringToVK(SettingsMgr->hkMenu);
	SettingsMgr->iVKMenuInfo			= StringToVK(SettingsMgr->hkInfo);

	// DLL Procs
	HookMetadata::sLFS.ModLoader		= ParseLibFunc(SettingsMgr->szModLoader);
	HookMetadata::sLFS.AntiCheatEngine	= ParseLibFunc(SettingsMgr->szAntiCheatEngine);
	HookMetadata::sLFS.CurlSetOpt		= ParseLibFunc(SettingsMgr->szCurlSetOpt);
	HookMetadata::sLFS.CurlPerform		= ParseLibFunc(SettingsMgr->szCurlPerform);

	printfCyan("Parsed Settings\n");
}

void SpawnError(const char* msg)
{
	MessageBoxA(NULL, msg, "MVSI", MB_ICONEXCLAMATION);
}

bool HandleWindowsVersion()
{
	if (IsWindows10OrGreater())
	{
		return true;
	}

	if (IsWindows7SP1OrGreater())
	{
		SpawnError("MVSI doesn't officially support Windows 8 or 7 SP1. It may misbehave.");
		return true;
	}

	SpawnError("MVSI doesn't support Windows 7 or Earlier. Might not work.");
	return true;

	
}

#include <string>

inline bool VerifyProcessName(std::string expected_process) {
	std::string process_name = GetProcessName();

	for (size_t i = 0; i < process_name.length(); ++i) {
		process_name[i] = std::tolower(process_name[i]);
	}

	return (process_name == expected_process);
}


bool OnInitializeHook()
{
	FirstRunMgr->Init();
	SettingsMgr->Init();

	if (!HandleWindowsVersion())
		return false;

	if (!SettingsMgr->bAllowNonMK && !(VerifyProcessName("MultiVersus-Win64-Shipping.exe") || !VerifyProcessName("MultiVersus.exe") || !VerifyProcessName("MVSI.exe")))
	{
		SpawnError("MVSI only works with MultiVersus!");
		return false;
	}

	if (SettingsMgr->bEnableConsoleWindow)
	{
		CreateConsole();
	}

	if (SettingsMgr->bEnableKeyboardHotkeys)
	{
		if (!(HookMetadata::KeyboardProcHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, HookMetadata::CurrentDllModule, GetCurrentThreadId())))
		{
			char x[100];
			sprintf(x, "Failed To Hook Keyboard FN: 0x%X", GetLastError());
			MessageBox(NULL, x, "Error", MB_ICONERROR);
		}
	}

	if (SettingsMgr->bPauseOnStart)
	{
		MessageBoxA(0, "Freezing Game Until OK", ":)", MB_ICONINFORMATION);
	}

	ProcessSettings(); // Parse Settings
	PreGameHooks(); // Queue Blocker

	return true;
}

// Plugin Stuff

const char* MK12HookPlugin::GetPluginName()
{
	return "MVSI";
}

const char* MK12HookPlugin::GetPluginProject()
{
	return "MVSI";
}

const char* MK12HookPlugin::GetPluginTabName()
{
	return "MVSI";
}

void MK12HookPlugin::OnInitialize()
{
	if (MK12HOOKSDK::IsOK())
		return;

	printfInfo("Called as an EHP Plugin");
	MK12HOOKSDK::Initialize();
}

void MK12HookPlugin::OnShutdown()
{
	if (HookMetadata::KeyboardProcHook) // Will be unloaded once by DLL, and once by EHP.
	{
		UnhookWindowsHookEx(HookMetadata::KeyboardProcHook);
		HookMetadata::KeyboardProcHook = nullptr;
	}
}

void MK12HookPlugin::OnFrameTick()
{
	// Things like async calls and polling tasks
}

void MK12HookPlugin::OnFightStartup()
{
	// not implemented yet
}

char* GetVersionedHookName()
{
	static char tabText[512];
	static bool bPrecalc = false;

	if (bPrecalc)
		return tabText;

	std::strcpy(tabText, MK12HookPlugin::GetPluginName());
	std::strcat(tabText, " Version ");
	std::strcat(tabText, CURRENT_HOOK_VERSION);
	bPrecalc = true;
	return tabText;
}

void MK12HookPlugin::TabFunction()
{
	if (!MK12HOOKSDK::IsOK())
		return;

	static int counter = 0;
	MK12HOOKSDK::ImGui_Text(GetVersionedHookName());

	if (MK12HOOKSDK::ImGui_CollapsingHeader("Patches"))
	{

		bool toggles[] = {
			HookMetadata::sActiveMods.bAntiSigCheck,
		};
		MK12HOOKSDK::ImGui_Checkbox("Pak Signature", &toggles[0]);

	}
	MK12HOOKSDK::ImGui_Separator();

}

// Dll Entry

bool APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpRes)
{
	HookMetadata::CurrentDllModule = hModule;
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		printfInfo("On Attach Initialize");
		OnInitializeHook();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		MK12HookPlugin::OnShutdown();
		break;
	}
	return true;
}