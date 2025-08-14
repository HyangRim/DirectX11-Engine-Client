#pragma once

#include "Recipe.h"

class RecipeManager
{
    DECLARE_SINGLE(RecipeManager);
    ~RecipeManager();

public:
    void Initialize();
    void LoadRecipesFromFile(const wstring& filePath);

    // 레시피 등록
    void RegisterRecipe(shared_ptr<Recipe> recipe);

    // 특정 아이템을 만들 수 있는 레시피 찾기
    shared_ptr<Recipe> FindRecipeByResult(int32 resultItemID) const;

    // 현재 가진 아이템으로 만들 수 있는 모든 레시피 찾기
    vector<shared_ptr<Recipe>> GetCraftableRecipes(const map<int32, int32>& playerItems) const;

    // 특정 재료로 만들 수 있는 레시피들 찾기
    vector<shared_ptr<Recipe>> GetRecipesByIngredient(int32 ingredientID) const;

private:

    map<int32, shared_ptr<Recipe>> m_recipes; // resultItemID -> Recipe
    multimap<int32, shared_ptr<Recipe>> m_recipesByIngredient; // ingredientID -> Recipe

    void SetupDefaultRecipes();
};

