# shadows-plus-plus — Agent Notes

C++23 Hyprland plugin that adds CSS-like box shadows to windows via window decorations and a custom render pass element.

## Build

- `make all` — produces `shadows-plus-plus.so`
- `make test` — builds and runs the GTest suite (requires `gtest` and `hyprutils` pkg-config)
- CI / verification: `nix flake check --all-systems --keep-going --print-build-logs`
- C++23 (`-std=c++2b`)
- Makefile adds `--no-gnu-unique` only when `CXX=g++`; clang does not get it

## Format

- `clang-format -Werror -i --dry-run *.[ch]pp`
- **No `.clang-format` config exists** — relies on system / default style

## Dev Environment

- `nix develop` — shell uses `gcc16Stdenv`, pulls `hyprland-debug` inputs
- Non-Nix: needs `pkg-config` packages `pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon gtest hyprutils`

## Architecture

- `main.cpp` — `PLUGIN_INIT`, `PLUGIN_EXIT`, `PLUGIN_API_VERSION`; registers up to 10 shadow config layers via `Config::Values::CIntValue`/`CGradientValue`/`CVec2Value`/`CBoolValue`/`CFloatValue` (Config V2 API), hooks window open events, and registers Lua helpers
- `CBoxShadowsDecoration.{hpp,cpp}` — `IHyprWindowDecoration` implementation; renders shadows via `g_pHyprRenderer->drawShadow` or `CRectPassElement` for sharp shadows
- `CBoxShadowsPassElement.{hpp,cpp}` — `IPassElement` for the custom render pass
- `shadowsLua.{hpp,cpp}` — Lua helper implementations for `hl.plugin.shadows.*`; also exposes `hl.plugin.shadows._test_*` helpers
- `geometry.{hpp,cpp}` — isolated geometry helpers (currently shadow clipping strips) shared with unit tests
- `globals.hpp` — shared `PHANDLE`, typed `SVars` config value struct, and decoration registry
- `tests/` — GTest unit tests
- `nix/tests/default.nix` — NixOS VM integration test

## Critical Constraints

- **Exact Hyprland version match**: `PLUGIN_INIT` compares `__hyprland_api_get_hash()` to `__hyprland_api_get_client_hash()` and throws if they differ. Plugin must be built against the exact Hyprland version it runs on.
- **Currently pinned to Hyprland 0.56.0+** (`88c6386994c960006e14c7bfa4ccfee873252882` via flake.lock).
