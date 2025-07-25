#include "pch.h"
#include "Game.h"
#include "IExecute.h"
#include "Scene.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Game::~Game()
{

}

WPARAM Game::Run(GameDesc& desc)
{
	_desc = desc;
	//assert(_desc.app != nullptr);

	// 1) 윈도우 창 정보 등록
	MyRegisterClass();

	// 2) 윈도우 창 생성
	if (!InitInstance(SW_SHOWNORMAL))
		return FALSE;
		
	GRAPHICS->Init(_desc.hWnd);
	TIME->Init();
	INPUT->Init(_desc.hWnd);
	GUI->Init();
	RESOURCES->Init();
	SOUND->Init();
	RENDER->Init();
	
	////_desc.app->Init();
	//  // 초기 씬 설정
	////auto startScene = make_shared<StartScene>();
	////SCENE->ChangeScene(startScene);
	//SCENE->ChangeScene(SCENE->GetCurScene());
	// 
	// 콜백을 통한 초기 씬 생성 및 설정
	if (_desc.createInitialScene)
	{
		auto initialScene = _desc.createInitialScene();
		SCENE->ChangeScene(initialScene);
	}

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}

	return msg.wParam;
}


ATOM Game::MyRegisterClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _desc.hInstance;
	wcex.hIcon = ::LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _desc.appName.c_str();
	wcex.hIconSm = wcex.hIcon;

	return RegisterClassExW(&wcex);
}

BOOL Game::InitInstance(int cmdShow)
{
	RECT windowRect = { 0, 0, static_cast<LONG>(_desc.width), static_cast<LONG>(_desc.height) };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	_desc.hWnd = CreateWindowW(_desc.appName.c_str(), _desc.appName.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, _desc.hInstance, nullptr);

	if (!_desc.hWnd)
		return FALSE;

	::ShowWindow(_desc.hWnd, cmdShow);
	::UpdateWindow(_desc.hWnd);

	return TRUE;
}

LRESULT CALLBACK Game::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	//INGUI IO처리. 
	if (ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_SIZE:
		break;
	case WM_MOUSEWHEEL:
	{
		// 마우스 휠 델타 값 추출
		int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		
		INPUT->OnMouseWheel(wheelDelta);
		return 0;
	}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(handle, message, wParam, lParam);
	}

	return 0;
}

void Game::Update()
{
	TIME->Update();
	INPUT->Update();
	ShowFPS();

	GRAPHICS->RenderBegin();


	GUI->Update();

	SCENE->Update();

	//_desc.app->Update();
	//_desc.app->Render();

	GUI->Render();

	GRAPHICS->RenderEnd();
}

void Game::ShowFPS()
{
	uint32 fps = GET_SINGLE(TimeManager)->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(_desc.hWnd, text);
}

