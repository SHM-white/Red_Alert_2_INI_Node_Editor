# Cross-platform binary distribution

The application uses Qt 6 Widgets and CMake with no Windows-only GUI dependencies. GitHub Actions builds and tests the same **release tag** separately for Windows x64, Linux x86_64, macOS arm64 and macOS Intel.

## Artifacts

| Asset suffix | How to run |
| --- | --- |
| `Windows-x64.zip` | Extract all files, run `RA2NodeStudio.exe` |
| `Linux-x86_64.AppImage` | `chmod +x *.AppImage && ./RA2NodeStudio-*.AppImage` |
| `macOS-arm64.zip` | Extract `.app` on an Apple Silicon Mac; open from Finder |
| `macOS-x86_64.zip` | Extract `.app` on an Intel Mac; open from Finder |

Each archive bundles Qt libraries and platform plugins. Linux AppImages still depend on a compatible kernel, glibc and common graphics/system libraries; this is not a claim of compatibility with every Linux distribution. macOS packages are **ad hoc signed and not notarized**; Gatekeeper may require the user to explicitly allow them in System Settings > Privacy & Security.

## Release automation

- Pushing a `v*` tag triggers all four builds and publishes to the matching release.
- Changes to `.github/workflows/release.yml` on `main` trigger a rebuild of the existing `v0.2.0` tag to backfill the current Release, keeping source and binary versions aligned.
- Manual workflow dispatch accepts an existing or new tag.
- Each OS runs the project's CTest suite before deployment. The publish job only runs after **every** matrix job succeeds.
- This workflow attaches assets to an existing GitHub Release; it retains the release URL and does not change the tag target.

## Scope

Linux ARM64 and Windows ARM64 are not currently built. The macOS bundles need real-hardware smoke-testing on both architectures before they can be considered fully user-verified.
