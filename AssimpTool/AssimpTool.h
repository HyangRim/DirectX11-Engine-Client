#pragma once

#include "IExecute.h"
#include "Scene.h"
class AssimpTool : public Scene
{

public:
	void Start() override;
	void Update() override;
	void Render() override;
};

