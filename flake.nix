{
  description = "Dev enviroment for trimware sw";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            pkgs.pkgsCross.avr.buildPackages.gcc
            avrdude
            ruby
            cmake
          ];

        #   shellHook = '''';
        };
      });
}