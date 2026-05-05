#!/bin/bash
set -e

# cd to project root
cd "$(dirname "$0")/.."

echo "=> Configuring..."
cmake --preset macos-debug

echo "=> Building..."
cmake --build --preset macos-debug

echo "=> Running..."
cd build/macos-debug
./hewnstead