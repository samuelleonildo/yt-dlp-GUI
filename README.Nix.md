# Nix

## Usage

```bash
# Run without installing
nix run github:samuelleonildo/yt-dlp-GUI

# Install to your profile
nix profile install github:samuelleonildo/yt-dlp-GUI
```

## NixOS / Home Manager

```nix
inputs.yt-dlp-gui.url = "github:samuelleonildo/yt-dlp-GUI";
```

```nix
home.packages = [ inputs.yt-dlp-gui.packages.${pkgs.system}.default ];
```
