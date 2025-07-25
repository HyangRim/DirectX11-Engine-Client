#pragma once
#include <functional>
class Scene;

struct GameDesc
{
	//shared_ptr<class IExecute> app = nullptr;
	std::function<shared_ptr<Scene>()> createInitialScene = nullptr;
	wstring appName = L"GameCoding";
	HINSTANCE hInstance = 0;
	HWND hWnd = 0;
	float width = 1366;
	float height = 768;
	bool vsync = true;
	bool windowed = true;
	Color clearColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
};

class Game
{
	DECLARE_SINGLE(Game);

private:
	~Game();

public:
	WPARAM Run(GameDesc& desc);

	GameDesc& GetGameDesc() { return _desc; }

private:
	ATOM MyRegisterClass();
	BOOL InitInstance(int cmdShow);

	void Update();
	void ShowFPS();

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	GameDesc _desc;
};

