#include "common.h"
#include "minhook/MinHook.h"

using fnSwapBuffers = BOOL(__stdcall*)(HDC a1);
fnSwapBuffers oSwapBuffers;
fnSwapBuffers glSwapBuffers;

WNDPROC wndproc = nullptr;

LRESULT CALLBACK WindowProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(handle == GetFocus())
	{
		// in game
		if (msg == WM_KEYDOWN && wparam == VK_INSERT)
			bindings.Init();
	}

	return CallWindowProc(wndproc, handle, msg, wparam, lparam);
}

bool first = true;

BOOL __stdcall hkSwapBuffers(HDC a1)
{
	if(first)
	{
		printf("Init\n");
		if(!jvm.Init())
		{
			MessageBoxA(NULL, "JVM: Failed to init!", NULL, NULL);
			MH_DisableHook(glSwapBuffers);
			return oSwapBuffers(a1);
		}
		srg.LoadFile("1.14.2");
		if(!bindings.Init())
		{
			MessageBoxA(NULL, "Bindings: Failed to init!", NULL, NULL);
			MH_DisableHook(glSwapBuffers);
			return oSwapBuffers(a1);
		}
		first = false;
	}

	printf("Object: 0x%p\n", Helpers::GetMinecraftObject());
	Bindings::HookCall("Update");
	
	return oSwapBuffers(a1);
}


bool Hooks::Init()
{
	HWND window = FindWindow("GLFW30", NULL);
	if(!window)
	{
		printf("Failed to find Minecraft!\n");
		return false;
	}

	// Perform hook TODO: Add error handling
	MH_Initialize();
	auto swapbuffers_address = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
	MH_CreateHook(swapbuffers_address, hkSwapBuffers, (void**)(&oSwapBuffers));
	MH_EnableHook(swapbuffers_address);

	if (!wndproc)
		wndproc = (WNDPROC)(SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WindowProc));
	
	return true;
}

Hooks hooks;