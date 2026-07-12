{ lib, stdenv, fetchFromGitHub, cmake, qt6, yt-dlp, ffmpeg, makeDesktopItem, copyDesktopItems }:

# replicate ./scripts/linux-desktop.sh for nix
let
  desktopItem = makeDesktopItem {
    name = "yt-dlp-gui";
    desktopName = "yt-dlp-GUI";
    comment = "Gui for yt-dlp";
    exec = "yt-dlp-gui";
    icon = "yt-dlp-gui";
    categories = [ "Utility" ];
    startupWMClass = "yt-dlp-GUI";
  };
in
stdenv.mkDerivation (finalAttrs: {
  pname = "yt-dlp-gui";
  version = "2026-03-02";

  src = fetchFromGitHub {
    owner = "samuelleonildo";
    repo = "yt-dlp-GUI";
    rev = "be310e5f0c6979c80b0dd748426d9995c7be55ee";
    hash = "sha256-RgnxK+KBXGcrpzzmpDMBqkM8mh4WSnTlxu8WqtpMUeQ=";
  };

  nativeBuildInputs = [
    cmake
    qt6.wrapQtAppsHook
    copyDesktopItems
  ];

  buildInputs = [
    qt6.qtbase
  ];

  desktopItems = [ desktopItem ];

  installPhase = ''
    runHook preInstall
    install -Dm755 yt-dlp-GUI $out/bin/yt-dlp-gui

    # necessary for hardcoded ./deps/ path check on main_window.cpp:447
    mkdir -p $out/bin/deps
    ln -s ${lib.getExe yt-dlp} $out/bin/deps/yt-dlp
    ln -s ${lib.getExe' ffmpeg "ffmpeg"} $out/bin/deps/ffmpeg

    install -Dm644 ../src/resources/icons/taskbar_icon.png \
    $out/share/icons/hicolor/256x256/apps/yt-dlp-gui.png

    runHook postInstall
  '';

  meta = {
    description = "Qt/C++ interface for yt-dlp";
    homepage = "https://github.com/samuelleonildo/yt-dlp-GUI";
    license = lib.licenses.unlicense;
    platforms = lib.platforms.linux;
    mainProgram = "yt-dlp-gui";
  };
})