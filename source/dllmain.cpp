#include "includes.h"
#include "src/mvsiutils.h"
#include "src/mvs.h"
#include "src/mvsi.h"
#include <tlhelp32.h> 
#include <VersionHelpers.h>

constexpr const char * CURRENT_HOOK_VERSION = "0.1.1";
const char* MVSI::MVSI_Version = (const char*)CURRENT_HOOK_VERSION;

Trampoline* GameTramp, * User32Tramp;

void CreateConsole();
void SpawnError(const char*);
void PreGameHooks();
void ProcessSettings();
bool OnInitializeHook();
void SpawnP2PServer() {};
void DespawnP2PServer() {};

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	bool state = lParam >> 31, transition = lParam & 0x40000000;
	auto RepeatCount = LOWORD(lParam);

	if (code >= 0 && !state)
	{
		if (GetAsyncKeyState(VK_F1))
		{
			printf("Attempting F1 action\n");
			MVSI::ShowNotification("MVSI Loaded", CURRENT_HOOK_VERSION, 10.f, true);
		}

	}

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
		HookMetadata::sActiveMods.bAntiSigCheck			= MVSI::Hooks::DisableSignatureCheck(GameTramp);
	}
	if (SettingsMgr->bEnableServerProxy)
	{
		HookMetadata::sActiveMods.bGameEndpointSwap		= MVSI::Hooks::OverrideGameEndpointsData(GameTramp);
	}
	if (SettingsMgr->bEnableProdServerProxy)
	{
		HookMetadata::sActiveMods.bProdEndpointSwap		= MVSI::Hooks::OverrideProdEndpointsData(GameTramp);
	}
	if (SettingsMgr->bSunsetDate)
	{
		HookMetadata::sActiveMods.bSunsetDate			= MVSI::Hooks::PatchSunsetSetterIntoMVSIChecker(GameTramp);
	}
	if (SettingsMgr->bHookUE)
	{
		HookMetadata::sActiveMods.bUEFuncs				= MVSI::Hooks::HookUEFuncs(GameTramp);
	}
	if (SettingsMgr->bDialog)
	{
		HookMetadata::sActiveMods.bDialog				= MVSI::Hooks::DialogHooks(GameTramp);
	}
	if (SettingsMgr->bNotifs)
	{
		HookMetadata::sActiveMods.bNotifs = MVSI::Hooks::NotificationHooks(GameTramp);
	}
	
}

void ProcessSettings()
{
	// KeyBinds
	SettingsMgr->iVKMenuToggle			= StringToVK(SettingsMgr->hkMenu);

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

	for (size_t i = 0; i < expected_process.length(); ++i) {
		expected_process[i] = std::tolower(expected_process[i]);
	}

	return (process_name == expected_process);
}

void InitializeKeyboard()
{
	HookMetadata::KeyboardProcHook = SetWindowsHookEx(
		WH_KEYBOARD,
		KeyboardProc,
		HookMetadata::CurrentDllModule,
		GetCurrentThreadId()
	);
}

bool OnInitializeHook()
{
	FirstRunMgr->Init();
	SettingsMgr->Init();

	if (!SettingsMgr->bAllowNonMK && !(VerifyProcessName("MultiVersus-Win64-Shipping.exe") || VerifyProcessName("MultiVersus.exe") || VerifyProcessName("MVSI.exe") || VerifyProcessName("iDev.exe")))
	{
		SpawnError("MVSI only works with the original MVS steam game!");
		return false;
	}

	if (!HandleWindowsVersion())
		return false;

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

	// Hash Exe
	uint64_t EXEHash = HashTextSectionOfHost();
	CachedPatternsMgr->Init(EXEHash, CURRENT_HOOK_VERSION);

	ProcessSettings(); // Parse Settings
	PreGameHooks(); // Queue Blocker
	SpawnP2PServer();

	return true;
}

static void OnShutdown()
{
	if (HookMetadata::KeyboardProcHook) // Will be unloaded once by DLL, and once by EHP.
	{
		UnhookWindowsHookEx(HookMetadata::KeyboardProcHook);
		HookMetadata::KeyboardProcHook = nullptr;
	}

	DespawnP2PServer();
}

// Dll Entry

bool APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpRes)
{
	HookMetadata::CurrentDllModule = hModule;
	HHOOK hook_ = nullptr;
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		printfInfo("On Attach Initialize");
		OnInitializeHook();
		InitializeKeyboard();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		OnShutdown();
		break;
	}
	return true;
}