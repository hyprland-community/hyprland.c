{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
    in {
      devShells.default = pkgs.mkShell.override {stdenv = pkgs.clangStdenv;} {
        name = "hyprland-c-shell";
        nativeBuildInputs = with pkgs; [
          cmake
          json_c

          clang-tools_15
          bear
          lldb
        ];
      };
    });
}
