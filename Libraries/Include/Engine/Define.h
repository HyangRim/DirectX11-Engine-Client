#pragma once

#define DECLARE_SINGLE(classname)			\
private:									\
	classname() { }							\
public:										\
	static classname* GetInstance()			\
	{										\
		static classname s_instance;		\
		return &s_instance;					\
	}

#define GET_SINGLE(classname)	classname::GetInstance()


#define CHECK(p)	assert(SUCCEEDED(p))
#define GAME		GET_SINGLE(Game)		
#define GRAPHICS	GET_SINGLE(Graphics)
#define DEVICE		GRAPHICS->GetDevice()
#define DC			GRAPHICS->GetDeviceContext()
#define INPUT		GET_SINGLE(InputManager)
#define TIME		GET_SINGLE(TimeManager)
#define DT			TIME->GetDeltaTime()
#define RESOURCES	GET_SINGLE(ResourceManager)
#define RENDER		GET_SINGLE(RenderManager)
#define GUI			GET_SINGLE(ImGuiManager)
#define SCENE		GET_SINGLE(SceneManager)
#define CURSCENE	SCENE->GetCurScene()


#define GET_TECH(_isShadow) _isShadow == true ? 1 : 0


enum LayerMask {
	LAYER_DEFAULT = 0,
	LAYER_UI = 1
};