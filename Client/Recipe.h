#pragma once

struct RecipeIngredient {
    int32 itemID;
    int32 quantity = 1;

    RecipeIngredient(int32 id, int32 qty) : itemID(id), quantity(qty) {}
};

class Recipe
{
public:
    Recipe(int32 resultItemID, const vector<RecipeIngredient>& ingredients);
    ~Recipe();

    // 조합 결과 아이템 ID
    int32 GetResultItemID() const { return m_resultItemID; }

    // 필요한 재료들
    const vector<RecipeIngredient>& GetIngredients() const { return m_ingredients; }

    // 조합 가능 여부 확인
    bool CanCraft(const map<int32, int32>& playerItems) const;

    // 조합 실행 (재료 소모)
    bool ExecuteCraft(map<int32, int32>& playerItems) const;

private:
    int32 m_resultItemID;
    vector<RecipeIngredient> m_ingredients;
};

