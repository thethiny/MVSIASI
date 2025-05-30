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



	bool MVSIOfflineModeChecker(int32_t* _TSS0_395)
	{
		MVSGame::Init_thread_header(_TSS0_395);
		if (_TSS0_395[0] == -1) // This portion will only be called once
		{
			// Check for updates
			//MVSGame::FDateTime(MVSGame::kSunsetDate, 2025, 6, 1, 0, 0, 0, 0); // No longer needed
			MVSI::IsMVSIUpdateRequired = false;

			MVSGame::Init_thread_footer(_TSS0_395);
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

			wprintf(L"Rerouting traffic from Prod Server \"%s\" to \"%s\"!\n", EndpointUrl, wServerUrl.c_str());

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

			wprintf(L"Rerouting traffic from MVS server \"%llx\" to \"%s\"!\n", (int64_t)EndpointAddress, wServerUrl.c_str());

			const char* end = ServerUrl.c_str();

			MVSGame::GetEndpointKeyValue(TargetStringDest, end);
			return (const char**)TargetStringDest;

			//obj.ValuePointer = new wchar_t[StringLength];
			//memcpy(obj.ValuePointer, wServerUrl.c_str(), StringLength);

			//obj.ValueLength = ServerUrl.size() + 1; // Characters count in str and not wstr
			//obj.ValueLength8BAligned = (obj.ValueLength + 7) & (~7);

			//MVSGame::GetEndpointKeyValue(obj, EndpointAddress);
			//return &obj.ValuePointer;
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

		uint64_t* lpSigCheckPattern = FindPattern(GetModuleHandleA(NULL), SettingsMgr->pSigCheck);
		if (!lpSigCheckPattern)
		{
			printfError("Couldn't find SigCheck Pattern\n");
			return false;
		}

		uint64_t hook_address = (uint64_t)lpSigCheckPattern + 0x30;
		if (SettingsMgr->iLogLevel)
			printf("SigCheck Pattern found at: %p\n", lpSigCheckPattern);

		//uint64_t CalledFuncAddr = GetDestinationFromOpCode(hook_address+7, 1, 5, 4);

		//if (SettingsMgr->iLogLevel)
		//	printf("SigCheck Function Callee found at: %p\n", (uint64_t*)CalledFuncAddr);
		//

		//Patch(GetGameAddr(CalledFuncAddr), (uint8_t)0xC3); // ret
		//Patch(GetGameAddr(CalledFuncAddr) + 1, (uint32_t)0x90909090); // Nop
		MakeProxyFromOpCode(
			GameTramp,
			hook_address + 7,
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

		uint64_t* lpPattern = FindPattern(GetModuleHandleA(NULL), pattern);
		if (!lpPattern)
		{
			printfError("Couldn't find SunsetDate Pattern");
			return false;
		}

		uint64_t call_address = ((uint64_t)lpPattern);
		if (SettingsMgr->iLogLevel)
			printf("SunsetDate Pattern Found at: %p\n", lpPattern);

		MakeProxyFromOpCode(GameTramp, call_address + 7, (uint8_t)4, MVSI::Proxies::MVSIOfflineModeChecker, &MVSGame::Init_thread_header, PATCH_CALL);
		GetProcFromOpCode(call_address + 0x47, 4, &MVSGame::Init_thread_footer);
		GetProcFromOpCode(call_address + 0x3B, 4, &MVSGame::FDateTime);
		MVSGame::kSunsetDate = (uint64_t*) GetDestinationFromOpCode(call_address + 0x19, 3, 7, 4);
		
		uint64_t final_bool_offset = call_address - 0x2F;
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
		Patch<uint16_t>(call_address + 0xC, 0xDAEB);
		
		// Nop the addresses
		uint64_t nop_loc = call_address + 0xE;
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

		uint64_t* lpPattern = FindPattern(GetModuleHandleA(NULL), pattern);
		if (!lpPattern)
		{
			printfError("Couldn't find ProdEndpointLoader Pattern");
			return false;
		}

		uint64_t call_address = ((uint64_t)lpPattern) + 0x0A;
		if (SettingsMgr->iLogLevel)
			printf("ProdEndpointLoader Pattern Found at: %p\n", lpPattern);

		MakeProxyFromOpCode(GameTramp, call_address, (uint8_t)4, MVSI::Proxies::OverrideProdEndpoint, &MVSGame::SetFStringValue, PATCH_CALL);
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

		uint64_t* lpPattern = FindPattern(GetModuleHandleA(NULL), pattern);
		if (!lpPattern)
		{
			printfError("Couldn't find EndpointLoader Pattern");
			return false;
		}

		uint64_t call_address = ((uint64_t)lpPattern) + 0x0A;
		if (SettingsMgr->iLogLevel)
			printf("EndpointLoader Pattern Found at: %p\n", lpPattern);

		MakeProxyFromOpCode(GameTramp, call_address, (uint8_t)4, MVSI::Proxies::OverrideGameEndpoint, &MVSGame::GetEndpointKeyValue, PATCH_CALL);

		printfSuccess("EndpointLoader Proxied");
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