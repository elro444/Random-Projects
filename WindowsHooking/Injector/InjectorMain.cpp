#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <detours.h>
#include <string>

#pragma comment(lib, "detours.lib")

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

void EnableDebugPriv() {
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}

int main(int, char *[]) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(STARTUPINFO);

	char cmd[] = "C:\\Riot Games\\League of Legends\\LeagueClient.exe";
	BOOL result = DetourCreateProcessWithDll(cmd, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi, "d.dll", NULL);// , "C:\\Users\\Elro\\source\\repos\\Windows Hooking\\x64\\Release\\d.dll", NULL);
	printf("Result: %d\n", result);
	if (!result)
	{
		printf("Error: %s\n", GetLastErrorAsString().c_str());
	}
	/*PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	bool foundIt = false;
	while (!foundIt)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE) {
			while (Process32Next(snapshot, &entry) == TRUE) {
				if (_stricmp(entry.szExeFile, "LeagueClientUx.exe") == 0) {
					printf("FOUND IT!\n");
					foundIt = true;
					EnableDebugPriv();

					char dirPath[MAX_PATH];
					char fullPath[MAX_PATH];

					GetCurrentDirectory(MAX_PATH, dirPath);

					sprintf_s(fullPath, MAX_PATH, "\"%s\\d.dll\"", dirPath);
					printf("Full path: %s\n", fullPath);

					HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, entry.th32ProcessID);
					printf("hProcess: %d\n", hProcess);
					LPVOID libAddr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
					LPVOID llParam = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(fullPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

					

					SIZE_T bytesWritten = 0;
					WriteProcessMemory(hProcess, llParam, fullPath, strlen(fullPath), &bytesWritten);
					printf("BytesWritten: %d\n", bytesWritten);
					HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)libAddr, llParam, NULL, NULL);
					
					printf("hRemoteThread: %d\n", hRemoteThread);
					CloseHandle(hProcess);
				}
			}
		}

		CloseHandle(snapshot);
	}
	
*/
	printf("Waiting\n");
	getchar();
	return 0;
}