#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include <json.hpp>
using json = nlohmann::json;

struct Recipe {
    std::string name;
    std::string category;
    double posibility;
};

std::vector<Recipe> loadRecipes() {
    std::vector<Recipe> recipes;
    std::ifstream recipeFile("recipes.json");
    
    if (!recipeFile.is_open()) {
        std::cerr << "无法打开 recipes.json" << std::endl;
        return recipes;
    }

    json j;
    try {
        recipeFile >> j;
    } catch (const std::exception& e) {
        std::cerr << "JSON 格式错误: " << e.what() << std::endl;
        return recipes;
    }

    if (!j.contains("recipes") || !j["recipes"].is_array()) {
        std::cerr << "JSON 格式错误: 缺少 \"recipes\" 数组" << std::endl;
        return recipes;
    }

    for (const auto& r : j["recipes"]) {
        Recipe recipe;
        recipe.name = r.value("name", "");
        recipe.category = r.value("category", "");
        recipe.posibility = r.value("posibility", 1.0);

        if (recipe.posibility < 0.0) recipe.posibility = 0.0;

        recipes.push_back(recipe);
    }

    return recipes;
}

bool saveRecipes(const std::vector<Recipe>& recipes) {
    json j;
    j["recipes"] = json::array();

    for (const auto& recipe : recipes) {
        j["recipes"].push_back({
            {"name", recipe.name},
            {"category", recipe.category},
            {"posibility", recipe.posibility}
        });
    }

    std::ofstream recipeFile("recipes.json");
    if (!recipeFile.is_open()) {
        std::cerr << "写入 recipes.json 错误" << std::endl;
        return false;
    }

    recipeFile << j.dump(4) << '\n';
    if (!recipeFile) {
        std::cerr << "保存 recipes.json 错误" << std::endl;
        return false;
    }

    return true;
}

std::size_t pickWeightedRecipeIndex(const std::vector<Recipe>& recipes, std::mt19937& rng) {
    if (recipes.empty()) {
        return std::numeric_limits<std::size_t>::max();
    }

    double totalWeight = 0.0;
    for (const auto& recipe : recipes) {
        if (recipe.posibility > 0.0) {
            totalWeight += recipe.posibility;
        }
    }

    if (totalWeight <= 0.0) {
        std::uniform_int_distribution<std::size_t> distribution(0, recipes.size() - 1);
        return distribution(rng);
    }

    std::uniform_real_distribution<double> distribution(0.0, totalWeight);
    const double target = distribution(rng);

    double cumulativeWeight = 0.0;
    for (std::size_t i = 0; i < recipes.size(); ++i) {
        if (recipes[i].posibility > 0.0) cumulativeWeight += recipes[i].posibility;

        if (target < cumulativeWeight) return i;
    }

    return recipes.size() - 1;
}

int main() {
    std::mt19937 rng(std::random_device{}());

    auto recipes = loadRecipes();
    if (recipes.empty()) {
        std::cerr << "没有加载到任何食谱" << std::endl;
        return EXIT_FAILURE;
    }

    const std::size_t selectedIndex = pickWeightedRecipeIndex(recipes, rng);
    if (selectedIndex == std::numeric_limits<std::size_t>::max()) {
        std::cerr << "没有可用的食谱权重" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "随机选中的食谱是: " << recipes[selectedIndex].name << std::endl;
    std::cout << "类型是: " << recipes[selectedIndex].category << std::endl << std::endl;

    recipes[selectedIndex].posibility /= 2.0;

    if (!saveRecipes(recipes)) {
        return EXIT_FAILURE;
    }

    std::cout << "已将该食谱的概率减半" << std::endl;

    return EXIT_SUCCESS;
}