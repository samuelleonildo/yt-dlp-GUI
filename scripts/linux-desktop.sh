#!/usr/bin/env bash
set -e

APP_NAME="yt-dlp-GUI"
EXEC_NAME="yt-dlp-GUI"
ICON_NAME="taskbar_icon"

# finds script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXEC_PATH="$SCRIPT_DIR/../$EXEC_NAME"

DESKTOP_FILE="$HOME/.local/share/applications/$APP_NAME.desktop"

echo "Creating .desktop at $DESKTOP_FILE"

mkdir -p "$HOME/.local/share/applications"

cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Type=Application
Name=yt-dlp-GUI
Comment=GUI for yt-dlp
Exec=$EXEC_PATH
Icon=$SCRIPT_DIR/$ICON_NAME.png
Terminal=false
Categories=Utility;
StartupWMClass=yt-dlp-GUI
EOF

chmod +x "$DESKTOP_FILE"

echo ".desktop created succesfully"

if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$HOME/.local/share/applications" || true
fi