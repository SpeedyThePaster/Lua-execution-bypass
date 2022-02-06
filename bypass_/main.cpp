#include "common.h"
#include "minhook.h"

typedef HANDLE(APIENTRY* LPFN_CREATEFILEW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

LPFN_CREATEFILEW g_CreateFileW;

bool initialized{ false };

HANDLE WINAPI CreateFileHook(LPCWSTR fileName, DWORD desiredAccess, DWORD shareMode, LPSECURITY_ATTRIBUTES pSecurityAttributes, DWORD creationDisposition, DWORD flagsAndAttributes, HANDLE hTemplateFile)
{
	if (!initialized)
	{
		if (wcsstr(fileName, (L"graph.lua")))
		{
			std::wstring targetPath = std::wstring(L"C:").
				append(L"\\").
				append(L"test").
				append(L"\\").
				append(L"test.lua");

			MessageBoxA(NULL, "Executed", "Info", NULL);

			initialized = true;

			return g_CreateFileW(targetPath.c_str(), desiredAccess, shareMode, pSecurityAttributes, creationDisposition, flagsAndAttributes, hTemplateFile);
		}
	}

	return g_CreateFileW(fileName, desiredAccess, shareMode, pSecurityAttributes, creationDisposition, flagsAndAttributes, hTemplateFile);
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (!FindWindow(L"grcWindow", nullptr))
			return true;

		MessageBoxA(NULL, "Injected", "Info", NULL);

		MH_Initialize();
		MH_CreateHook(CreateFileW, CreateFileHook, &reinterpret_cast<PVOID&>(g_CreateFileW));
		MH_EnableHook(CreateFileW);
	}
	return true;
	}

	return true;
}
