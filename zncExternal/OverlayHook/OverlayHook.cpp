#include "OverlayHook.hpp"
#include "../rbx.hpp"

static TextEditor Editor;

bool COverlayHook::CreateD3DDevice(HWND handle)
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	RtlZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.BufferDesc.Width = 0;
	SwapChainDesc.BufferDesc.Height = 0;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = handle;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const UINT CreateDeviceFlags = 0;
	D3D_FEATURE_LEVEL D3DFeatureLevel;
	const D3D_FEATURE_LEVEL FeatureLevelArr[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT Result = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr, CreateDeviceFlags, FeatureLevelArr,
		2, D3D11_SDK_VERSION, &SwapChainDesc, &this->DxgiSwapChain, &this->D3D11Device,
		&D3DFeatureLevel, &this->D3D11DeviceContext);

	if (Result == DXGI_ERROR_UNSUPPORTED)
	{
		Result = D3D11CreateDeviceAndSwapChain(nullptr,
			D3D_DRIVER_TYPE_WARP, nullptr, CreateDeviceFlags, FeatureLevelArr,
			2, D3D11_SDK_VERSION, &SwapChainDesc, &this->DxgiSwapChain, &this->D3D11Device,
			&D3DFeatureLevel, &this->D3D11DeviceContext);
	}

	if (Result != S_OK)
		return false;

	this->CreateRenderTarget();
	return true;
}

void COverlayHook::CleanupD3DDevice()
{
	this->CleanupRenderTarget();

	if (this->DxgiSwapChain)
	{
		this->DxgiSwapChain->Release();
		this->DxgiSwapChain = nullptr;
	}

	if (this->D3D11DeviceContext)
	{
		this->D3D11DeviceContext->Release();
		this->D3D11DeviceContext = nullptr;
	}

	if (this->D3D11Device)
	{
		this->D3D11Device->Release();
		this->D3D11Device = nullptr;
	}
}

void COverlayHook::CreateRenderTarget()
{
	ID3D11Texture2D* D3D11BackBuffer;
	this->DxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&D3D11BackBuffer));
	if (D3D11BackBuffer != nullptr)
	{
		this->D3D11Device->CreateRenderTargetView(D3D11BackBuffer, nullptr, &this->D3D11RenderTargetView);
		D3D11BackBuffer->Release();
	}
}

void COverlayHook::CleanupRenderTarget()
{
	if (this->D3D11RenderTargetView)
	{
		this->D3D11RenderTargetView->Release();
		this->D3D11RenderTargetView = nullptr;
	}
}

