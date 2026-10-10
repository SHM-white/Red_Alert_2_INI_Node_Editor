# RA2 Node Studio v0.2.0 — Qt 6 Revival

First public portable Windows build of the redesigned Red Alert 2 / Yuri's Revenge INI node editor.

## Highlights
- Qt 6.8.3 / CMake / C++20 modern desktop application, retaining the original prototype code as reference
- Three-pane section explorer, relationship graph / source editor, and editable property inspector
- VS2022-inspired square-corner flat UI; four preset themes and shareable custom themes, each with independent colors for 11 node categories
- Add/rename/delete INI sections and properties, raw-source edits, undo/redo, safe save and preserved original byte representation when unmodified
- Indexed, selectable multi-level graph: independent parent and child depth limits 0–5, default 1, persisted per user
- Bounded cycle-safe BFS; max 14 nodes per depth level and 64 nodes on either side for responsiveness
- Node-type filtering, double-click navigation and graph viewport controls
- Automated CTest suite and Qt offscreen startup test on Windows and Linux

## Download / run

Download **RA2NodeStudio-v0.2.0-Windows-x64.zip** below. Unzip the complete folder, then run `RA2NodeStudio.exe`. Qt libraries and necessary plugins are bundled. No Qt SDK installation is necessary.

## Notes and limitations

- Always edit copies of mods until compatibility is verified.
- Section/category and reference recognition are heuristics, not a complete RA2/YR/Ares/Phobos cross-file semantic parser.
- ANSI decoding depends on your operating system locale; set the encoding before opening files.
- Node counts are limited for performance; raising maximum *depth* may not show every reachable node in a dense graph.
- Windows/Linux CI compile and headless tests run before release. Manual gameplay and interactive GUI testing are separate.
