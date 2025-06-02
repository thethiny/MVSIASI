#include "mvsi.h"


HookMetadata::ActiveMods			HookMetadata::sActiveMods;
HookMetadata::LibMapsStruct			HookMetadata::sLFS;
bool MVSI::IsMVSIUpdateRequired = false;

namespace MVSI::Proxies {

	HANDLE __stdcall CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		//if (lpFileName)
		//{
		//	//std::wstring fileName = lpFileName;
		//	wchar_t* wcFileName = (wchar_t*)lpFileName;
		//	std::wstring fileName(wcFileName, wcslen(wcFileName));
		//	if (!wcsncmp(wcFileName, L"..", 2))
		//	{
		//		std::wstring wsSwapFolder = L"MKSwap";
		//		std::wstring newFileName = L"..\\" + wsSwapFolder + fileName.substr(2, fileName.length() - 2);
		//		if (std::filesystem::exists(newFileName.c_str()))
		//		{
		//			wprintf(L"Loading %s from %s\n", wcFileName, wsSwapFolder.c_str());
		//			MVSGame::vSwappedFiles.push_back(wcFileName);
		//			return CreateFileW(newFileName.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		//		}
		//	}

		//}

		return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	void __cdecl DummyPrint(uint64_t* a) {
		printf("DUMMY PRINT! %p\n", a);
	}

	void __cdecl TestCallback(uint64_t* a) {
		printf("Test Callback! %p\n", a);
	}

	MVSGame::UFighterGameInstance* CopyFighterInstance(MVSGame::UFighterGameInstance* t, uint64_t* a2)
	{
		MVSGame::Instances::FighterGame = t;
		return MVSGame::UFigherGameInstanceConst(t, a2);
	}

	void SaveModWarningState()
	{
		FirstRunMgr->bPaidModWarned = true;
		FirstRunMgr->Save();
	}

	void OfflineModeWarning()
	{
		ShowNotification("Update Required", "Offline mode activated", 10.f);
	}

	bool MVSIOfflineModeChecker(int32_t* _TSS0_395)
	{
		static bool isUpdateWarned = !SettingsMgr->bDebug; // Only for debug now
		static bool isPaidWarnShown = FirstRunMgr->bPaidModWarned;
		static bool isVersionShown = false;
		MVSGame::Init_thread_header(_TSS0_395);
		if (_TSS0_395[0] == -1) // This segment will only be called once
		{
			// Check for updates

				
			//MVSGame::FDateTime(MVSGame::kSunsetDate, 2025, 6, 1, 0, 0, 0, 0); // No longer needed
			MVSI::IsMVSIUpdateRequired = false;

			MVSGame::Init_thread_footer(_TSS0_395);
		}

		if (!isPaidWarnShown && MVSGame::Instances::FighterGame)
		{

			MVSGame::UMvsDialog* dialog = ShowDialog((const char*)"MVSI is a FREE mod!", (const char*)"If you have paid for this, please ask for a refund!", (const char*)"I Agree", (const char*)"I Disagree");
			if (dialog)
			{
				uint64_t resultNo = dialog->AssignCallbackToButton(&dialog->OnButtonTwoClicked, MVSGame::QuitGame);
				uint64_t resultYes = dialog->AssignCallbackToButton(&dialog->OnButtonOneClicked, &SaveModWarningState);

				isPaidWarnShown = true;
			}
		}

		if (!isVersionShown && MVSGame::Instances::FighterGame)
		{
			if (ShowNotification((const char*)"MVSI Loaded", MVSI_Version, 10.f))
				isVersionShown = true;
		}

		if (!isUpdateWarned && MVSGame::Instances::FighterGame)
		{
			MVSGame::UMvsDialog* dialog = ShowDialog((const char*)"New Update Available!", (const char*)"Would you like to download the update now?", (const char*)"Yes", (const char*)"Play Offline");
			if (dialog)
			{
				uint64_t resultYes = dialog->AssignCallbackToButton(&dialog->OnButtonOneClicked, MVSGame::QuitGame); // Change to the update one
				uint64_t resultNo = dialog->AssignCallbackToButton(&dialog->OnButtonTwoClicked, &OfflineModeWarning); // Create function to disable internet or simply refuse connections from the server.
				isUpdateWarned = true;
			}
		}
		
		return MVSI::IsMVSIUpdateRequired; // Other checks can also be placed here
	}

