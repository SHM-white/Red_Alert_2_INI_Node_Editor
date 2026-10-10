# RA2 Node Studio — Qt 6 Revival

A node-oriented graphical editor for Red Alert 2 / Yuri's Revenge INI mod files.

The repository also contains the original Rine, GraphicsControl and ImGui/INIBrowser experiments. They are preserved for reference; the new application builds from the **root CMakeLists.txt**, not the old Visual Studio solution.

## PR2 × PR3 UI / theme integration

The integration branch preserves PR2's tested INI editing and raw source workflow, while adopting PR3's **three-pane desktop layout**: section explorer with type filter (left), graph plus source tab (center), and always-visible property inspector (right). The UI now follows a **VS2022-inspired straight-edged, compact flat visual system**: zero corner radii, thin borders, subdued toolbar/panel surfaces, and square graph cards with slim category accents. (This is an inspiration, not a copy of Microsoft branding.)

**Theme profiles** now include eight UI colors and eleven independently stored node-category colors. The toolbar theme selector switches the entire profile; its **Edit Theme** button opens a palette editor. Built-in themes are read-only templates: editing them creates a new custom copy. Use the Theme menu to import/export JSON, delete custom profiles, or quickly recolor the currently selected node category. Custom profiles persist in QSettings. Older PR2 node color preferences are migrated when opening the built-in theme.

Theme JSON schema is documented in [docs/THEMES.md](docs/THEMES.md). Neither theme files nor color preferences modify the underlying INI file.

## Implemented

- Standalone Qt 6 Widgets application with CMake/C++20.
- Four themes: deep dark, light, ocean blue and red/black. Persisted UI settings and custom color for each node category.
- Color-coded cards for Infantry, Vehicle, Aircraft, Building, Weapon, Warhead, Projectile, Animation, Superweapon, Registry, Other. Categories are heuristically detected from RA2 type lists and object keys.
- Indexed incoming/outgoing section references, local relationship graph, panning, zooming, double-click node navigation. Graph instantiates at most 14 upstream and 14 downstream sections at a time.
- Section search, property table, section and property add/delete/rename/value edit, undo/redo (30 snapshot commands).
- Optional source editing tab; changes apply automatically upon leaving it, or by clicking Apply.
- File New/Open/Save/Save As with unsaved-change prompts.
- Order-preserving line-oriented INI writer: comments, unknown properties, section ordering and type-registry indexes are not rewritten into unordered maps. Unmodified saves retain exact original file bytes. Edited saves use QSaveFile safe writes.
- UTF-8 or system-native ANSI decoding; invalid UTF-8 produces an error rather than silently corrupting text.
- Minimal headless CTest checks for parsing, roundtrip, editing, category classification and references.

## Build

Requirements: Qt 6.5+ (Widgets), CMake 3.21+, and C++20 compiler.

    cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/compiler
    cmake --build build --config Release --parallel
    ctest --test-dir build -C Release --output-on-failure

For Windows, use a Qt package matching your MSVC or MinGW compiler. GitHub Actions runs Windows and Linux builds and uploads a Windows windeployqt deployment folder as a workflow artifact.

## Encoding and mod safety

Some Chinese RA2 mods use GBK/ANSI. On a Chinese-locale Windows installation, choose **System/ANSI before opening** a legacy INI. System encoding is OS-locale dependent; on Linux or an English-locale Windows machine it may not decode GBK. A separate GBK codec selector is planned.

INI section and key ordering can be game-significant; sorting and reconstruction can break mods. Always back up game data before large-scale edits. On modifications, mixed line endings may be normalized; completely unmodified content is preserved byte-for-byte. Semicolons in values are treated as comments.

## Known limits

This is a substantial first-pass refactor, **not yet a full RA2/YR/Ares/Phobos-aware mod IDE**. Edges are inferred from exact referenced section-name tokens (including comma-separated lists), so special key syntax, inheritance, included files, unresolved external sections and game-specific typed references are not fully resolved. Category detection is heuristic. Graph covers only a bounded one-hop neighborhood. Deleted sections remove their associated line range. Undo snapshots can consume memory for very large files, and the parser currently runs on the UI thread.

See docs/ROADMAP.md for remaining correctness work, benchmarks, multi-file workspaces and build packaging milestones.

## GUI initialization smoke test

The CTest suite also instantiates the full main window in an offscreen Qt backend, via `RA2NodeStudio --smoke-test`, to catch crashes during theme loading and three-pane UI construction on Windows and Linux. GUI controls still benefit from hands-on desktop interaction tests.

### Flat UI and regression coverage

The square-corner styling applies to the toolbar, dock-like panes, table headers, tabs, inputs, buttons, menus, group boxes and graph cards. The theme dialog uses small color preview swatches, editable HEX fields and a picker, rather than entire saturated colored buttons. Theme profiles still switch all 11 node-category colors independently. An offscreen UI regression test verifies that an empty category filter clears the selection and inspector, and that switching back restores the matching section.

## Adjustable relationship depths

The center pane's graph header now provides independent **父节点最大深度** (upstream) and **子节点最大深度** (downstream) spin boxes. Both accept **0–5**, default to 1, and persist in QSettings as graph/parentDepth and graph/childDepth. Setting either to 0 hides that direction while preserving the other. Changes rebuild only the displayed neighborhood; they do not alter INI contents. The 适应画布 button fits the expanded graph in the viewport.

Graph traversal uses bounded, cycle-safe BFS. Each depth column can include up to **14** nodes, and each side can include up to **64** nodes total. These limits are distinct from depth; on dense mods, some deeper nodes may be omitted to stay responsive. In cyclic graphs a section may appear once on each side, because parent and child traversals have independent visited sets; this preserves both directed views. Drawn edges reflect the spanning discovery tree, not every possible shortcut or cyclic link.

graph_neighborhood_tests covers independent depth settings, 0-depth hiding, multi-hop oriented edges, cycle handling, breadth/total caps and invalid selections. The offscreen UI test exercises the controls against a fixture and verifies the relation model updates.
