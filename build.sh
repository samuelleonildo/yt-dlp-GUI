#!/bin/bash
set -e

RELEASE_DIR="release"
BUILD_DIR="build"

# Detect the operating system
OS="$(uname -s)"
echo "Detected OS: $OS"

# Remove old executable if it exists
if [ -f "$RELEASE_DIR/yt-dlp-GUI" ] || [ -f "$RELEASE_DIR/yt-dlp-GUI.exe" ]; then
    echo "Removing old executable..."
    rm -rf "$RELEASE_DIR/yt-dlp-GUI" "$RELEASE_DIR/yt-dlp-GUI.exe"
fi

# Remove old build directory if it exists
if [ -d "$BUILD_DIR" ]; then
    echo "Removing old build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Generating Makefiles with CMake..."

# Determine number of threads for build
JOBS=$(nproc 2>/dev/null || echo 4)

# Set generator, Qt path and executable based on OS
if [[ "$OS" == "Linux" ]]; then
    GENERATOR="Unix Makefiles"
    QT_PATH="$HOME/Qt/6.9.2/gcc_64"  # Adjust according to your Linux Qt installation
    BUILD_CMD="make -j$JOBS"
    EXECUTABLE="./yt-dlp-GUI"
elif [[ "$OS" == "MINGW"* ]] || [[ "$OS" == "MSYS"* ]] || [[ "$OS" == "CYGWIN"* ]]; then
    GENERATOR="MinGW Makefiles"
    QT_PATH="/c/Qt/6.9.2/mingw_64"   # Adjust according to your Windows Qt installation (Git Bash path)
    BUILD_CMD="mingw32-make -j$JOBS"
    EXECUTABLE="./yt-dlp-GUI.exe"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

# Generate CMake build system
cmake .. -G "$GENERATOR" -DCMAKE_PREFIX_PATH="$QT_PATH"

# Build the project
echo "Building project..."
$BUILD_CMD
echo "Build completed successfully!"

# Copy executable to release directory
echo "Copying executable to release/"
mkdir -p "../$RELEASE_DIR"
cp "$EXECUTABLE" "../$RELEASE_DIR/"
echo "Copied successfully!"