	int64_t* OverrideProdEndpoint(int64_t* FStringPtr, const wchar_t* EndpointUrl)
	{
		CPPython::string ServerUrl = SettingsMgr->szProdServerUrl;

		if (HookMetadata::sActiveMods.bProdEndpointSwap && !ServerUrl.empty())
		{
			ServerUrl = ServerUrl.strip("/") + "/";

			int StringLength = (ServerUrl.size() + 1) * 2;
			std::wstring wServerUrl = std::wstring(ServerUrl.begin(), ServerUrl.end());

			SetColorCyan();
			wprintf(L"Rerouting traffic from Prod Server \"%s\" to \"%s\"!\n", EndpointUrl, wServerUrl.c_str());
			ResetColors();

			const wchar_t* end = wServerUrl.c_str();

			MVSGame::SetFStringValue(FStringPtr, end);

		}
		else
		{
			MVSGame::SetFStringValue(FStringPtr, EndpointUrl);
		}

		return FStringPtr;

	}

	const char** OverrideGameEndpoint(int64_t* TargetStringDest, const char * EndpointAddress)
	{
		CPPython::string ServerUrl = SettingsMgr->szServerUrl;

		if (HookMetadata::sActiveMods.bGameEndpointSwap && !ServerUrl.empty())
		{
			ServerUrl = ServerUrl.strip("/");
			
			int StringLength = (ServerUrl.size() + 1) * 2;
			std::wstring wServerUrl = std::wstring(ServerUrl.begin(), ServerUrl.end());

			SetColorCyan();
			wprintf(L"Rerouting traffic from MVS server \"%llx\" to \"%s\"!\n", (int64_t)EndpointAddress, wServerUrl.c_str());
			ResetColors();

			const char* end = ServerUrl.c_str();

			MVSGame::GetEndpointKeyValue(TargetStringDest, end);
			return (const char**)TargetStringDest;
		}

		return MVSGame::GetEndpointKeyValue(TargetStringDest, EndpointAddress);

	}
};

std::map<int, const char*> CURL_MAP
{
	{46,	"CURLOPT_UPLOAD"},
	{47,	"CURLOPT_POST"},
	{10001,	"CURLOPT_WRITEDATA"},
	{10029,	"CURLOPT_HEADERDATA"},
	{10002,	"CURLOPT_URL"},
	{10004,	"CURLOPT_PROXY"},
	{10173,	"CURLOPT_USERNAME"},
	{10005,	"CURLOPT_USERPWD"},
	{10023,	"CURLOPT_HTTPHEADER"},
	{60,	"CURLOPT_POSTFIELDSIZE"},
	{20012,	"CURLOPT_READFUNCTION"},
	{10009,	"CURLOPT_READDATA"},
	{10010,	"CURLOPT_ERRORBUFFER"},
	{8,		"CURLPROTO_FTPS"},
	{10015,	"CURLOPT_POSTFIELDS"},
	{20011,	"CURLOPT_WRITEFUNCTION"},
	{10018,	"CURLOPT_USERAGENT"},
	{80,	"CURLOPT_HTTPGET"},
	{81,	"CURLOPT_SSL_VERIFYHOST"},
	{14,	"CURLOPT_INFILESIZE"},
	{64,	"CURLOPT_SSL_VERIFYPEER"},
	{99,	"CURLOPT_NOSIGNAL"},
	{41,	"CURLOPT_VERBOSE"},
	{42,	"CURLOPT_HEADER"},
	{43,	"CURLOPT_NOPROGRESS"},
	{10086,	"CURLOPT_SSLCERTTYPE"},
	{20079, "CURLOPT_HEADERFUNCTION"},
	{20108, "CURLOPT_SSL_CTX_FUNCTION"},
	{10065, "CURLOPT_CAINFO"},
	{10097, "CURLOPT_CAPATH"},

};

bool bFirstPost = true;

enum class ArgTypes {
	ARGTYPE_NONE = 0, // UNK
	ARGTYPE_STRING, // String Pointer
	ARGTYPE_AGBINARY, // Data
	ARGTYPE_FUNCTION, // Callback
	ARGTYPE_CANCEL, // Return 0
	ARGTYPE_SETZERO, // Arg3 becomes 0
	ARGTYPE_INT, // Integer
	ARGTYPE_STRUCT, // Struct Pointer
};

