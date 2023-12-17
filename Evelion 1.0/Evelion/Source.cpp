#include "xorstr.hpp"
#include "Functions.h"
#include "memory.h"
#include "Overlay.h"
#include "struct.h"

#include <TlHelp32.h>
#include <thread>
#include <string>
#include "iostream"
#include <cmath>
#include <math.h>
#include <vector>

#include <cstring>

bool ShowMenu = false;
bool Unhook = false;

bool esp = true;
bool enemy_box = true;
bool enemy_name = true;
bool def_models = true;

float gWorldToScreen[16];
PlayerPosition players[32];

RGBA Cyan = { 0, 231, 255, 255 };

const auto memory = Memory{ "hl.exe" };
const auto gameoverlayrenderer = memory.GetModuleAddress("gameoverlayrenderer.dll");

size_t hwSize = 19 * 1024 * 1024;  // 19 MB в байтах
void* hwBuffer = malloc(hwSize);

size_t clientSize = 2 * 1024 * 1024;
void* clientBuffer = malloc(clientSize);

auto hwnd1 = FindWindowA(NULL, "Counter-Strike");
int id = GetWindowThreadProcessId(hwnd1, &Game::PID);

int width;
int height;		

bool ScreenTransform(Vector3 vPoint, float* vScreen)
{
	vScreen[0] = gWorldToScreen[0] * vPoint.x + gWorldToScreen[4] * vPoint.y + gWorldToScreen[8] * vPoint.z + gWorldToScreen[12];
	vScreen[1] = gWorldToScreen[1] * vPoint.x + gWorldToScreen[5] * vPoint.y + gWorldToScreen[9] * vPoint.z + gWorldToScreen[13];
	float z = gWorldToScreen[3] * vPoint.x + gWorldToScreen[7] * vPoint.y + gWorldToScreen[11] * vPoint.z + gWorldToScreen[15];

	if (z == 0.0f)
	{
		return 0.0f >= z;
	}
	else
	{
		vScreen[0] = vScreen[0] * (1.0f / z);
		vScreen[1] = vScreen[1] * (1.0f / z);
		return 0.0 >= (1.0f / z);
	}
}


bool WorldToScreen(Vector3 vPoint, float* vScreen)
{
	bool iResult = ScreenTransform(vPoint, vScreen);
	if (vScreen[0] < 1 && vScreen[1] < 1 && vScreen[0] > -1 && vScreen[1] > -1 && !iResult)
	{
		vScreen[0] = vScreen[0] * (width / 2) + (width / 2);
		vScreen[1] = -vScreen[1] * (height / 2) + (height / 2);
		return true;
	}
	return false;
}

void FullOffestsUpdateThread() {
	while (1) {
		while (esp) {

			memory.ReadModuleMemory("hw.dll", hwBuffer, hwSize);
			memory.ReadModuleMemory("client.dll", clientBuffer, clientSize);

			gWorldToScreen[0] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780);
			gWorldToScreen[1] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 4);
			gWorldToScreen[2] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 8);
			gWorldToScreen[3] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 12);
			gWorldToScreen[4] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 16);
			gWorldToScreen[5] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 20);
			gWorldToScreen[6] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 24);
			gWorldToScreen[7] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 28);
			gWorldToScreen[8] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 32);
			gWorldToScreen[9] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 36);
			gWorldToScreen[10] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 40);
			gWorldToScreen[11] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 44);
			gWorldToScreen[12] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 48);
			gWorldToScreen[13] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 52);
			gWorldToScreen[14] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 56);
			gWorldToScreen[15] = memory.ReadModuleBuffer<float>(hwBuffer, 0xEC9780 + 60);
			
			float screenPositionTemp[2];
			float stateTemp = 9999;

			for (int i = 1; i < 32; i++)
			{

				float playerX = memory.ReadModuleBuffer<float>(hwBuffer, 0x120461C - 0x0250 + i * 0x0250 + 0x0184);

				if (!playerX) continue;

				float playerY = memory.ReadModuleBuffer<float>(hwBuffer, 0x120461C - 0x0250 + i * 0x0250 + 0x0188);
				float playerZ = memory.ReadModuleBuffer<float>(hwBuffer, 0x120461C - 0x0250 + i * 0x0250 + 0x018C);

				Vector3 TargetPos = { playerX , playerY , playerZ };

				WorldToScreen(TargetPos, screenPositionTemp);


				stateTemp = memory.ReadModuleBuffer<float>(hwBuffer, 0x120461C - 0x0250 + i * 0x0250 + 0x017C + 0x1);

				if (stateTemp == players[i].state) {
					if (!players[i].dead) {
						players[i].screenPosition[0] = 0;
						players[i].screenPosition[1] = 0;
						players[i].dead = true;
					}
					players[i].state = stateTemp;
					continue;
				}

				players[i].state = stateTemp;
				players[i].dead = false;
				players[i].screenPosition[0] = screenPositionTemp[0];
				players[i].screenPosition[1] = screenPositionTemp[1];
				
			}

		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

}