__forceinline LRESULT __stdcall WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (OverlayHook->D3D11Device != nullptr && wParam != SIZE_MINIMIZED)
		{
			OverlayHook->CleanupRenderTarget();
			OverlayHook->DxgiSwapChain->ResizeBuffers(2, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			OverlayHook->CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

void setupcustomimguistyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 6.0f;
	style.ChildRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;

	style.Alpha = 1.0f;
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;

	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
	colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.54f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
	colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.54f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.54f);
	colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.86f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

bool COverlayHook::Initialize()
{
	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEXW WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEXW);
	WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandleW(nullptr);
	WindowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	WindowClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	WindowClass.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0)));
	WindowClass.lpszClassName = L"znc"; // wide string
	WindowClass.lpszMenuName = nullptr;

	RegisterClassExW(&WindowClass);

	HWND Window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
		WindowClass.lpszClassName,
		L"znc",   // wide string
		WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		nullptr, nullptr,
		WindowClass.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(Window, 0, 255, LWA_ALPHA);

	const MARGINS Margin = { -1 };
	DwmExtendFrameIntoClientArea(Window, &Margin);

	if (!this->CreateD3DDevice(Window))
	{
		this->CleanupD3DDevice();
		UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
		return false;
	}

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;

	setupcustomimguistyle();
	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(this->D3D11Device, this->D3D11DeviceContext);

	const ImVec4 ClearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	bool Init = true, Draw = true, Done = false;
	while (!Done)
	{
		MSG Message;
		while (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessageA(&Message);
			if (Message.message == WM_QUIT)
				Done = true;
		}

		if (Done)
			break;

		const auto TargetWindow = FindWindow(nullptr, L"Roblox");
		const auto ForegroundWindow = GetForegroundWindow();

		if (TargetWindow != ForegroundWindow && Window != ForegroundWindow)
			MoveWindow(Window, 0, 0, 0, 0, true);
		else
		{
			RECT Rect;
			GetWindowRect(TargetWindow, &Rect);

			auto RSizeX = Rect.right - Rect.left;
			auto RSizeY = Rect.bottom - Rect.top;

			auto Status = false;
			MONITORINFO MonitorInfo = { sizeof(MONITORINFO) };
			if (GetMonitorInfoA(MonitorFromWindow(TargetWindow, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
			{
				RECT WindowRect;
				if (GetWindowRect(TargetWindow, &WindowRect))
				{
					Status = WindowRect.left == MonitorInfo.rcMonitor.left
						&& WindowRect.right == MonitorInfo.rcMonitor.right
						&& WindowRect.top == MonitorInfo.rcMonitor.top
						&& WindowRect.bottom == MonitorInfo.rcMonitor.bottom;
				}
			}

			if (Status)
			{
				RSizeX += 16;
				RSizeY -= 24;
			}
			else
			{
				RSizeY -= 63;
				Rect.left += 8;
				Rect.top += 31;
			}
			MoveWindow(Window, Rect.left, Rect.top, RSizeX, RSizeY, 1);
		}

		if ((GetAsyncKeyState(VK_INSERT) & 1)) // open with Insert key (you can change this key) eg. VK_F1 for F1 key, VK_END for End key, etc
			Draw = !Draw;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			if (GetForegroundWindow() == FindWindowA(nullptr, "Roblox") || GetForegroundWindow() == Window) // find roblox window by its title
			{
				if (Draw)
				{
					ImGui::SetNextWindowSize(ImVec2(600, 500));
					ImGui::Begin("zync External", nullptr, ImGuiWindowFlags_NoResize);
					{
						// variables to hold the walkspeed and jumppower values
						static float changespeed = rbx::get_current_walkspeed(); // incase your walkspeed isnt 16
						static float changejp = rbx::get_current_jumppower(); // incase your jumppower isnt 50
						static float changehealth = rbx::get_current_health(); // incase your health isnt 100
						static float changegravity = rbx::get_gravity(); // incase your gravity isnt 196.2
						instance character = rbx::get_char();
						auto descendants = character.get_descendants();
						// main 
						static instance dm = rbx::get_datamodel();
						ImGui::Text("DataModel: %p", (void*)dm.address); // get datamodel address
						ImGui::Separator();
						ImGui::Text("Character address: %p", (void*)character.address); // get character address
						ImGui::Text("Descendants found: %d", descendants.size()); // get number of descendants
						ImGui::Separator();
						static bool enabled = false;
						if (ImGui::Checkbox("NoClip", &enabled)) {
							instance character = rbx::get_char(); // get character
							if (character.valid()) { // if character exists
								auto descendants = character.get_descendants(); // get all descendants of character
								for (auto& part : descendants) { // loop through all descendants
									if (!part.valid()) continue; // if part is not valid then continue

									std::string className = instance::get_class_name(part.address); // get class name of part
									if (className == "Part" || className == "MeshPart") { // if class name is Part or MeshPart then we found a part
										uintptr_t primitive = memory::read<uintptr_t>(part.address + offsets::primitive); // get primitive address
										if (!primitive) continue; // if primitive is null then continue

										BYTE val = memory::read<BYTE>(primitive + offsets::cancollide); // read the CanCollide byte
										if (enabled) // if enabled is true then we disable CanCollide
											val &= ~0x08;   // disable CanCollide
										else
											val |= 0x08;    // enable CanCollide
										memory::write<BYTE>(primitive + offsets::cancollide, val); // write the new value to CanCollide
									}
								}
							}
						}

						

						ImGui::Text("Walkspeed: %.2f", rbx::get_current_walkspeed()); // show current walkspeed
						if (ImGui::SliderFloat("WalkSpeed", &changespeed, 16.0f, 500.0f)) { // float because roblox might have decimal walkspeed eg. 16.5f
							instance humanoid = rbx::get_local_humanoid(); // we get the humanoid
							if (humanoid.valid()) {
								// we use write to write memory as we are changine walkspeed which is a float
								memory::write<float>(humanoid.address + offsets::walkspeed, changespeed);
								memory::write<float>(humanoid.address + offsets::walkspeedcheck, changespeed); // prevent kicks for changing walkspeed
							}
						}
						ImGui::Text("Jumppower: %.2f", rbx::get_current_jumppower()); // show current jumppower
						if (ImGui::SliderFloat("Jumppower", &changejp, 50.0f, 500.0f)) { // float because roblox might have decimal jumppower eg. 50.5f
							instance humanoid = rbx::get_local_humanoid(); // we get the humanoid
							if (humanoid.valid()) { // if the humanoid is valid (exists) then we change the jumppower
								memory::write<float>(humanoid.address + offsets::jumppower, changejp); // we use write to write memory as we are changine jumppower which is a float
							}
						}
						ImGui::Text("Health: %.2f", rbx::get_current_health()); // show current health
						if (ImGui::SliderFloat("Health", &changehealth, 100.0f, 1000.0f)) { // float because roblox might have decimal jumppower eg. 50.5f
							instance humanoid = rbx::get_local_humanoid(); // we get the humanoid
							if (humanoid.valid()) { // if the humanoid is valid (exists) then we change the jumppower
								memory::write<float>(humanoid.address + offsets::jumppower, changejp); // we use write to write memory as we are changine jumppower which is a float
							}
						}
						ImGui::Text("Gravity: %.2f", rbx::get_gravity()); // show current gravity
						if (ImGui::SliderFloat("Gravity", &changegravity, 0.0f, 1000.0f)) { // float because roblox might have decimal jumppower eg. 50.5f
							rbx::set_gravity(changegravity);
						}
					}
					ImGui::End();

					// fps counter
					ImGuiIO& io = ImGui::GetIO(); // get io to get display size
					ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 110, 10)); // set position to top right
					ImGui::SetNextWindowBgAlpha(0.3f); // set background alpha to 30% so its see through
					ImGui::Begin("FPS", nullptr, // create a new window called FPS
						ImGuiWindowFlags_NoTitleBar | // no titlebar
						ImGuiWindowFlags_AlwaysAutoResize | // auto resize to fit text
						ImGuiWindowFlags_NoMove | // cant move it
						ImGuiWindowFlags_NoInputs // ignore inputs (clicks, etc
					);
					float delta = 0.0f; // delta time variable
					float fps = 0.0f; // fps variable

					static auto last = std::chrono::high_resolution_clock::now(); // last time we checked
					auto now = std::chrono::high_resolution_clock::now(); // get current time
					delta = std::chrono::duration<float, std::chrono::seconds::period>(now - last).count(); // calculate delta time
					last = now; // set last to now for next frame

					fps = 1.0f / delta; // calculate fps
					ImGui::Text("FPS: %.1f", fps); // display fps
					ImGui::End(); // end fps window

				}
			}

			if (Draw)
				SetWindowLongA(Window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			else
				SetWindowLongA(Window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

			ImGui::EndFrame();
			ImGui::Render();

			const float ClearColorWithAlpha[4] = { ClearColor.x * ClearColor.w, ClearColor.y * ClearColor.w, ClearColor.z * ClearColor.w, ClearColor.w };
			this->D3D11DeviceContext->OMSetRenderTargets(1, &this->D3D11RenderTargetView, nullptr);
			this->D3D11DeviceContext->ClearRenderTargetView(this->D3D11RenderTargetView, ClearColorWithAlpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			this->DxgiSwapChain->Present(1, 0);

			const float targetFrameTime = 1.0f / 60.0f; // 60 FPS
			static DWORD lastTime = timeGetTime();
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - lastTime) / 1000.0f;

			if (deltaTime < targetFrameTime)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((targetFrameTime - deltaTime) * 1000)));
			}

			lastTime = timeGetTime();
		}
	}

	Init = false;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	this->CleanupD3DDevice();
	DestroyWindow(Window);
	UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
}