ArgTypes GetArgType(const char* arg_type)
{
	if (arg_type == "CURLOPT_URL")
		return ArgTypes::ARGTYPE_STRING;
	if (arg_type == "CURLOPT_USERAGENT")
		return ArgTypes::ARGTYPE_STRING;
	if (arg_type == "CURLOPT_WRITEDATA")
		return ArgTypes::ARGTYPE_STRUCT;
	if (arg_type == "CURLOPT_USERNAME")
		return ArgTypes::ARGTYPE_STRUCT;
	if (arg_type == "CURLOPT_USERPWD")
		return ArgTypes::ARGTYPE_STRUCT;
	if (arg_type == "CURLOPT_HEADERDATA")
		return ArgTypes::ARGTYPE_AGBINARY;
	if (arg_type == "CURLOPT_READDATA")
		return ArgTypes::ARGTYPE_AGBINARY;
	if (arg_type == "CURLOPT_WRITEFUNCTION")
		return ArgTypes::ARGTYPE_FUNCTION;
	if (arg_type == "CURLOPT_READFUNCTION")
		return ArgTypes::ARGTYPE_FUNCTION;
	if (arg_type == "CURLOPT_SSL_VERIFYPEER")
		return ArgTypes::ARGTYPE_SETZERO;
	if (arg_type == "CURLOPT_SSL_VERIFYHOST")
		return ArgTypes::ARGTYPE_SETZERO;
	if (arg_type == "CURLOPT_INFILESIZE")
		return ArgTypes::ARGTYPE_INT;
	if (arg_type == "CURLOPT_POSTFIELDSIZE")
		return ArgTypes::ARGTYPE_INT;
	if (arg_type == "CURLOPT_CAPATH")
		return ArgTypes::ARGTYPE_STRING;
	if (arg_type == "CURLOPT_CAINFO")
		return ArgTypes::ARGTYPE_STRING;
	if (arg_type == "CURLOPT_HEADERFUNCTION")
		return ArgTypes::ARGTYPE_FUNCTION;
	if (arg_type == "CURLOPT_SSL_CTX_FUNCTION")
		return ArgTypes::ARGTYPE_FUNCTION;
	if (arg_type == "CURLOPT_SSLCERTTYPE")
		return ArgTypes::ARGTYPE_STRING;
	return ArgTypes::ARGTYPE_NONE;
}

// Hooks
namespace MVSI::Hooks {
	using namespace Memory::VP;
	using namespace hook;

	bool DisableSignatureCheck(Trampoline* GameTramp)
	{
		printf("\n==DisableSignatureCheck==\n");
		if (SettingsMgr->pSigCheck.empty())
		{
			printfRed("pSigCheck Not Specified. Please Add Pattern to ini file!\n");
			return false;
		}

		PatternFinder lpSigCheckPattern = SettingsMgr->pSigCheck;
		if (!lpSigCheckPattern)
		{
			printfError("Couldn't find SigCheck Pattern\n");
			return false;
		}

		lpSigCheckPattern += 0x30;
		if (SettingsMgr->iLogLevel)
			printf("SigCheck Pattern found at: %p\n", (uint64_t*)lpSigCheckPattern);

		// Old method
		//uint64_t CalledFuncAddr = GetDestinationFromOpCode(hook_address+7, 1, 5, 4);
		//Patch(GetGameAddr(CalledFuncAddr), (uint8_t)0xC3); // ret
		//Patch(GetGameAddr(CalledFuncAddr) + 1, (uint32_t)0x90909090); // Nop

		MakeProxyFromOpCode(
			GameTramp,
			lpSigCheckPattern + 7,
			(uint8_t)4,
			&DummyVoidFunc,
			PATCH_JUMP
		);


		printfSuccess("SigCheck Patched");

		return true;
	}

