#include "pch.h"
#include "ImGuiManager.h"

void ImGuiManager::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(GAME->GetGameDesc().hWnd);
	ImGui_ImplDX11_Init(DEVICE.Get(), DC.Get());
}

void ImGuiManager::End()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::ShowPickedObj()
{
	auto curPickedObj = CURSCENE->GetPickedObj();

	//메모리 영역이 유효할 경우에.
	if (!curPickedObj.expired()) {

		//가져온 GameObject 정보를 ImGui에 넣기. 
		auto vTranslate = curPickedObj.lock()->GetTransform()->GetLocalPosition();
		auto vRotation = curPickedObj.lock()->GetTransform()->GetLocalRotation();
		vRotation = (vRotation / XM_PI) * 180.f;
		auto vScale = curPickedObj.lock()->GetTransform()->GetLocalScale();

		float vPos[3] = { vTranslate.x, vTranslate.y, vTranslate.z };
		float vRot[3] = { vRotation.x, vRotation.y, vRotation.z };
		float vSca[3] = { vScale.x, vScale.y, vScale.z };

		ImGui::Text("Transform");
		ImGui::SameLine();
		if (ImGui::InputFloat3("Local Position", vPos)) {
			vTranslate.x = vPos[0];
			vTranslate.y = vPos[1];
			vTranslate.z = vPos[2];
			curPickedObj.lock()->GetTransform()->SetLocalPosition(vTranslate);
		}

		ImGui::Text("Rotation");
		ImGui::SameLine();
		if (ImGui::InputFloat3("Local Rotation", vRot)) {
			vRotation.x = vRot[0];
			vRotation.y = vRot[1];
			vRotation.z = vRot[2];
			curPickedObj.lock()->GetTransform()->SetLocalRotation(vRotation);
		}

		ImGui::Text("Scale");
		ImGui::SameLine();
		if (ImGui::InputFloat3("Local Scale", vSca)) {
			vScale.x = vSca[0];
			vScale.y = vSca[1];
			vScale.z = vSca[2];
			curPickedObj.lock()->GetTransform()->SetLocalScale(vScale);
		}
	}
}
