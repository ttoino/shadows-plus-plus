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
      });

      overlays = {
        default = self.overlays.shadows-plus-plus;

        shadows-plus-plus = final: prev: {
          hyprlandPlugins = (prev.hyprlandPlugins or { }) // {
            shadows-plus-plus = final.callPackage ./. { };
          };
        };
      };

      checks = eachSystem (system: self.packages.${system});

      devShells = eachSystem (
        system: with pkgsFor.${system}; {
          default = mkShell.override { stdenv = gcc14Stdenv; } {
            name = "shadows-plus-plus";
            buildInputs = [ hyprland.packages.${system}.hyprland ];
            inputsFrom = [ hyprland.packages.${system}.hyprland ];
          };
        }
      );
    };
}
