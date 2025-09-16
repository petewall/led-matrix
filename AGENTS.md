# Repository Guidelines

## Project Structure & Module Organization
- `src/`: firmware entrypoint (`main.cpp`).
- `include/`: shared headers and hardware config (`hardware.h`).
- `lib/<Module>/`: self‑contained modules (e.g., `LedMatrix`, `Display`, `Off`, `WebServer`).
- `test/`: native unit tests (GoogleTest via PlatformIO).
- `platformio.ini`: environments (`led_matrix`, `testing`) and flags.
- `Makefile`: convenience targets; `.pio/` contains build artifacts.

## Build, Test, and Development Commands
- `make build`: compile `env:led_matrix` → `.pio/build/led_matrix/firmware.bin`.
- `make upload`: flash firmware to the Wemos D1 Mini (`board = d1_mini`).
- `make check`: static analysis via `pio check --skip-packages`.
- `make test`: run native tests in `env:testing` with verbose output.
- `pio device monitor -b 115200`: open the serial monitor.

## Coding Style & Naming Conventions
- Language: C++14 with `-Wall -Werror`; keep builds warning‑free.
- Indentation: 2 spaces, no tabs. Keep lines focused and readable.
- Files: class files match names (e.g., `LedMatrix.h/.cpp`).
- Naming: Classes `PascalCase`; functions/methods `lowerCamelCase`;
  constants/macros `UPPER_SNAKE_CASE`.
- Organization: public headers in `include/`; module code in `lib/<Module>/`.

## Testing Guidelines
- Framework: GoogleTest on `env:testing` (native platform).
- Location: add `.cpp` tests under `test/` (e.g., `test_display.cpp`).
- Scope: favor hardware‑free logic; mock or separate hardware access.
- Run: `make test` (equivalent to `pio test --environment testing`).

## Commit & Pull Request Guidelines
- Commits: imperative mood and concise subject (e.g., "Add clock render step"); group related changes.
- PRs: include a clear description, linked issues, and evidence of testing
  (serial logs, photos/video of the matrix, or screenshots). Describe
  hardware used and steps to reproduce. Note any user‑visible changes.

## Security & Configuration Tips
- Avoid committing secrets. Move Wi‑Fi creds from `include/hardware.h` to a local
  `include/secrets.h` (git‑ignored) and `#include "secrets.h"` from code, or pass via
  `platformio.ini` `build_flags` defines (e.g., `-D WIFI_SSID=...`).
- Verify `monitor_speed` and board/port before uploading.
