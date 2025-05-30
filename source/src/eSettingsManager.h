#pragma once
#include<string>
#include "..\IniReader.h"

class eSettingsManager {
public:
	void Init();

public:
	// Settings

	bool bEnableKeyboardHotkeys;

	// Debug
	bool bEnableConsoleWindow;
	bool bPauseOnStart;
	int	iLogLevel;
	bool bDebug;
	bool bAllowNonMK;


	// Toggles
	bool bSunsetDate;
	bool bDisableSignatureCheck;

	// Addresses

	// Patterns
	std::string pSigCheck;
	std::string pEndpointLoader;
	std::string pProdEndpointLoader;
	std::string pSunsetDate;


	// Menu Section
	std::string hkMenu;
	int iVKMenuToggle;

	//Other
	int iLogSize;
	bool FORCE_CHECK_VER = false;
	std::string szGameVer;
	std::string szModLoader;
	std::string szAntiCheatEngine;
	std::string szCurlSetOpt;
	std::string szCurlPerform;

	//Private Server
	std::string szServerUrl;
	bool bEnableServerProxy;
	// WB
	std::string szProdServerUrl;
	bool bEnableProdServerProxy;

};

class eFirstRunManager
{
public:
	void Init();
	void Save();

public:
	bool bPaidModWarned;

private:
	CIniReader* ini;
};

extern eSettingsManager* SettingsMgr;
extern eFirstRunManager* FirstRunMgr;