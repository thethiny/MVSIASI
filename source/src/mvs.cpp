#include "mvsiutils.h"
#include "mvs.h"


// Game Functions
MVSGame::GetEndpointKeyValueType*			MVSGame::GetEndpointKeyValue			= nullptr;
MVSGame::SetFStringValueType*				MVSGame::SetFStringValue				= nullptr;
MVSGame::FDateTimeType*						MVSGame::FDateTime						= nullptr;
MVSGame::InitThreadHeaderType*				MVSGame::Init_thread_header				= nullptr;
MVSGame::InitThreadFooterType*				MVSGame::Init_thread_footer				= nullptr;
// Game vars
uint64_t*									MVSGame::kSunsetDate					= nullptr;

namespace MVSGame::FNameFunc {
	inline uint16_t GetSize(FName& F)
	{
		return F.NameSize >> 6;
	};
	inline char* GetName(FName& F)
	{
		return F.Name;
	}
	void Print(FName& F)
	{
		uint16_t s = GetSize(F);
		fwrite(F.Name, 1, s, stdout);
		printf("\n");
	}
	char* ToStr(FName& F)
	{
		uint16_t s = GetSize(F);
		char* name = new char[s + 1];
		strncpy(name, F.Name, s);
		name[s] = '\0'; // Null terminate
		return name;
	}
	FName* FromStr(const char* string)
	{
		uint16_t length = (uint16_t)strlen(string);
		FName* f = (FName*)(new uint8_t[length + 2]);
		f->NameSize = (length << 6) + 0xC; // IDK where C comes from
		strncpy(f->Name, string, length);

		return f;
	}
}