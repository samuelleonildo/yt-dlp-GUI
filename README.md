# yt-dlp-GUI

A lightweight, open-source C++ **graphical interface** for [yt-dlp](https://github.com/yt-dlp/yt-dlp) — making it easier to download and manage online videos without using command line.

## Features

- Clean and simple interface for [yt-dlp](https://github.com/yt-dlp/yt-dlp)
- Run and monitor downloads visually — no terminal needed
- Fully open-source and public domain (The Unlicense)
- Cross-platform: works on Linux and Windows
- Portable — no installation required

## Getting Started

### Option 1 — Download a release
Download the latest prebuilt version from the [Releases page](https://github.com/samuelleonildo/yt-dlp-GUI/releases/)
<br>
The .zip or .tar.gz file contains everything you need — just extract and run **yt-dlp-GUI**.

### Option 2 — Build from source:
### Prerequisites

- Linux or Windows
- Build tools: `g++`, `CMake`
- Dependencies:
    - [yt-dlp](https://github.com/yt-dlp/yt-dlp)
    - ffmpeg
    - Qt

### Build

1. Clone the repository
    ```bash
    git clone https://github.com/samuelleonildo/yt-dlp-GUI.git
    cd yt-dlp-GUI
    ```
2. Run the build script:

    Using `build.sh`
    ```sh
    ./build.sh
    ```
    > This script creates the `release/` directory and copies the executable into it
    <br>
    > _Note: The application was programmed to use the yt-dlp and ffmpeg that are in the `release/deps/` directory_

3. Run `copy_deps.sh` to handle required library dependencies on some platforms:
    ```sh
    ./copy_deps.sh
    ```
    > This will copy all Qt dependencies to the `release/` directory
    <br>
    > _Note: this script handles Scoop-installed deps on Windows_

4. The executable will be located in the `release/` directory — run it directly
    ```bash
    ./release/yt-dlp-GUI
    ```

## Notes
- If you encounter warnings or download issues, try updating yt-dlp
    > Make sure you're in the `release/` directory

    ```bash
    ./yt-dlp -U # Linux
    .\yt-dlp.exe -U # Windows
    ```

## Project Structure

- `src/` — Contains all source code for the application.
- `CMakeLists.txt` — CMake build configuration.
- `build.sh` — Shell script to automate the project build.
- `copy_deps.sh` — Script to handle dependency copy during build process.

## License

Licensed under [The Unlicense](LICENSE) — free and public domain.

## Author
samuelleonildo
<br>
[github.com/samuelleonildo](https://github.com/samuelleonildo/)