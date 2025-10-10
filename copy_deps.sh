#!/bin/bash
set -e


# paths
RELEASE_DIR="./release"
DEPS_DIR="$RELEASE_DIR/deps"
EXECUTABLE="./build/yt-dlp-GUI"

YT_DLP_PATH="$(which yt-dlp)"
FFMPEG_PATH="$(which ffmpeg)" 


# Detect OS
OS="$(uname -s)"
echo "Detected OS: $OS"


# Remove old release directory
if [ -d "$RELEASE_DIR" ]; then
    echo "Removing old release directory..."
    rm -rf "$RELEASE_DIR"
fi
mkdir -p "$DEPS_DIR"


# Copy main executable to release
if [[ "$OS" == "Linux" ]]; then
    EXECUTABLE_PATH="$EXECUTABLE"
elif [[ "$OS" == "MINGW"* ]] || [[ "$OS" == "MSYS"* ]] || [[ "$OS" == "CYGWIN"* ]]; then
    EXECUTABLE_PATH="$EXECUTABLE.exe"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

echo "Copying main executable to release..."
cp "$EXECUTABLE_PATH" "$RELEASE_DIR/"


# Copy yt-dlp and ffmpeg
YT_DLP_PATH="$(which yt-dlp)"
FFMPEG_PATH="$(which ffmpeg)"

if [ -z "$YT_DLP_PATH" ] || [ -z "$FFMPEG_PATH" ]; then
    echo "yt-dlp or ffmpeg not found in PATH. Please install them."
    exit 1
fi


# Copy project dependencies
if [[ "$OS" == "Linux" ]]; then
    echo "Copying Linux shared libraries..."
    ldd "$EXECUTABLE_PATH" | grep "=> /" | awk '{print $3}' | while read lib; do
        echo "Copying $lib..."
        cp --preserve=links "$lib" "$RELEASE_DIR/"
    done
    echo "All Linux dependencies copied to $RELEASE_DIR."

elif [[ "$OS" == "MINGW"* ]] || [[ "$OS" == "MSYS"* ]] || [[ "$OS" == "CYGWIN"* ]]; then
    echo "Deploying Qt dependencies using windeployqt..."
    
    # Detect Qt automatically (typical installation path)
    if [ -z "$QT_PATH" ]; then
        QT_PATH="/c/Qt/6.9.2/mingw_64"  # Adjust if needed
    fi
    WINDEPLOYQT="$QT_PATH/bin/windeployqt.exe"
    
    if [ ! -f "$WINDEPLOYQT" ]; then
        echo "windeployqt.exe not found at $WINDEPLOYQT"
        exit 1
    fi

    "$WINDEPLOYQT" "$RELEASE_DIR/$(basename "$EXECUTABLE_PATH")"
    echo "Qt dependencies deployed."
fi
echo "Copying yt-dlp and ffmpeg to ./release/deps/"
cp "$YT_DLP_PATH" "$DEPS_DIR"
cp "$FFMPEG_PATH" "$DEPS_DIR"
echo "yt-dlp and ffmpeg Copied to ./release/deps/"

echo "Release folder is ready!"