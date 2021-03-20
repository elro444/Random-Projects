#include <Windows.h>




LRESULT CALLBACK MyWindowProc(_In_ HWND   hwnd, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

// For a program with no console, swap 'int main()' with this line: [requires Windows.h]
//int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ INT)
int main()
{
	WNDCLASS lpWndClass;
	ZeroMemory(&lpWndClass, sizeof(lpWndClass));
	lpWndClass.lpszClassName = TEXT("RawInputWindowClass");
	lpWndClass.lpfnWndProc =
		;
	
	RegisterClass(&lpWndClass);




	return 0;
}

LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	return 1;
}