void Draw() {
	if (esp) {

		for (int i = 1; i < 32; i++)
		{
			if (players[i].dead) continue;
			float screenPosition[2];

			//Check in lobby
			int lobby = memory.ReadModuleBuffer<char>(hwBuffer, 0x105CFC8);
			if (!lobby) {
				players[i].screenPosition[0] = 0;
				players[i].screenPosition[1] = 0;
				players[i].state = 0;
				continue;
			}

			screenPosition[0] = players[i].screenPosition[0];
			screenPosition[1] = players[i].screenPosition[1];

			if (screenPosition[0] < 2) continue;
			if (screenPosition[1] < 2) continue;

			uintptr_t nameAddress = 0x120461C - 0x0250 + i * 0x0250 + 0x0100;
			std::string name;

			if (enemy_name) {
				char ch1;
				if (enemy_name) {

					do {
						ch1 = memory.ReadModuleBuffer<char>(hwBuffer, nameAddress);
						name.push_back(ch1);
						++nameAddress;
					} while (ch1 != '\0');
				}
			}

			uintptr_t modelAddress = 0x120461C - 0x0250 + i * 0x0250 + 0x012C;

			if (def_models) {
				std::string model;
				char ch2;

				do {
					ch2 = memory.ReadModuleBuffer<char>(hwBuffer, modelAddress);
					model.push_back(ch2);
					++modelAddress;
				} while (ch2 != '\0');


				int team = memory.ReadModuleBuffer<int>(clientBuffer, 0x100DF4);


				if (team == 2 &&
					(model.find("urban") != std::string::npos ||
						model.find("gign") != std::string::npos ||
						model.find("gsg9") != std::string::npos ||
						model.find("sas") != std::string::npos)) {
					continue;
				}
				else if (team == 1 &&
					(model.find("terror") != std::string::npos ||
						model.find("leet") != std::string::npos ||
						model.find("arctic") != std::string::npos ||
						model.find("guerilla") != std::string::npos)) {
					continue;
				}
			}

			if (enemy_box) DrawCircleFilled(screenPosition[0], screenPosition[1], 3, &Cyan);

			if (enemy_name) DrawNewText(screenPosition[0], screenPosition[1] - 32, &Cyan, name.c_str());

		}
	}

}

void GetResolution() {
	uintptr_t widthAddress = gameoverlayrenderer + 0x10EDB8;
	uintptr_t heightAddress = gameoverlayrenderer + 0x10EDB8 + 4;

	width = memory.Read<int>(widthAddress);
	height = memory.Read<int>(heightAddress);
}

void DrawMenu() {
	ImGui::SetNextWindowSize(ImVec2(500.f, 200.f));
	ImGui::Begin("Evelion", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Checkbox("Esp", &esp);
	ImGui::Separator();
	ImGui::Checkbox("Box", &enemy_box);
	ImGui::SameLine();
	ImGui::Checkbox("Name", &enemy_name);
	ImGui::SameLine();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Checkbox("Show Only Enemies", &def_models);
	ImGui::SameLine();
	ImGui::Text("(only works with default models)");
	ImGui::SameLine();

	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::Button("Unhook")) {
		Unhook = true;
	}

	ImGui::Text("Version 1.0 by Zebra");

	ImGui::End();
}

void Render() {
	if (GetAsyncKeyState(VK_DELETE) & 1) Unhook = true;
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;
	if (GetAsyncKeyState(VK_END) & 1) enemy_box = !enemy_box;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	Draw();

	if (ShowMenu)
		DrawMenu();

	ImGui::EndFrame();

	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) {
		if (Unhook) {
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			DestroyWindow(OverlayWindow::Hwnd);
			UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
			exit(0);
		}

		if (PeekMessage(&DirectX9Interface::Message, OverlayWindow::Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == Process::Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = Process::Hwnd;

		POINT TempPoint2;
		GetCursorPos(&TempPoint2);
		io.MousePos.x = TempPoint2.x - TempPoint.x;
		io.MousePos.y = TempPoint2.y - TempPoint.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process::WindowWidth = TempRect.right;
			Process::WindowHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = Process::WindowWidth;
			DirectX9Interface::pParams.BackBufferHeight = Process::WindowHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process::WindowWidth, Process::WindowHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWindow::Hwnd);
	UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process::WindowWidth;
	Params.BackBufferHeight = Process::WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.Fonts->AddFontDefault();

	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};
	
	RegisterClassEx(&OverlayWindow::WindowClass);
	if (Process::Hwnd){
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process::WindowWidth = TempRect.right;
		Process::WindowHeight = TempRect.bottom;
	}

	OverlayWindow::Hwnd = CreateWindowEx(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, Process::WindowLeft, Process::WindowTop, Process::WindowWidth, Process::WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

int processCheck() {

}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	bool WindowFocus = false;

	if (!hwnd1) {
		MessageBox(nullptr, "Please run game before running Evelion!", "Evelion", MB_OK);
		exit(0);
	}


	GetResolution();
	std::thread aliveThread(ProcessAlive);
	std::thread updateThread(FullOffestsUpdateThread);
	
	while (WindowFocus == false) {
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (Game::PID == ForegroundWindowProcessID) {
			Process::ID = Game::PID;
			Process::Hwnd = hwnd1;

			RECT TempRect;
			GetWindowRect(Process::Hwnd, &TempRect);
			Process::WindowWidth = TempRect.right - TempRect.left;
			Process::WindowHeight = TempRect.bottom - TempRect.top;
			Process::WindowLeft = TempRect.left;
			Process::WindowRight = TempRect.right;
			Process::WindowTop = TempRect.top;
			Process::WindowBottom = TempRect.bottom;

			char TempTitle[MAX_PATH];
			GetWindowText(Process::Hwnd, TempTitle, sizeof(TempTitle));
			Process::Title = TempTitle;

			char TempClassName[MAX_PATH];
			GetClassName(Process::Hwnd, TempClassName, sizeof(TempClassName));
			Process::ClassName = TempClassName;

			char TempPath[MAX_PATH];
			GetModuleFileNameEx(Process::Handle, NULL, TempPath, sizeof(TempPath));
			Process::Path = TempPath;

			WindowFocus = true;
		}
	}

	
	std::string s = RandomString(10);
	OverlayWindow::Name = s.c_str();
	SetupWindow();
	DirectXInit();
	while (TRUE) {
		MainLoop();
	}
}