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
cd build/macos-debug && ./hewnstead
```

### Convenience scripts

For a faster dev loop, the `scripts/` directory provides one-liner wrappers:

```bash
./scripts/build.sh      # configure + build
./scripts/run.sh        # configure + build + run
./scripts/clean.sh      # remove build directory
```

Optional shell aliases (add to `~/.zshrc` or `~/.bashrc`):

```bash
alias hsbuild='[path-to-hewnstead]/scripts/build.sh'
alias hsrun='[path-to-hewnstead]/scripts/run.sh'
alias hsclean='[path-to-hewnstead]/scripts/clean.sh'
```

After sourcing, `hsbuild` / `hsrun` / `hsclean` work from any directory.

## License

Hewnstead is licensed under the [MIT License](LICENSE).
Bundled third-party libraries retain their respective licenses; see [`THIRD_PARTY_LICENSES.md`](THIRD_PARTY_LICENSES.md).