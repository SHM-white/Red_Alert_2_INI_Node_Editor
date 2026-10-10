# RA2 INI Node Editor · RINE Modern

为《红色警戒 2》/《尤里的复仇》的 INI 配置设计的现代化节点编辑器（Qt 6 Widgets / CMake / C++20）。

> 此分支的 `src/` 是**重新实现的现代版应用**；原 `Rine/`、`INIBrowser/`、`GraphicsControl/` 等目录保留用于对照，不参与新构建。旧版的 Visual Studio 项目和 `.pro` 文件不再是默认入口。

## 使用

- **文件**：新建、打开、保存、另存为；未保存时关闭/切换文件会提示。可通过命令行传入 `.ini` 路径。
- **节浏览器**：即时搜索（100 ms 防抖）、类型筛选、节的新增/重命名/删除。
- **可视化关系图**：左侧是引用当前节的其他对象，右侧是当前节引用的对象；节点按类型上色，点击可跳转，鼠标滚轮缩放和拖动画布。每侧最多显示 14 个直接关联对象，防止大文件形成巨量图元。
- **属性检查器**：直接双击编辑键名或值，支持新增和删除属性。
- **编辑**：最多保留 20 步撤销/重做，保存后标记为干净状态。
- **外观**：午夜蓝、深灰、浅色 3 套主题，自动记住选择和窗口大小。
- **数据安全**：原样保留未修改行、注释、顺序、行尾格式和 UTF-8 BOM；使用 `QSaveFile` 原子保存。通过 CTest 验证读写往返和编辑行为。

## 下载 Windows 64 位程序

打开 [GitHub Actions](../../actions/workflows/build.yml) → 选择最新的绿色构建 → 在页面底部 **Artifacts** 下载 `RA2-INI-Node-Editor-windows-x64`，解压后运行 `RA2-INI-Node-Editor.exe`。软件包附带 Qt 运行时 DLL，不需要单独安装 Qt。

工作流在 `modernize/**` 分支 push、PR 和手动触发时运行。测试作业使用 Ubuntu 验证 INI 核心；Windows 作业使用 MSVC 2022、Qt 6.8.3，部署 Qt DLL 并上传程序包。

## 本地构建

Qt 6.5+、CMake 3.24+、支持 C++20 的编译器：

```sh
cmake -S . -B build -DRA2_BUILD_GUI=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

仅测试 INI 引擎（无需 Qt）：

```sh
cmake -S . -B build-core -DRA2_BUILD_GUI=OFF
cmake --build build-core --parallel
ctest --test-dir build-core --output-on-failure
```

## 节点颜色

| 类型 | 默认颜色 | 识别方式 |
| --- | --- | --- |
| 步兵 Infantry | 青绿 | `[InfantryTypes]` 列表 |
| 载具 Vehicle | 橙金 | `[VehicleTypes]` 列表 |
| 航空器 Aircraft | 蓝 | `[AircraftTypes]` 列表 |
| 建筑 Building | 紫 | `[BuildingTypes]` 列表 |
| 武器 Weapon | 粉红 | `Damage` / `ROF` / `Warhead` / `Projectile` 属性启发式识别 |
| 弹头 Warhead | 珊瑚红 | `[Warheads]` 或其他对象的 `Warhead=` 引用 |
| 弹体 Projectile | 浅蓝 | `[Projectiles]` 或 `Projectile=` 引用 |
| 其他 | 中性灰蓝 | 无法分类的节 |

配色是**编辑器辅助识别**，不是游戏引擎的硬性分类规则；同一 ID 出现在多个分类表时后面的映射会覆盖前面的。

## 实现细节与已知边界

- INI 后端以原始字节和行索引为基础，不会因加载文件或切换节点而重写整个文件；保存会保留未修改行的原始编码与注释。
- 自动识别 UTF-8 BOM / 合法 UTF-8，其余视为系统本地 ANSI 编码。在简体中文 Windows 上，通常对应 CP936/GBK。**跨操作系统打开 GBK 时可能乱码**；暂不提供手动编码下拉或编码转换。
- 通过值与节名的**完全匹配**建立引用索引，不解析 `A,B,C` 等复合值，不处理跨文件继承或 `#include`，也不自动修复重命名后其他节中的引用。
- 解析宽松，目标是 RA2 常用的 `[Section]` / `key=value` 格式；编辑行保留键值前后格式，分号注释仅在值开始处或空白符后识别。
- 节和键名采用 ASCII 不区分大小写进行唯一性校验；遇到旧文件原有重复键时保留并显示，不会加载时静默丢弃。
- 撤销重做采取有上限的文档快照，兼顾正确性和实现复杂度；极大文件连续修改仍有进一步压缩命令的空间。
- 当前实现是**可运行的基础现代化编辑器**，不是完整的红警 2 语义分析器或所有历史实验模块的移植。尚未完成跨文件引用、类型专用表单、复合值解析、批量编辑、语法验证与插件扩展。

## 后续规划

参见 [docs/ROADMAP.md](docs/ROADMAP.md)。欢迎针对不同 Mod 的具体 INI 语法补充测试样例与规则配置。
