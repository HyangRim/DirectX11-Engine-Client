#include "pch.h"
#include "InventoryPanelUI.h"

#include "EquipableItem.h"
#include "Item.h"
#include "ItemSlot.h"

InventoryPanelUI::InventoryPanelUI()
{

}

InventoryPanelUI::~InventoryPanelUI()
{

}

void InventoryPanelUI::Initialize()
{
	LoadResources();
	CreatePanels();
}

void InventoryPanelUI::Update()
{

}

void InventoryPanelUI::SetVisible(bool visible)
{

}

void InventoryPanelUI::Cleanup()
{

}

void InventoryPanelUI::LoadResources()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"ImageShader.fx");

	// 모든 UI 머티리얼에 동일한 설정 적용
	auto SetupUIMaterial = [&](shared_ptr<Material> material) {
		material->SetShader(shader);
		material->SetRenderQueue(RenderQueue::Transparent);
		material->SetTransparent(true);  // 모든 UI에 추가
		material->SetRenderingMode(RenderingMode::Forward);
		};

	wstring prefixPath = L"..\\Resources\\Textures\\UI\\ItemIcon\\";
	vector<int> itemTag = { 110406, 110504, 201413, 202409, 203506, 204408 };

	for (int i = 0; i < 6; i++)
	{
		shared_ptr<Material> itemIcon = make_shared<Material>();
		SetupUIMaterial(itemIcon);

		wstring tag = L"ItemIcon_" + to_wstring(itemTag[i]);
		wstring path = prefixPath + tag + L".png";
		auto itemIconTexture = RESOURCES->Load<Texture>(tag, path);

		itemIcon->SetDiffuseMap(itemIconTexture);
		MaterialDesc& itemIconDesc = itemIcon->GetMaterialDesc();
		itemIconDesc.ambient = Vec4(1.f);
		itemIconDesc.diffuse = Vec4(1.f);
		itemIconDesc.specular = Vec4(1.f);
		RESOURCES->Add(tag, itemIcon);
	}


	
}

void InventoryPanelUI::CreatePanels()
{
	m_panel = make_shared<GameObject>();
	m_panel->SetName(L"CharMainPanel");

	auto panel = make_shared<UIPanel>();
	m_panel->AddComponent(panel);

	panel->Create(Vec2(960.f, 768 - 57), Vec2(252, 62), Vec4(0.f, 0.f, 0.f, 0.f), nullptr);
	m_panel->SetLayerIndex(LAYER_UI);

	CURSCENE->AddUIObject(m_panel, true);
	CURSCENE->RegisterUIParent(m_panel);

	CreateTestItems();
	CreateInventorySlots();
}

void InventoryPanelUI::RegisterUIObject(shared_ptr<GameObject> uiObject)
{

}

void InventoryPanelUI::CreateInventorySlots()
{
	m_inventorySlots.clear();

	// 5x2 그리드로 10개 슬롯 생성
	int slotsX = 5;
	int slotsY = 2;
	Vec2 slotSize = Vec2(46, 28);
	Vec2 spacing = Vec2(5, 5);
	Vec2 startPos = Vec2(960.f - (252 / 2.f) + 23, (768 - 57) - (62 / 2.f) + 14); // 패널 내 시작 위치
	Vec2 panelSize = Vec2(252, 62);

	for (int row = 0; row < slotsY; row++)
	{
		for (int col = 0; col < slotsX; col++)
		{
			int slotIndex = row * slotsX + col;

			// ItemSlot 생성
			shared_ptr<ItemSlot> itemSlot;

			if (row == 0 && col == 0) itemSlot = make_shared<ItemSlot>(m_testItems[0], true);
			else if (row == 0 && col == 1) itemSlot = make_shared<ItemSlot>(m_testItems[1], true);
			else if (row == 0 && col == 2) itemSlot = make_shared<ItemSlot>(m_testItems[2], true);
			else if (row == 0 && col == 3) itemSlot = make_shared<ItemSlot>(m_testItems[3], true);
			else if (row == 0 && col == 4) itemSlot = make_shared<ItemSlot>(m_testItems[4], true);

			else itemSlot = make_shared<ItemSlot>(m_testItems[5], true);

			itemSlot->SetSlotType(SLOTTYPE::INVENTORY);
			Vec2 slotPos = Vec2(
				startPos.x + col * (slotSize.x + spacing.x),
				startPos.y + row * (slotSize.y + spacing.y)
			);
			itemSlot->CreateSlot(slotPos, slotSize, slotIndex);

			m_inventorySlots.push_back(itemSlot);
		}
	}
}

