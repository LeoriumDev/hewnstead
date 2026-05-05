#!/bin/bash
set -e

cd "$(dirname "$0")/.."

cmake --preset macos-debug
cmake --build --preset macos-debug

echo "=> Build complete. Run: cd build/macos-debug && ./hewnstead"