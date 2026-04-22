# Weekly-Random-Recipe

随机刷新菜谱的小应用

## 使用说明
1. **下载项目**

从 **Releases** 下载 或者 通过一下命令**从源码构建**
```bash
git clone https://github.com/sglwsjxh/Weekly-Random-Recipe.git
cd Weekly-Random-Recipe

g++ -std=c++17 main.cpp -Iinclude -o weekly-random-recipe.exe
```

**2. 将菜谱信息复制到 `recipes.json` 文件中**
```bash
cp recipes.json.example recipes.json
```

**3. 运行 `main.cpp`，随机输出一个菜谱，并将该菜谱的概率减半**