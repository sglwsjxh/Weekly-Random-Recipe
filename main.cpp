#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <json.hpp>
using json = nlohmann::json;

struct Rec {
    std::string name;
    std::string cat;
    double p;
};

std::vector<Rec> loadRecipes() {
    std::ifstream f("recipes.json");
    if (!f.is_open()) {
        std::cerr << "无法打开 recipes.json" << std::endl;
        return {};
    }

    json j;
    try {
        f >> j;
    } catch (const std::exception& e) {
        std::cerr << "JSON 格式错误: " << e.what() << std::endl;
        return {};
    }

    if (!j.contains("recipes") || !j["recipes"].is_array()) {
        std::cerr << "JSON 格式错误: 缺少 \"recipes\" 数组" << std::endl;
        return {};
    }

    std::vector<Rec> rs;
    for (const auto& x : j["recipes"]) {
        Rec r;
        r.name = x.value("name", "");
        r.cat = x.value("category", "");
        r.p = x.value("posibility", 1.0);
        if (r.p < 0.0) r.p = 0.0;
        rs.push_back(r);
    }
    return rs;
}

bool saveRecipes(const std::vector<Rec>& rs) {
    json j;
    j["recipes"] = json::array();

    for (const auto& r : rs) {
        j["recipes"].push_back({
            {"name", r.name},
            {"category", r.cat},
            {"posibility", r.p}
        });
    }

    std::ofstream f("recipes.json");
    if (!f.is_open()) {
        std::cerr << "写入 recipes.json 错误" << std::endl;
        return false;
    }

    f << j.dump(4) << '\n';
    if (!f) {
        std::cerr << "保存 recipes.json 错误" << std::endl;
        return false;
    }
    return true;
}

std::size_t pickRecipeIndex(const std::vector<Rec>& rs, std::mt19937& rng) {
    std::vector<double> ws;
    ws.reserve(rs.size());
    double sum = 0.0;
    for (const auto& r : rs) {
        const double w = r.p > 0.0 ? r.p : 0.0;
        ws.push_back(w);
        sum += w;
    }

    if (sum <= 0.0) {
        std::uniform_int_distribution<std::size_t> d(0, rs.size() - 1);
        return d(rng);
    }

    std::discrete_distribution<std::size_t> d(ws.begin(), ws.end());
    return d(rng);
}

int main() {
    std::mt19937 rng(std::random_device{}());

    auto rs = loadRecipes();
    if (rs.empty()) {
        std::cerr << "没有加载到任何食谱" << std::endl;
        return EXIT_FAILURE;
    }

    const std::size_t i = pickRecipeIndex(rs, rng);

    std::cout << "随机选中的食谱是: " << rs[i].name << std::endl;
    std::cout << "类型是: " << rs[i].cat << std::endl << std::endl;

    rs[i].p /= 2.0;

    if (!saveRecipes(rs)) return EXIT_FAILURE;

    std::cout << "已将该食谱的概率减半" << std::endl;

    return EXIT_SUCCESS;
}