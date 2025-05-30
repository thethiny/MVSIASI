#include "eSettingsManager.h"
#include <Windows.h>

eSettingsManager* SettingsMgr = new eSettingsManager();
eFirstRunManager* FirstRunMgr = new eFirstRunManager;

void eFirstRunManager::Init()
{
	ini = new CIniReader("mvsi_state.ini");

	bPaidModWarned = ini->ReadBoolean("FirstRun", "PaidModWarned", false);

	if (!bPaidModWarned)
	{
		Save(); // Save before displaying message box since some users complaining about a crash
		MessageBoxA(0,
			"Please note that MVS Infinite is a FREE fan revival mod for MVS that is meant to be used with free content.\n"
			"If you have paid for anything, ask for a refund.",
			"MVSI Installed",
			MB_ICONEXCLAMATION
		);
	}
}

void eFirstRunManager::Save()
{
	ini->WriteBoolean("FirstRun", "PaidModWarned", true);
}

void eSettingsManager::Init()
{
	CIniReader ini("");

	// Debug Settings
	bEnableConsoleWindow		= ini.ReadBoolean	("Settings.Debug",		"ShowConsole",				false);
	bPauseOnStart				= ini.ReadBoolean	("Settings.Debug",		"DebugPause",				false);
	bDebug						= ini.ReadBoolean	("Settings.Debug",		"DebugLogging",				false);
	bAllowNonMK					= ini.ReadBoolean	("Settings.Deubg",		"NonMVSPatching",			false);
	
	// Settings
	iLogSize					= ini.ReadInteger	("Settings",			"LogSize",					50);
	iLogLevel					= ini.ReadInteger	("Settings",			"LogLevel",				0);
	szModLoader					= ini.ReadString	("Settings",			"ModLoader",				"Kernel32.CreateFileW");
	szAntiCheatEngine			= ini.ReadString	("Settings",			"AntiCheatEngine",		"User32.EnumChildWindows");
	szCurlSetOpt				= ini.ReadString	("Settings",			"CurlSetOpt",				"libcurl.curl_easy_setopt");
	szCurlPerform				= ini.ReadString	("Settings",			"CurlPerform",			"libcurl.curl_easy_perform");
	bEnableKeyboardHotkeys		= ini.ReadBoolean	("Settings",			"EnableKeyboardHotkeys",	true);
	// Keybinds
	hkMenu						= ini.ReadString	("Settings.Keybinds",	"ToggleMenu",				"F1");

	// Patches
	bSunsetDate					= ini.ReadBoolean   ("Patches",             "SunsetDate",              true);
	bDisableSignatureCheck		= ini.ReadBoolean	("Patches",				"PakLoader",			   true);

	// Patterns
	pSigCheck					= ini.ReadString	("Patterns",			"SigCheck",					"");
	pEndpointLoader				= ini.ReadString	("Patterns",			"EndpointLoader",			"");
	pProdEndpointLoader			= ini.ReadString	("Patterns",			"ProdEndpointLoader",		"");
	pSunsetDate					= ini.ReadString	("Patterns",			"SunsetDate",				"");


	

	// Private Server
	szServerUrl					= ini.ReadString	("Server.Game",			"ServerUrl",				"");
	szProdServerUrl				= ini.ReadString	("Server.Prod",			"ServerUrl",				"");
	bEnableServerProxy			= ini.ReadBoolean	("Server.Game",			"Enabled",					false);
	bEnableProdServerProxy		= ini.ReadBoolean	("Server.Prod",			"Enabled",					false);
}
