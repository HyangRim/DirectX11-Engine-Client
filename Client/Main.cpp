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

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	GameDesc desc;
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance;
	desc.vsync = true;
	desc.hWnd = NULL;
	desc.width = 1366;
	desc.height = 768;
	desc.clearColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
	desc.app = make_shared<BillboardDemo>();

	GAME->Run(desc);

	return 0;
}