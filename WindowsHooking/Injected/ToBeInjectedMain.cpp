#undef UNICODE

#include <windows.h>
#include <detours.h>
#include <stdio.h>
#include <string>

//#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "detours.lib" )
//#pragma comment( lib, "detoured.lib" )

#define FILE_PATH "C:\\Users\\Elro\\Desktop\\LoL-RE\\Log.txt"


std::wstring stringReplace(std::wstring str, const std::wstring& from, const std::wstring& to);
std::string stringReplace(std::string str, const std::string& from, const std::string& to);

auto Real_CreateProcessW = CreateProcessW;
auto Real_CreateProcessA = CreateProcessA;

void logToFileW(std::wstring line);
void logToFileA(std::string line);

BOOL WINAPI My_CreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
BOOL WINAPI My_CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);




//auto Real_WriteFile = WriteFile;
//BOOL __stdcall My_WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
char g_dllPath[MAX_PATH];
char g_exePathA[MAX_PATH];
wchar_t g_exePathW[MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE, DWORD dwReason, LPVOID) 
{
	LONG lError = 0;
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		//MessageBox(NULL, TEXT("DLL_PROCESS_ATTACH"), TEXT("Success"), MB_OK);
		GetModuleFileName((HINSTANCE)&__ImageBase, g_dllPath, _countof(g_dllPath));
		GetModuleFileNameA(NULL, g_exePathA, _countof(g_exePathA));
		GetModuleFileNameW(NULL, g_exePathW, _countof(g_exePathW));
		/*logToFileA(std::string("DllPath: ") + g_dllPath);
		logToFileA(std::string("ExePath: ") + g_exePath);*/
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID &)Real_CreateProcessW, My_CreateProcessW);
		DetourAttach(&(PVOID &)Real_CreateProcessA, My_CreateProcessA);
		//DetourAttach(&(PVOID &)Real_WriteFile, My_WriteFile);

		lError = DetourTransactionCommit();
		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, TEXT("Failed to detour"), TEXT("Elro"), MB_OK);
			return FALSE;
		}
		break;

	case DLL_PROCESS_DETACH:
		//MessageBox(NULL, TEXT("DLL_PROCESS_DETACH"), TEXT("Success"), MB_OK);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID &)Real_CreateProcessW, My_CreateProcessW);
		DetourDetach(&(PVOID &)Real_CreateProcessA, My_CreateProcessA);
		//DetourDetach(&(PVOID &)Real_WriteFile, My_WriteFile);

		lError = DetourTransactionCommit();
		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, TEXT("Failed to detour"), TEXT("Elro"), MB_OK);
			return FALSE;
		}
		break;
	}

	return TRUE;
}

HMODULE injectDLL(HANDLE hProcess)
{
	HMODULE hRet = NULL;
	DWORD bufferSize = 0;

	bufferSize = lstrlen(g_dllPath);
	bufferSize *= sizeof(TCHAR);

	LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	LPVOID GetLastErrorAddr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetLastError");
	LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, LLParam, g_dllPath, bufferSize, NULL);
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, LLParam, NULL, NULL);
	if (hThread != NULL)
	{
		do
		{
			GetExitCodeThread(hThread, (LPDWORD)&hRet);
			Sleep(10);
		} while ((int)hRet == 259);
		
		CloseHandle(hProcess);
	}
	return hRet;
}

std::string stringReplace(std::string str, const std::string& from, const std::string& to) 
{
	size_t start_pos = str.find(from);
	if (start_pos != std::string::npos)
		str.replace(start_pos, from.length(), to);
	return str;
}

std::wstring stringReplace(std::wstring str, const std::wstring& from, const std::wstring& to)
{
	size_t start_pos = str.find(from);
	if (start_pos != std::wstring::npos)
		str.replace(start_pos, from.length(), to);
	return str;
}

void logToFileA(std::string line)
{
	FILE* f = nullptr;
	fopen_s(&f, FILE_PATH, "a");
	fprintf(f, "%s\n", line.c_str());
	fclose(f);
}

void logToFileW(std::wstring line)
{
	FILE* f = nullptr;
	fopen_s(&f, FILE_PATH, "a");
	fwprintf(f, L"%s\n", line.c_str());
	fclose(f);
}

BOOL WINAPI My_CreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	std::wstring commandLine = lpCommandLine;
	stringReplace(commandLine, L"--no-proxy-server", L"--proxy-server"); // Get rid of the unwanted parameter

	size_t bufferSize = _snwprintf(NULL, 0, L"[%s]: CreateProcess - (%s, %s)", g_exePathW, lpApplicationName, commandLine.data());
	std::wstring buffer(bufferSize + 1, '\0');
	_snwprintf(&buffer[0], buffer.size(), L"[%s]: CreateProcess - (%s, %s)", g_exePathW, lpApplicationName, commandLine.data());
	logToFileW(buffer);
	/*if ((dwCreationFlags & CREATE_SUSPENDED) != CREATE_SUSPENDED)
		dwCreationFlags |= CREATE_SUSPENDED;*/

	BOOL bRet = Real_CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	if (bRet)
	{
		//HMODULE hModule = injectDLL(lpProcessInformation->hProcess);

		ResumeThread(lpProcessInformation->hThread);
	}

	return bRet;
}


BOOL WINAPI My_CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	//logToFileA("Were in?");

	std::string commandLine = lpCommandLine;
	stringReplace(commandLine, "--no-proxy-server", "--proxy-server"); // Get rid of the unwanted parameter

	size_t bufferSize = snprintf(NULL, 0, "[%s]: CreateProcess - (%s, %s)", g_exePathA, lpApplicationName, commandLine.data());
	std::string buffer(bufferSize + 1, '\0');
	snprintf(&buffer[0], buffer.size(), "[%s]: CreateProcess - (%s, %s)", g_exePathA, lpApplicationName, commandLine.data());
	logToFileA(buffer);
	//MessageBox(NULL, TEXT("My_CreateProcessA"), TEXT("Elro"), MB_OK);

	if ((dwCreationFlags & CREATE_SUSPENDED) != CREATE_SUSPENDED)
		dwCreationFlags |= CREATE_SUSPENDED;

	BOOL bRet = Real_CreateProcessA(lpApplicationName, &commandLine[0], lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	if (bRet)
	{
		HMODULE hModule = injectDLL(lpProcessInformation->hProcess);
		if (hModule == NULL)
			MessageBox(NULL, TEXT("hModule is NULL"), TEXT("Error"), MB_OK);


		ResumeThread(lpProcessInformation->hThread);
	}

	return bRet;
}

//BOOL __stdcall My_WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
//{
//	const char* newBuffer = "HA HA HA HA, Im not what you wanted to save!";
//	return Real_WriteFile(hFile, newBuffer, lstrlen(newBuffer), lpNumberOfBytesWritten, lpOverlapped);
//}