void InventoryPanelUI::CreateTestItems()
{
	m_testItems.clear();

	// 테스트 아이템 1: 일반 무기
	auto weapon = make_shared<EquipableItem>();
	weapon->SetItemID(110406); // 기존에 로드된 아이템 아이콘 ID
	weapon->SetName(L"IronSword");
	weapon->SetDescription(L"날카로운 철검입니다.");
	weapon->SetItemType(ITEMTYPE::EQUIPABLE);
	weapon->SetItemGrade(ITEMGRADE::COMMON);
	weapon->SetEquipType(EquipmentType::WEAPON);

	ItemStatus weaponStatus;
	weaponStatus.attackPower = 50;
	weaponStatus.attackSpeed = 1.2f;
	weapon->SetStatus(weaponStatus);

	m_testItems.push_back(weapon);

	// 테스트 아이템 2: 희귀 방어구
	auto armor = make_shared<EquipableItem>();
	armor->SetItemID(110504);
	armor->SetName(L"IronPlate");
	armor->SetDescription(L"튼튼한 강철 갑옷입니다.");
	armor->SetItemType(ITEMTYPE::EQUIPABLE);
	armor->SetItemGrade(ITEMGRADE::RARE);
	armor->SetEquipType(EquipmentType::CHEST);

	ItemStatus armorStatus;
	armorStatus.defense = 30;
	armorStatus.maxHP = 100;
	armor->SetStatus(armorStatus);

	m_testItems.push_back(armor);

	// 테스트 아이템 3: 전설 아이템
	auto legendary = make_shared<EquipableItem>();
	legendary->SetItemID(201413);
	legendary->SetName(L"DragonSword");
	legendary->SetDescription(L"전설의 드래곤 검입니다.");
	legendary->SetItemType(ITEMTYPE::EQUIPABLE);
	legendary->SetItemGrade(ITEMGRADE::LEGENDARY);
	legendary->SetEquipType(EquipmentType::HEAD);

	ItemStatus legendaryStatus;
	legendaryStatus.attackPower = 150;
	legendaryStatus.attackSpeed = 1.8f;
	legendaryStatus.lifeSteal = 10.0f;
	legendary->SetStatus(legendaryStatus);

	m_testItems.push_back(legendary);

	// 테스트 아이템 4: 전설 아이템
	auto lsword = make_shared<EquipableItem>();
	lsword->SetItemID(202409);
	lsword->SetName(L"LSword");
	lsword->SetDescription(L"L곤 검입니다.");
	lsword->SetItemType(ITEMTYPE::EQUIPABLE);
	lsword->SetItemGrade(ITEMGRADE::LEGENDARY);
	lsword->SetEquipType(EquipmentType::CHEST);

	ItemStatus lStatus;
	lStatus.attackPower = 150;
	lStatus.attackSpeed = 1.8f;
	lStatus.lifeSteal = 10.0f;
	lsword->SetStatus(lStatus);

	m_testItems.push_back(lsword);

	// 테스트 아이템 5: 전설 아이템
	auto legendary2 = make_shared<EquipableItem>();
	legendary2->SetItemID(203506);
	legendary2->SetName(L"L1Sword");
	legendary2->SetDescription(L"L1곤 검입니다.");
	legendary2->SetItemType(ITEMTYPE::EQUIPABLE);
	legendary2->SetItemGrade(ITEMGRADE::LEGENDARY);
	legendary2->SetEquipType(EquipmentType::ARM);

	ItemStatus legendary2Status;
	legendary2Status.attackPower = 150;
	legendary2Status.attackSpeed = 1.8f;
	legendary2Status.lifeSteal = 10.0f;
	legendary2->SetStatus(legendary2Status);

	m_testItems.push_back(legendary2);

	// 테스트 아이템 6: 전설 아이템
	auto legendary3 = make_shared<EquipableItem>();
	legendary3->SetItemID(204408);
	legendary3->SetName(L"L2Sword");
	legendary3->SetDescription(L"L2곤 검입니다.");
	legendary3->SetItemType(ITEMTYPE::EQUIPABLE);
	legendary3->SetItemGrade(ITEMGRADE::LEGENDARY);
	legendary3->SetEquipType(EquipmentType::LEG);

	ItemStatus legendary3Status;
	legendary3Status.attackPower = 150;
	legendary3Status.attackSpeed = 1.8f;
	legendary3Status.lifeSteal = 10.0f;
	legendary3->SetStatus(legendary3Status);

	m_testItems.push_back(legendary3);

}