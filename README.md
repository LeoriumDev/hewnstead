# hewnstead

A medieval voxel sandbox where you build, settle, and outlast the seasons.

## Building

> **Note**: Currently only tested on macOS (Apple Silicon, Tahoe). Linux and Windows support planned but unverified.

Requires:
- CMake ≥ 3.28
- A C++20 compiler (Apple Clang 14+, Clang 13+, GCC 11+, or MSVC 19.30+)
- Ninja

```bash
git clone https://github.com/LeoriumDev/hewnstead.git
cd hewnstead
cmake --preset macos-debug
cmake --build --preset macos-debug
./build/macos-debug/hewnstead
```

## License

Hewnstead is licensed under the [MIT License](LICENSE).
Bundled third-party libraries retain their respective licenses; see [`THIRD_PARTY_LICENSES.md`](THIRD_PARTY_LICENSES.md).
