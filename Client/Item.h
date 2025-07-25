#pragma once

enum class ITEMTYPE {
	ETC,			//기타(나뭇가지, 돌, 고무 등)
	EQUIPMENT,		//장비(Player가 장착할 수 있음)
	CONSUMABLE		//소비 아이템. (HP)
};

class Item
{
public:
	Item();
	virtual ~Item();

public:
	void SetName(wstring& _name) { m_itemName = _name; }
	void SetName(wstring&& _name) { m_itemName = move(_name); }

	wstring& GetName() { return m_itemName; }

	void SetDescription(wstring& _desc) { m_itemName = _desc; }
	void SetDescription(wstring&& _desc) { m_itemName = move(_desc); }

	wstring& GetDesc() { return m_itemDescription; }

	void SetItemType(ITEMTYPE _type) { m_itemType = _type; }
	ITEMTYPE GetItemType() { return m_itemType; }

	void SetImage(shared_ptr<Texture> _itemImage) { m_itemImage = _itemImage; }
	shared_ptr<Texture> GetImage() { return m_itemImage; }

protected:
	wstring m_itemName;
	wstring m_itemDescription;

	ITEMTYPE m_itemType = ITEMTYPE::ETC;
	shared_ptr<Texture> m_itemImage;
};

