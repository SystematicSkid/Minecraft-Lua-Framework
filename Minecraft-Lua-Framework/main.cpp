#include "common.h"

void Init()
{
	AllocConsole();
	FILE* out;
	freopen_s(&out, "conout$", "w", stdout);
	
	if (!hooks.Init())
		MessageBoxA(NULL, "Hooks: Failed to initialize!", NULL, NULL);
}

DWORD WINAPI DllMain( _In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved )
{
	if(_Reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(static_cast<HMODULE>(_DllHandle)); // Less Overhead
		Init();
		return 1;
	}
	return 0;
}