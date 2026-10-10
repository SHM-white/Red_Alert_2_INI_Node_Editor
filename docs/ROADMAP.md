# Follow-up roadmap

## Done in this branch

| Area | Result |
| --- | --- |
| Build | Qt 6 Widgets, CMake, cross-platform CI and CTest |
| INI | Indexed section model, ordered line edits, UTF-8/System modes, original-byte safe save |
| Editor | Section/key CRUD, value editing, editable raw source, undo/redo |
| Graph | Bounded local neighborhood, reference lookup index, zoom and pan |
| Themes | Four global themes, category-based colors and overrides |
| QA | Headless tests and planned deployment pipeline |

## Next — semantic engine

- [ ] Encode RA2/YR/Ares/Phobos reference key schemas (Primary/Secondary/Warhead/Projectile/Image/etc.), avoiding accidental edges from ordinary values.
- [ ] Multi-file includes, inheritance, duplicate definitions, missing target highlighting and cross-file navigation.
- [ ] Explicit GBK/GB18030 decoder on non-Chinese OS locales.
- [ ] Golden-file tests with real-world INIs (trailing comments, duplicate keys, whitespace, mixed line endings, special syntax).
- [ ] Review section-deletion behavior for comments located between sections.

## Next — performance and interface

- [ ] Asynchronous parse/index with cancellation for multi-megabyte mod packs.
- [ ] Diff-based undo instead of full document snapshots.
- [ ] Lazy-expand graph, minimap, manual node pinning and layout persistence.
- [ ] Schema-aware property editors (bool/color/number/reference) and completion.
- [ ] Per-section class overrides and contrast/accessibility audit.
- [ ] SVG/PNG graph export, multi-file mod workspace and better help.
- [ ] Benchmark startup, navigation and edit latency with large fixtures.
- [ ] Native Windows installer and official portable releases after CI passes.

## Production acceptance

1. Source fixtures round-trip without unintended changes.
2. Game-sensitive lists retain their order and comment placement.
3. Relationships have verified accuracy on a curated RA2/YR mod set.
4. Large file operations remain responsive with bounded memory and render cost.
5. Windows binaries build, deploy and launch successfully.

## Multi-level graph explorer

- [x] Independent persistent parent/child maximum depth controls (0–5, default 1)
- [x] Column layout for multiple depths; bounded breadth-first expansion with cycle deduplication
- [x] 14 nodes per depth level and 64 total per side to protect UI responsiveness
- [x] Unit and Qt offscreen UI smoke coverage for multi-level relationships
- [ ] Offer optional expansion of truncated columns, node-budget controls and a minimap for very large mod graphs
- [ ] Show non-tree / cycle links as optional secondary edges without visual clutter
