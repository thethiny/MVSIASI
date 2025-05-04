#include "mkutils.h"
#include "mk12.h"


// Game Functions
MK12::GetEndpointKeyValueType*			MK12::GetEndpointKeyValue			= nullptr;

namespace MK12::FNameFunc {
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