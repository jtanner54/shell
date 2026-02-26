#!/bin/sh

set -e

# 1. Resolve the script directory once
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

(
  cd "$SCRIPT_DIR"
  
  # 2. Enforce VCPKG_ROOT existence. Fail loudly if missing.
  TOOLCHAIN="${VCPKG_ROOT:?VCPKG_ROOT environment variable is not set}/scripts/buildsystems/vcpkg.cmake"
  
  cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN"
  cmake --build ./build
)

# 3. Safely quote the execution path
exec "$SCRIPT_DIR/build/shell" "$@"