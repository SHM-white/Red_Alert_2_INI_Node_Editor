# Cross-platform binary distribution

The application uses Qt 6 Widgets and CMake with no Windows-only GUI dependencies. GitHub Actions builds and tests the same **release tag** separately for Windows x64, Linux x86_64, macOS arm64 and macOS Intel.

## Artifacts

| Asset suffix | How to run |
| --- | --- |
| `Windows-x64.zip` | Extract all files, run `RA2NodeStudio.exe` |
| `Linux-x86_64.AppImage` | Portable image; requires FUSE unless launched with `APPIMAGE_EXTRACT_AND_RUN=1` |
| `ra2-node-studio_X.Y.Z_amd64.deb` | Debian/Ubuntu x86_64: install with `sudo apt install ./ra2-node-studio_*.deb`, then `ra2-node-studio` |
| `Linux-x86_64-portable.tar.gz` | No FUSE or root required: extract and run `./AppRun` |
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

## AppImage FUSE errors, including WSL

Some Debian/WSL environments lack `fusermount`; an AppImage needs it to mount its SquashFS payload. This is not a crash in the editor. Without installing FUSE, try:

```bash
chmod +x ./RA2NodeStudio-v0.2.0-Linux-x86_64.AppImage
APPIMAGE_EXTRACT_AND_RUN=1 ./RA2NodeStudio-v0.2.0-Linux-x86_64.AppImage
```

Alternatively, **install the Debian package**. It runs the deployed Qt application directly from `/opt/ra2-node-studio` and **does not use AppImage or FUSE**:

```bash
sudo apt install ./ra2-node-studio_0.2.0_amd64.deb
ra2-node-studio
sudo apt remove ra2-node-studio
```

Or extract the FUSE-free `Linux-x86_64-portable.tar.gz` into a directory and run `./AppRun`. This is a separate executable directory, not the compressed AppImage. Both alternatives bundle Qt libraries and plugins.

On WSL the editor additionally requires WSLg or an X11 display server; check `DISPLAY` or `WAYLAND_DISPLAY`. Installing a DEB removes the FUSE mount requirement, but does not create a graphics server. The DEB targets Debian/Ubuntu **amd64**, compiled against Ubuntu 22.04 / glibc 2.35, and still depends on core X11/OpenGL system libraries.

## Packaging verification

The Linux pipeline constructs the DEB and portable tarball from the **same Qt-deployed AppDir** as the AppImage. It checks DEB metadata and contents, installs the package using apt on a clean Ubuntu runner, and executes `QT_QPA_PLATFORM=offscreen ra2-node-studio --smoke-test` through its installed launcher. Packaging files live under `packaging/linux/`.

Pull requests run the same compilation and packaging jobs but **cannot modify the Release**. On merge, the existing release workflow can attach the new Linux assets to the existing `v0.2.0` release. All build targets must pass before publishing.
