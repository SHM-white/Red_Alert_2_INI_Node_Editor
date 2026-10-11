#!/usr/bin/env bash
# Make a native DEB and FUSE-free tarball from the Qt-deployed AppDir.
set -euo pipefail
if (( $# != 4 )); then
  echo "Usage: $0 <AppDir> <vX.Y.Z> <output-dir> <packaging-dir>" >&2
  exit 2
fi
appdir=$(realpath "$1")
tag=$2
dist=$(realpath -m "$3")
metadata=$(realpath "$4")
version=$(printf '%s' "$tag" | sed 's/^v//')
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+([.+~][0-9A-Za-z.+~:-]+)?$ ]]; then
  echo "Invalid Debian version from release tag: $tag" >&2; exit 2
fi
if [[ ! -x "$appdir/AppRun" || ! -x "$appdir/usr/bin/RA2NodeStudio" ]]; then
  echo "AppDir lacks AppRun or the main executable" >&2; exit 1
fi
if ! find "$appdir/usr" -name 'libQt6Core.so*' -print -quit | grep -q .; then
  echo "AppDir does not contain the deployed Qt runtime" >&2; exit 1
fi
mkdir -p "$dist"
tar -C "$appdir" -czf "$dist/RA2NodeStudio-$tag-Linux-x86_64-portable.tar.gz" .

workdir=$(mktemp -d)
trap 'rm -rf "$workdir"' EXIT
root="$workdir/deb"
install -d -m 0755 "$root/DEBIAN" "$root/opt/ra2-node-studio" \
  "$root/usr/bin" "$root/usr/share/applications" \
  "$root/usr/share/icons/hicolor/scalable/apps"
cp -a "$appdir/." "$root/opt/ra2-node-studio/"
install -m 0755 "$metadata/linux/ra2-node-studio-launcher.sh" "$root/usr/bin/ra2-node-studio"
install -m 0644 "$metadata/ra2-node-studio.svg" "$root/usr/share/icons/hicolor/scalable/apps/ra2-node-studio.svg"
install -m 0644 "$metadata/linux/ra2-node-studio.deb.desktop" "$root/usr/share/applications/ra2-node-studio.desktop"
size=$(du -sk "$root/opt/ra2-node-studio" | cut -f1)
cat > "$root/DEBIAN/control" <<EOF
Package: ra2-node-studio
Version: $version
Section: editors
Priority: optional
Architecture: amd64
Maintainer: RA2 Node Studio Contributors <noreply@github.com>
Homepage: https://github.com/SHM-white/Red_Alert_2_INI_Node_Editor
Installed-Size: $size
Depends: libc6 (>= 2.35), libstdc++6, libgcc-s1, libgl1, libglx0, libopengl0, libegl1, libx11-6, libx11-xcb1, libxcb1, libxkbcommon0, libfontconfig1, libfreetype6, libdbus-1-3, libgpg-error0, zlib1g
Description: Visual INI node editor for Red Alert 2 mods
 Qt 6 graphical editor with indexed node dependencies and themes.
 Qt shared libraries and plugins are included under /opt/ra2-node-studio.
 No AppImage mount or FUSE runtime is required.
EOF
dpkg-deb --build --root-owner-group "$root" "$dist/ra2-node-studio_"$version"_amd64.deb"
echo "Created Debian installer and FUSE-free archive:"
ls -lh "$dist/ra2-node-studio_"$version"_amd64.deb" "$dist/RA2NodeStudio-$tag-Linux-x86_64-portable.tar.gz"
