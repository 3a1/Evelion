#pragma once
#include "Overlay.h"
#include <string>

float boxTemp[3] = { 0, 231, 255 };
float nameTemp[3] = { 0, 231, 255 };

RGBA BoxColor = {0, 231, 255, 255};
RGBA NameColor = {0, 231, 255, 255};

bool ShowMenu = false;
bool Unhook = false;

bool esp = true;
bool enemy_box = true;
bool enemy_name = true;
bool def_models = true;
bool in_lobby = false;

struct PlayerPosition {
	float screenPosition[2];
	float state;
	bool dead;
	std::string name;
};

PlayerPosition players[64];

namespace DirectX9Interface {
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}

namespace OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}

class Vector2 {
public:
	float x, y;
};

namespace Process {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
}

namespace Game {
	HANDLE handle = NULL;
	uintptr_t client = NULL;
	uintptr_t engine = NULL;
	DWORD PID = 0;
}