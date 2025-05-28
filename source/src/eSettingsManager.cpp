#include "eSettingsManager.h"
#include <Windows.h>

eSettingsManager* SettingsMgr = new eSettingsManager();
eFirstRunManager* FirstRunMgr = new eFirstRunManager;

void eFirstRunManager::Init()
{
	ini = new CIniReader("mvsi_state.ini");

	bPaidModWarned				= ini->ReadBoolean	("FirstRun",			"bPaidModWarned",			false);


	if (!bPaidModWarned)
	{
		Save(); // Save before displaying message box since some users complaining about a crash
		MessageBoxA(0, "Please note that MVS Infinite is a FREE revival mod for MultiVersus tool that is meant to be used with free content.\nIf you have paid for anything, ask for a refund.", "MVSI Installed", MB_ICONEXCLAMATION);
	}
}

void eFirstRunManager::Save()
{
	ini->WriteBoolean("FirstRun", "bPaidModWarned", true);
}

void eSettingsManager::Init()
{
	CIniReader ini("");

	// Debug Settings
	bEnableConsoleWindow		= ini.ReadBoolean	("Settings.Debug",		"bEnableConsoleWindow",		false);
	bPauseOnStart				= ini.ReadBoolean	("Settings.Debug",		"bPauseOnStart",			false);
	bDebug						= ini.ReadBoolean	("Settings.Debug",		"bDebug",					false);
	bAllowNonMK					= ini.ReadBoolean	("Settings.Deubg",		"bAllowNonMK",				false);
	
	// Settings
	iLogSize					= ini.ReadInteger	("Settings",			"iLogSize",					50);
	iLogLevel					= ini.ReadInteger	("Settings",			"iLogLevel",				0);
	szModLoader					= ini.ReadString	("Settings",			"szModLoader",				"Kernel32.CreateFileW");
	szAntiCheatEngine			= ini.ReadString	("Settings",			"szAntiCheatEngine",		"User32.EnumChildWindows");
	szCurlSetOpt				= ini.ReadString	("Settings",			"szCurlSetOpt",				"libcurl.curl_easy_setopt");
	szCurlPerform				= ini.ReadString	("Settings",			"szCurlPerform",			"libcurl.curl_easy_perform");
	bEnableKeyboardHotkeys		= ini.ReadBoolean	("Settings",			"bEnableKeyboardHotkeys",	true);
	// Patches

	// Patches.AntiCheat
	bDisableSignatureCheck		= ini.ReadBoolean	("Patches.AntiCheat",	"bDisableSignatureCheck",	true);

	// Patterns
	pSigCheck					= ini.ReadString	("Patterns",			"pSigCheck",				"");
	pEndpointLoader				= ini.ReadString	("Patterns",			"pEndpointLoader",			"");
	pProdEndpointLoader			= ini.ReadString	("Patterns",			"pWBEndpointLoader",		"");


	// Keybinds
	hkMenu						= ini.ReadString	("Keybinds",			"hkMenu",					"F1");
	hkInfo						= ini.ReadString	("Keybinds",			"hkInfo",					"TAB");
	hkCheats					= ini.ReadString	("Keybinds",			"hkCheats",					"F12");

	// Private Server
	szServerUrl					= ini.ReadString	("Server",				"szServerUrl",				"");
	bEnableServerProxy			= ini.ReadBoolean	("Server",				"bEnableServerProxy",		false);
	szProdServerUrl				= ini.ReadString	("Server.Prod",			"szProdServerUrl",			"");
	bEnableProdServerProxy		= ini.ReadBoolean	("Server.Prod",			"bEnableProdServerProxy",		false);
}
