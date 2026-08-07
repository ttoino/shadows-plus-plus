{
  inputs = {
    hyprland.url = "github:hyprwm/Hyprland";
    nixpkgs.follows = "hyprland/nixpkgs";
    systems.follows = "hyprland/systems";
  };

  outputs =
    {
      self,
      hyprland,
      nixpkgs,
      systems,
      ...
    }:
    let
      inherit (nixpkgs) lib;
      eachSystem = lib.genAttrs (import systems);

      pkgsFor = eachSystem (
        system:
        import nixpkgs {
          localSystem.system = system;
          overlays = [
            self.overlays.shadows-plus-plus
            hyprland.overlays.hyprland-packages
          ];
        }
      );
    in
    {
      packages = eachSystem (system: {
        inherit (pkgsFor.${system}.hyprlandPlugins)
          shadows-plus-plus
          ;

        shadows-plus-plus-tests = pkgsFor.${system}.hyprlandPlugins.shadows-plus-plus.override {
          withTests = true;
        };
      });

      overlays = {
        default = self.overlays.shadows-plus-plus;

        shadows-plus-plus = final: prev: {
          hyprlandPlugins = (prev.hyprlandPlugins or { }) // {
            shadows-plus-plus = final.callPackage ./. { };
          };
        };
      };

      checks = eachSystem (system:
        self.packages.${system}
        // import ./nix/tests { inherit self hyprland; } pkgsFor.${system}
      );

      devShells = eachSystem (
        system: with pkgsFor.${system}; {
          default = mkShell.override { stdenv = gcc16Stdenv; } {
            name = "shadows-plus-plus";
            buildInputs = [ hyprland.packages.${system}.hyprland-debug ];
            inputsFrom = [ hyprland.packages.${system}.hyprland-debug ];
          };
        }
      );
    };
}
