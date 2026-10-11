#!/bin/sh
# Package entrypoint: executes deployed AppDir in place without AppImage/FUSE.
set -eu
exec /opt/ra2-node-studio/AppRun "$@"
