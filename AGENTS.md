# shadows-plus-plus — Agent Notes

C++23 Hyprland plugin that adds CSS-like box shadows to windows via window decorations and a custom render pass element.

## Build

- `make all` — produces `shadows-plus-plus.so`
- CI / verification: `nix flake check --all-systems --keep-going --print-build-logs`
- C++23 (`-std=c++2b`)
- Makefile adds `--no-gnu-unique` only when `CXX=g++`; clang does not get it

## Format

- `clang-format -Werror -i --dry-run *.[ch]pp`
- **No `.clang-format` config exists** — relies on system / default style

## Dev Environment

- `nix develop` — shell uses `gcc14Stdenv`, pulls `hyprland-debug` inputs
- Non-Nix: needs `pkg-config` packages `pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon`

## Architecture

- `main.cpp` — `PLUGIN_INIT`, `PLUGIN_EXIT`, `PLUGIN_API_VERSION`; registers up to 10 shadow config layers via `Config::Values::CIntValue`/`CColorValue`/`CVec2Value`/`CBoolValue`/`CFloatValue` (Config V2 API) and hooks window open events
- `CBoxShadowsDecoration.{hpp,cpp}` — `IHyprWindowDecoration` implementation
- `CBoxShadowsPassElement.{hpp,cpp}` — `IPassElement` for the custom render pass
- `globals.hpp` — shared `PHANDLE` and typed `SVars` config value struct

## Critical Constraints

- **Exact Hyprland version match**: `PLUGIN_INIT` compares `__hyprland_api_get_hash()` to `__hyprland_api_get_client_hash()` and throws if they differ. Plugin must be built against the exact Hyprland version it runs on.
- **Currently pinned to Hyprland 0.55** (`01ab0474b4ae92db9c25dc0eef6515b99544698f` via flake.lock).
- No test suite exists.
