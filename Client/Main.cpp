#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "SceneDemo.h"
#include "RawBufferDemo.h"
#include "GroupBufferDemo.h"
#include "TextureBufferDemo.h"
#include "StructuredDemo.h"
#include "ViewportDemo.h"
#include "OrthoGraphicDemo.h"
#include "ButtonDemo.h"
#include "BillboardDemo.h"
#include "SnowBillboardDemo.h"
#include "ParticleDemo.h"
#include "UITestDemo.h"
#include "dxgidebug.h"
#include "LumiaIsland.h"
#include "StartScene.h"
#include "CharacterSelectScene.h"

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

void D3DMemoryLickCheck()
{
	HMODULE dxgidebugdll = GetModuleHandleW(L"dxgidebug.dll");
	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));

	IDXGIDebug* debug;

	GetDebugInterface(IID_PPV_ARGS(&debug));


	OutputDebugStringW(L"▼▼▼▼▼▼▼▼▼▼▼▼▼Direct3D Object ref count 메모리 누수 체크 ▼▼▼▼▼▼▼▼▼▼▼▼\r\n");

	debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);

	OutputDebugStringW(L"▲▲▲▲▲▲▲▲▲▲▲▲▲반환되지 않은 IUnknown 객체가 있을경우 위에 나타남 ▲▲▲▲▲▲▲▲▲▲▲▲\r\n");

	debug->Release();
}




#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
	GameDesc desc;
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance;
	desc.vsync = true;
	desc.hWnd = NULL;
	desc.width = 1366;
	desc.height = 768;
	desc.clearColor = Color(0.f, 0.f, 0.f, 0.f);
	//desc.app = make_shared<StartScene>();
	  // 초기 씬 생성 콜백 설정
	desc.createInitialScene = []() -> shared_ptr<Scene> {
		return make_shared<LumiaIsland>();
		};

	GAME->Run(desc);


#ifdef _DEBUG
	D3DMemoryLickCheck();
#endif

	return 0;
}