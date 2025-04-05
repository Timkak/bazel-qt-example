{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let 
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    base = pkgs.appimageTools.defaultFhsEnvArgs;
    fhs = pkgs.buildFHSEnv (base //  {
      name = "QT environment";  
      targetPkgs = pkgs: with pkgs; [
        bazel_7
        cmake
        gcc13
        llvmPackages_18.clang-tools
        llvmPackages_18.clang
	libxkbcommon
        libGL
        libGLU
        xorg.libX11
        xorg.libXext
        xorg.libXrender
        xorg.libXi
        xorg.libXrandr
        xorg.libXfixes
        xorg.libXcursor
        xorg.libxcb
        xorg.xcbutil
        xorg.xcbutilkeysyms
        xorg.xcbutilwm
        xorg.xcbutilimage
        # Multimedia and system dependencies
        alsa-lib
        libpulseaudio
        dbus
        fontconfig
        freetype
        openssl
        # Additional Qt dependencies
        vulkan-headers
        vulkan-loader
        wayland
        wayland-protocols
      ];
      extraBwrapArgs = [ "--bind /tmp /tmp" ];
      profile = ''
        export QT_QPA_PLATFORM_PLUGIN_PATH="${pkgs.qt6.qtbase.bin}/lib/qt-6/plugins/platforms"
        export QT_PLUGIN_PATH="${pkgs.qt6.qtbase.bin}/lib/qt-6/plugins"
        export QML2_IMPORT_PATH="${pkgs.qt6.qtbase.bin}/lib/qt-6/qml"
      '';
    });
  in {
    devShells.${system}.default = fhs.env;
  };
}

