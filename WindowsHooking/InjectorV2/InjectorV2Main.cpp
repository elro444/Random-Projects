#undef UNICODE
#include <vector>
#include <string>
#include <windows.h>
#include <Tlhelp32.h>



namespace std
{
	#ifdef UNICODE
	typedef std::wstring tstring;
	#else
	typedef std::string tstring;
	#endif
}


// call GetModuleFileName to get the full path of the module before installing the hook
//static LPCTSTR lpszDllName = TEXT("C:\\Users\\Elro\\source\\repos\\Windows Hooking\\x64\\Release\\PlagueX64.dll");
HMODULE injectDLL(HANDLE hProcess, LPCWSTR lpcszDllPath);
BOOL WINAPI CreateProcessWHook(__in_opt LPCTSTR lpApplicationName, __inout_opt LPTSTR lpCommandLine, __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes, __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes, __in BOOL bInheritHandles, __in DWORD dwCreationFlags, __in_opt LPVOID lpEnvironment, __in_opt LPCTSTR lpCurrentDirectory, __in LPSTARTUPINFO lpStartupInfo, __out LPPROCESS_INFORMATION lpProcessInformation);
std::string GetLastErrorAsString();
 



int main(int, char *[]) 
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(STARTUPINFO);

	//TCHAR cmd[] = TEXT("C:\\Riot Games\\League of Legends\\LeagueClient.exe");
	TCHAR cmd[] = TEXT("C:\\Windows\\System32\\cmd.exe");
	//TCHAR cmd[] = TEXT("C:\\Users\\Elro\\Desktop\\ROP Gadget\\BufferOverflow.exe");
	BOOL result = CreateProcessWHook(cmd, NULL, NULL, NULL, FALSE, 0, NULL, TEXT("C:\\Riot Games\\League of Legends"), &si, &pi);
	printf("Result: %d\n", result);
	if (!result)
	{
		printf("Error: %s\n", GetLastErrorAsString().c_str());
	}
	//getchar();
	return 0;
}

HMODULE injectDLL(HANDLE hProcess, LPCWSTR lpcszDllPath)
{
	HMODULE hRet = NULL;
	DWORD bufferSize = 0;

	bufferSize = lstrlenW(lpcszDllPath);
	bufferSize *= sizeof(TCHAR);

	LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
	LPVOID GetLastErrorAddr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetLastError");
	LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, LLParam, lpcszDllPath, bufferSize, NULL);
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, LLParam, NULL, NULL);
	if (hThread != NULL)
	{
		do
		{
			GetExitCodeThread(hThread, (LPDWORD)&hRet);
			Sleep(10);
		} while ((int)hRet == 259);
		if (hRet == NULL)
		{
			DWORD dwErrorCode = 0;
			hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)GetLastErrorAddr, NULL, NULL, NULL);
			do
			{
				GetExitCodeThread(hThread, &dwErrorCode);
				Sleep(10);
			} while (dwErrorCode == 259);
			printf("Error: %d\n", dwErrorCode);
		}
		CloseHandle(hProcess);
	}
	return hRet;
}

// hook future process creation - install this hook on top of CreateProcessW
// I'd suggest using Microsoft Detours [http://research.microsoft.com/en-us/projects/detours/]
BOOL WINAPI CreateProcessWHook(__in_opt LPCTSTR lpApplicationName, __inout_opt LPTSTR lpCommandLine, __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes, __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes, __in BOOL bInheritHandles, __in DWORD dwCreationFlags, __in_opt LPVOID lpEnvironment, __in_opt LPCTSTR lpCurrentDirectory, __in LPSTARTUPINFO lpStartupInfo, __out LPPROCESS_INFORMATION lpProcessInformation)
{
	static LPCWSTR lpszDllName = L"C:\\Users\\Elro\\source\\repos\\Windows Hooking\\Release\\PlagueX86.dll";
	// create the process suspended
	if ((dwCreationFlags & CREATE_SUSPENDED) != CREATE_SUSPENDED)
		dwCreationFlags |= CREATE_SUSPENDED;
	// call original CreateProcessW
	BOOL bRet = CreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	if (bRet)
	{
		// inject DLL
		HMODULE hModule = injectDLL(lpProcessInformation->hProcess, lpszDllName);
		if (!hModule)
			printf("Injection error.\n");
		else
			printf("hModule: %d\n", hModule);

		// resume thread
		ResumeThread(lpProcessInformation->hThread);
	}

	return bRet;
}

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
