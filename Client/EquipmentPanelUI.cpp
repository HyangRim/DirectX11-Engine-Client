#include "pch.h"
#include "EquipmentPanelUI.h"

#include "ItemSlot.h"

EquipmentPanelUI::EquipmentPanelUI()
{

}
EquipmentPanelUI::~EquipmentPanelUI()
{

}

void EquipmentPanelUI::Update()
{

}

void EquipmentPanelUI::Initialize()
{
	LoadResources();
	CreatePanels();
}

void EquipmentPanelUI::SetVisible(bool visible)
{
	m_isVisible = visible;
	m_panel->GetUIPanel()->SetVisible(visible);
}

void EquipmentPanelUI::Cleanup()
{

}


void EquipmentPanelUI::LoadResources()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixTag = L"Ico_Status_";
	vector<wstring> slotTag = { L"Weapon", L"Armor", L"Head", L"Arm", L"Leg" };
	wstring prefixPath = L"..\\Resources\\Textures\\UI\\CharEquipmentIcon\\";

	for (int i = 0; i < slotTag.size(); i++)
	{
		shared_ptr<Material> charEquipIcon = make_shared<Material>();
		SetupUIMaterial(charEquipIcon);

		wstring tag = prefixTag + slotTag[i];
		wstring path = prefixPath + tag + L".png";
		auto charEquipTexture = RESOURCES->Load<Texture>(tag, path);

		charEquipIcon->SetDiffuseMap(charEquipTexture);
		MaterialDesc& charEquipDesc = charEquipIcon->GetMaterialDesc();
		charEquipDesc.ambient = Vec4(1.f);
		charEquipDesc.diffuse = Vec4(1.f);
		charEquipDesc.specular = Vec4(1.0f);
		RESOURCES->Add(tag, charEquipIcon);
	}





	prefixPath = L"..\\Resources\\Textures\\UI_Btn\\";
	wstring prefixBtnTag = L"Img_Item_Slot_";
	vector<wstring> gradeTag = { L"Common", L"Uncommon", L"Rare", L"Epic", L"Legendary" };

	for (int i = 0; i < 5; i++)
	{
		shared_ptr<Material> btnImg = make_shared<Material>();
		SetupUIMaterial(btnImg);

		wstring tag = prefixBtnTag + gradeTag[i];
		wstring path = prefixPath + tag + L".png";
		auto btnImgTexture = RESOURCES->Load<Texture>(tag, path);

		btnImg->SetDiffuseMap(btnImgTexture);
		MaterialDesc& btnImgDesc = btnImg->GetMaterialDesc();
		btnImgDesc.ambient = Vec4(1.f);
		btnImgDesc.diffuse = Vec4(1.f);
		btnImgDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, btnImg);
	}
}


void EquipmentPanelUI::CreatePanels()
{
	m_panel = make_shared<GameObject>();
	m_panel->SetName(L"CharEquipPanel");

	auto panel = make_shared<UIPanel>();
	m_panel->AddComponent(panel);

	panel->Create(Vec2(380.f, 768 - 57), Vec2(38, 115), Vec4(0.f, 0.f, 0.f, 0.7f), nullptr);
	m_panel->SetLayerIndex(LAYER_UI);

	CURSCENE->AddUIObject(m_panel, true);
	CURSCENE->RegisterUIParent(m_panel);

	CreateEquipmentSlots();
}

void EquipmentPanelUI::CreateEquipmentSlots()
{
	m_equipmentSlots.clear();

	// 5x2 그리드로 10개 슬롯 생성
	int slotsX = 1;
	int slotsY = 5;
	Vec2 slotSize = Vec2(34, 22);
	Vec2 spacing = Vec2(0, 1);

	Vec2 startPos = Vec2(380.f - (38 / 2.f) + 19, (768 - 57) - (115 / 2.f) + 13); // 패널 내 시작 위치
	Vec2 panelSize = Vec2(38, 115);

	for (int row = 0; row < slotsY; row++)
	{
		for (int col = 0; col < slotsX; col++)
		{
			int slotIndex = row * slotsX + col;

			// ItemSlot 생성
			auto itemSlot = make_shared<ItemSlot>(nullptr, false);
			itemSlot->SetSlotType(SLOTTYPE::EQUIPMENT);
			Vec2 slotPos = Vec2(
				startPos.x + col * (slotSize.x + spacing.x),
				startPos.y + row * (slotSize.y + spacing.y)
			);
			itemSlot->CreateSlot(slotPos, slotSize, slotIndex);

			m_equipmentSlots.push_back(itemSlot);
		}
	}
}

void EquipmentPanelUI::RegisterUIObject(shared_ptr<GameObject> uiObject)
{

}
