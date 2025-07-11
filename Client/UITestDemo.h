#pragma once
#include "IExecute.h"

class UITestDemo : public IExecute
{

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void ShowImguiTransform();

private:
	shared_ptr<Shader> m_shader;

	bool m_TransformImgui = true;
};