	bool PatchSunsetSetterIntoMVSIChecker(Trampoline* GameTramp)
	{
		printf("\n==Override Sunset Function==\n");
		std::string pattern = SettingsMgr->pSunsetDate;
		if (pattern.empty())
		{
			printfError("pSunsetDate Not Specified. Please Add Pattern to ini file!");
			return false;
		}

		PatternFinder lpPattern = pattern;
		if (!lpPattern)
		{
			printfError("Couldn't find SunsetDate Pattern");
			return false;
		}

		if (SettingsMgr->iLogLevel)
			printf("SunsetDate Pattern Found at: %p\n", (uint64_t*)lpPattern);

		MakeProxyFromOpCode(GameTramp, lpPattern + 7, (uint8_t)4, MVSI::Proxies::MVSIOfflineModeChecker, &MVSGame::Init_thread_header, PATCH_CALL);
		GetProcFromOpCode(lpPattern + 0x47, 4, &MVSGame::Init_thread_footer);
		GetProcFromOpCode(lpPattern + 0x3B, 4, &MVSGame::FDateTime);
		MVSGame::kSunsetDate = (uint64_t*) GetDestinationFromOpCode(lpPattern + 0x19, 3, 7, 4);
		
		uint64_t final_bool_offset = lpPattern - 0x2F;
		Memory::VP::InjectHook(final_bool_offset, GameTramp->Jump(MVSI::Proxies::MVSIOfflineModeChecker), PATCH_CALL);
		Patch<uint16_t>(final_bool_offset + 0x5, 0x10EB);

		if (SettingsMgr->iLogLevel)
		{
			printf("Init_thread_header Function Found at: %p\n", MVSGame::Init_thread_header);
			printf("Init_thread_footer Function Found at: %p\n", MVSGame::Init_thread_footer);
			printf("FDateTime Function Found at: %p\n", MVSGame::FDateTime);
			printf("SunsetDate Object Found at: %p\n", MVSGame::kSunsetDate);
		}
				
		// Jump to the cleanup function
		Patch<uint16_t>((uint64_t)lpPattern + 0xC, 0xDAEB);
		
		// Nop the addresses
		uint64_t nop_loc = lpPattern + 0xE;
		for (int i = 0; i < int(0x40 / 9) - 1; i++) // 7 times
		{
			Patch<uint64_t>(nop_loc, 0x841F0F66);
			//Patch<uint8_t>(nop_loc + 8, 0); // Unneeded
			nop_loc += 9;
		}
		for (int i = 0; i < int(12 / 4); i++)
		{
			Patch<uint32_t>(nop_loc, 0x401F0F);
			nop_loc += 4;
		}

		nop_loc = final_bool_offset + 0x7;
		for (int i = 0; i < int(16 / 4); i++)
		{
			Patch<uint32_t>(nop_loc, 0x401F0F);
			nop_loc += 4;
		}

		printfSuccess("Sunset Function Proxied");
		
	}

	bool OverrideProdEndpointsData(Trampoline* GameTramp)
	{
		printf("\n==OverrideGameEndpointsData==\n");
		std::string pattern = SettingsMgr->pProdEndpointLoader;
		if (pattern.empty())
		{
			printfError("pProdEndpointLoader Not Specified. Please Add Pattern to ini file!");
			return false;
		}

		if (SettingsMgr->szProdServerUrl.empty())
		{
			printfWarning("Prod Server Url is empty or not specified. Skipping!");
			return false;
		}

		PatternFinder lpPattern = pattern;
		if (!lpPattern)
		{
			printfError("Couldn't find ProdEndpointLoader Pattern");
			return false;
		}

		lpPattern += 0x0A;
		if (SettingsMgr->iLogLevel)
			printf("ProdEndpointLoader Pattern Found at: %p\n", (uint64_t*)lpPattern);

		MakeProxyFromOpCode(GameTramp, lpPattern, 4, MVSI::Proxies::OverrideProdEndpoint, &MVSGame::SetFStringValue, PATCH_CALL);

		printfSuccess("ProdEndpointLoader Proxied");
		return true;
	}

	bool OverrideGameEndpointsData(Trampoline* GameTramp)
	{
		printf("\n==OverrideGameEndpointsData==\n");
		std::string pattern = SettingsMgr->pEndpointLoader;
		if (pattern.empty())
		{
			printfError("pEndpointLoader Not Specified. Please Add Pattern to ini file!");
			return false;
		}

		if (SettingsMgr->szServerUrl.empty())
		{
			printfWarning("Server Url is empty or not specified. Skipping!");
			return false;
		}

		PatternFinder lpPattern = pattern;
		if (!lpPattern)
		{
			printfError("Couldn't find EndpointLoader Pattern");
			return false;
		}

		lpPattern += 0x0A;
		if (SettingsMgr->iLogLevel)
			printf("EndpointLoader Pattern Found at: %p\n", (uint64_t*)lpPattern);

		MakeProxyFromOpCode(GameTramp, lpPattern, 4, MVSI::Proxies::OverrideGameEndpoint, &MVSGame::GetEndpointKeyValue, PATCH_CALL);

		printfSuccess("EndpointLoader Proxied");
		return true;
	}

