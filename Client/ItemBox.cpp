#include "pch.h"
#include "ItemBox.h"
#include "Item.h"

ItemBox::ItemBox()
{
	SetType(OBJECTTYPE::ITEMBOX);
}

ItemBox::~ItemBox()
{
}

void ItemBox::Start()
{
	m_collider = make_shared<AABBBoxCollider>();
	AddComponent(m_collider);

	Super::Start();
}

void ItemBox::Update()
{
	Super::Update();
}

void ItemBox::LateUpdate()
{
	Super::LateUpdate();
}

shared_ptr<Item> ItemBox::InsertItem(int _index, shared_ptr<Item> _item)
{
	if (m_boxInventory[_index] != nullptr) {
		auto item = m_boxInventory[_index];
		m_boxInventory[_index] = _item;
		return item;
	}
	else {
		return nullptr;
	}
}

shared_ptr<Item> ItemBox::DeleteItem(int _index)
{
	if (m_boxInventory[_index] == nullptr) return nullptr;
	auto item = m_boxInventory[_index];
	m_boxInventory[_index] = nullptr;
	return item;
}

