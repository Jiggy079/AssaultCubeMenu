#include "gui.h"
#include "Cheat.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) {
		return true;
	}

	switch (message) {
		case WM_SIZE: {
			if (gui::device && wideParameter != SIZE_MINIMIZED) {
				gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
				gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			}
		} return 0;
	
		case WM_SYSCOMMAND: {
			if ((wideParameter & 0xfff0) == SC_KEYMENU) { // disable ALT menu
				return 0;
			}
		} break;

		case WM_DESTROY: {
			PostQuitMessage(0);
		} return 0;

		case WM_LBUTTONDOWN: {
			gui::position = MAKEPOINTS(longParameter);
		} return 0;

		case WM_MOUSEMOVE: {
			if (wideParameter == MK_LBUTTON) {
				const auto points = MAKEPOINTS(longParameter);
				RECT rect = {};

				GetWindowRect(gui::window, &rect);

				rect.left += points.x - gui::position.x;
				rect.top += points.y - gui::position.y;

				if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH && gui::position.y >= 0 && gui::position.y <= 19) {
					SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
	} 

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName, const char* className) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.style = CS_CLASSDC;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept {
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}



bool gui::CreateDevice() noexcept {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) {
		return false;
	}

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0) {
		return false;
	}
	
	return true;
}

void gui::ResetDevice() noexcept {
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL) {
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();

}

void gui::DestroyDevice() noexcept {
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}



void gui::CreateImGui() noexcept {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}



void gui::BeginRender() noexcept {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept {
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ResetDevice();
	}
}

void gui::Render() noexcept {
	static Cheat cheat = Cheat();
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("Cheat Menu", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Made by Jiggy");
	static int pid;
	static bool attached = false;
	static int clicked = 0;
	static bool godMode = false;

	if (ImGui::CollapsingHeader("Setup")) {
		if (attached) {
			ImGui::BeginDisabled();
		}
		ImGui::InputInt("Process ID", &pid);
		if (ImGui::Button("Attach")) {
			clicked++;
			if (cheat.attach(pid)) {
				attached = true;
			}
			else {
				attached = false;
			}
		}
		if (attached) {
			ImGui::EndDisabled();
		}
		if (clicked) {
			if (attached) {
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1.0f, 0, 1.0f), "Attached successfully!");
			}
			else {
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Could not attach, check PID.");
			}
		}
	}
	
	
	if (!attached) {
		ImGui::BeginDisabled(true);
	}

	if (ImGui::CollapsingHeader("Health/Armour")) {
		// god mode
		ImGui::Checkbox("God Mode", &godMode);
		if (godMode) {
			ImGui::BeginDisabled(true);
			cheat.enableGodMode(&godMode);
		}

		// set health
		static int health = 100;
		ImGui::InputInt("Health", &health);
		ImGui::SameLine();
		if (ImGui::Button("Set")) {
			cheat.setHealth(health);
		}

		if (godMode) {
			ImGui::EndDisabled();
		}
	}

	if (!attached) {
		ImGui::EndDisabled();
	}

	if (ImGui::CollapsingHeader("Debug")) {
		ImGui::Text(cheat.isAttached() ? "cheat->attached : true" : "cheat->attached : false");
		ImGui::Text("playerBase  :  ");
		ImGui::Text(godMode ? "godMode : true" : "godMode : false");
	}
	

	ImGui::End();
}