	bool DialogHooks(Trampoline* GameTramp)
	{
		printf("\n==Dialog Funcs==\n");
		if (!HookMetadata::sActiveMods.bUEFuncs)
		{
			printfError("UE Funcs were not enabled therefore Dialog cannot be used!");
			return false;
		}

		PatternFinder lpPattern;

		if (SettingsMgr->pDialog.empty())
		{
			printfError("pDialog Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		lpPattern = SettingsMgr->pDialog;
		if (!lpPattern)
		{
			printfError("Couldn't find Dialog Pattern");
			return false;
		}

		MVSGame::UMvsFrontendManager::AddDialogPtr = (MVSGame::UMvsFrontendManager::AddDialogType)(uint64_t*)(lpPattern + 30); // 0x142766C50
		GetProcFromOpCode(lpPattern + 38, 4, &MVSGame::GetFrontendManager); // 0x142722980

		if (SettingsMgr->iLogLevel)
		{
			printf("Dialog Pattern Found at: %p\n", (uint64_t*)lpPattern);
			printf("AddDialog at %p\n", MVSGame::UMvsFrontendManager::AddDialogPtr);
			printf("FrontendManager at %p\n", MVSGame::GetFrontendManager);
		}

		if (!SettingsMgr->pDialogParams.empty())
		{
			lpPattern = SettingsMgr->pDialogParams;
			if (lpPattern)
			{
				MVSGame::FMvsDialogParametersConst = (MVSGame::DialogParamsType*)(uint64_t*)lpPattern; // 0x14276C2F0
				//MVSGame::GLeave_Lobby_Prompt_Text = (MVSGame::FText*)GetGameAddr(0x1480ADDB0);

				if (SettingsMgr->iLogLevel)
				{
					printf("DialogParams Pattern Found at: %p\n", (uint64_t*)lpPattern);
				}

			}
		}

		if (SettingsMgr->pDialogCallback.empty())
		{
			printfError("pDialogCallback Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		lpPattern = SettingsMgr->pDialogCallback;
		if (!lpPattern)
		{
			printfError("Couldn't find DialogCallback Pattern");
			return false;
		}

		GetProcFromOpCode(lpPattern, 4, &MVSGame::SingleParamDialogCallbackSetter); // 0x1428AD240

		if (SettingsMgr->iLogLevel)
		{
			printf("DialogCallback Pattern Found at: %p\n", (uint64_t*)lpPattern);
			printf("DialogCallback at %p\n", MVSGame::SingleParamDialogCallbackSetter);
		}


		if (SettingsMgr->pQuitGameCallback.empty())
		{
			printfError("pQuitGameCallback Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		else
		{
			lpPattern = SettingsMgr->pQuitGameCallback;
			if (!lpPattern)
			{
				printfWarning("Couldn't find pQuitGameCallback Pattern. Going with alternative Shutdown mechanism!");
			}
			else
			{
				MVSGame::QuitGame = (MVSGame::QuitGameParamsType*)(uint64_t*)lpPattern; // 0x1428AFCC0

				if (SettingsMgr->iLogLevel)
				{
					printf("QuitGameCallback Pattern Found at: %p\n", (uint64_t*)lpPattern);
					printf("QuitGameCallback at %p\n", MVSGame::QuitGame);
				}
			}
		}

		printfSuccess("Dialog Usable!");
		return true;
	}

	bool NotificationHooks(Trampoline* GameTramp)
	{
		MVSGame::UMvsNotificationManager::GetNotifManagerPtr = (MVSGame::UMvsNotificationManager::GetNotifManagerType)GetGameAddr(0x1428B1D60);

		printf("\n==Notification Funcs==\n");
		if (!HookMetadata::sActiveMods.bUEFuncs)
		{
			printfError("UE Funcs were not enabled therefore Notifications cannot be used!");
			return false;
		}
		
		PatternFinder lpPattern;

		if (SettingsMgr->pNotifs.empty())
		{
			printfError("pNotifs Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		lpPattern = SettingsMgr->pNotifs; // This pattern finds 3 functions, but all of them are the exact same so any works
		if (!lpPattern)
		{
			printfError("Couldn't find Notifications Pattern");
			return false;
		}

		//MVSGame::UMvsNotificationManager::RequestShowNotificationPtr = (MVSGame::UMvsNotificationManager::RequestShowNotificationType)(uint64_t*)(lpPattern + 32); // 0x142995E40 // 32
		GetProcFromOpCode(lpPattern + 32, 4, &MVSGame::UMvsNotificationManager::RequestShowNotificationPtr);
		//MVSGame::UMvsNotificationManager::GetNotifManagerPtr = (MVSGame::UMvsNotificationManager::GetNotifManagerType)GetGameAddr(0x1428B1D60); // 0x1428B1D60 // 19
		GetProcFromOpCode(lpPattern + 19, 4, &MVSGame::UMvsNotificationManager::GetNotifManagerPtr); // 0x1428B1D60

		if (SettingsMgr->iLogLevel)
		{
			printf("Notifications Pattern Found at: %p\n", (uint64_t*)lpPattern);
			printf("ShowNotification at %p\n", MVSGame::UMvsNotificationManager::RequestShowNotificationPtr);
			printf("NotificationManager at %p\n", MVSGame::UMvsNotificationManager::GetNotifManagerPtr);
		}


		printfSuccess("Notifications Usable!");
		return true;
	}

	bool HookUEFuncs(Trampoline* GameTramp)
	{
		printf("\n==UE Funcs==\n");
		if (SettingsMgr->pFText.empty())
		{
			printfError("pFText Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		PatternFinder lpPattern = SettingsMgr->pFText;
		if (!lpPattern)
		{
			printfError("Couldn't find FText Pattern");
			return false;
		}

		PatternFinder first_address = lpPattern - (uint64_t)26;

		// FText
		GetProcFromOpCode(first_address + 14, 4, &FText::FromString); // 0x142AFE4C0
		GetProcFromOpCode(lpPattern + 3, 4, &FText::FromName); // 0x142AFE350
		GetProcFromOpCode((uint64_t)FText::FromString + 32, 4, &FText::GetEmpty); // 0x142AFE6D0

		if (SettingsMgr->iLogLevel)
		{
			printf("FText Pattern Found at: %p\n", (uint64_t*)lpPattern);
			printf("FText::FromString at: %p\n", FText::FromString);
			printf("FText::FromName at: %p\n", FText::FromName);
			printf("FText::GetEmpty at: %p\n", FText::GetEmpty);
		}

		// FName
		GetProcFromOpCode(first_address, 4, &FName::ToStringPtr);

		if (SettingsMgr->pCFName.empty())
		{
			printfError("pCFName Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		lpPattern = SettingsMgr->pCFName;
		if (!lpPattern)
		{
			printfError("Couldn't find CFName Pattern");
			return false;
		}

		if (SettingsMgr->iLogLevel)
		{
			printf("CFName Pattern Found at: %p\n", (uint64_t*)lpPattern);
		}

		GetProcFromOpCode(lpPattern + 79, 4, &FName::FNameCharConstructor); // 0x142BB5FC0

		if (SettingsMgr->pWCFname.empty())
		{
			printfError("pWCFname Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		lpPattern = SettingsMgr->pWCFname;
		if (!lpPattern)
		{
			printfError("Couldn't find WCFname Pattern");
			return false;
		}
		if (SettingsMgr->iLogLevel)
		{
			printf("WCFname Pattern Found at: %p\n", (uint64_t*)lpPattern);
		}
		FName::FNameWCharConstructor = (FName::FNameConstructorWCharType)GetGameAddr(lpPattern - 32); // 0x142BB6120

		if (SettingsMgr->iLogLevel)
		{
			printf("FName::ToString at: %p\n", FName::ToStringPtr);
			printf("FName::FName(char*) at: %p\n", FName::FNameCharConstructor);
			printf("FName::FName(wchar_t*) at: %p\n", FName::FNameWCharConstructor);
		}

		if (SettingsMgr->pFighterInstance.empty())
		{
			printfError("pFighterInstance Not Specified. Please Add Pattern to ini file!");
			return false;
		}
		lpPattern = SettingsMgr->pFighterInstance;
		if (!lpPattern)
		{
			printfError("Couldn't find FighterInstance Pattern");
			return false;
		}

		uint64_t address = GetDestinationFromOpCode(lpPattern, 3, 7, 4); // Couldn't get inside cuz too small, so get outside's lea, go inside, offset.
		MakeProxyFromOpCode(GameTramp, address + 14, 4, MVSI::Proxies::CopyFighterInstance, &MVSGame::UFigherGameInstanceConst, PATCH_JUMP); // 0x1422943BE

		if (SettingsMgr->iLogLevel)
		{
			printf("FighterInstance Pattern Found at: %p\n", (uint64_t*)lpPattern);
			printf("FighterInstance Proxied from %p to %p\n", MVSGame::UFigherGameInstanceConst, MVSI::Proxies::CopyFighterInstance);
		}

		printfSuccess("All Required UE Funcs Found!");

		return true;
	}

};

namespace MVSI::Mods {

}

namespace HookMetadata {
	HHOOK KeyboardProcHook = nullptr;
	HMODULE CurrentDllModule = NULL;
	HANDLE Console = NULL;

};