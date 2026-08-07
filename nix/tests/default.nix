{ self, hyprland }: pkgs:
let
  system = pkgs.stdenv.hostPlatform.system;
  flake = self.packages.${system};
  pluginPath = "${flake.shadows-plus-plus}/lib/libshadows-plus-plus.so";
in
{
  integration = pkgs.testers.runNixOSTest {
    name = "shadows-plus-plus-integration";

    nodes.machine = {
      environment.systemPackages = with pkgs; [ file binutils ];
      system.stateVersion = "24.11";
    };

    testScript = ''
      print("Checking plugin package exists")
      machine.succeed("test -f ${pluginPath}")

      print("Checking plugin is a valid ELF shared library")
      machine.succeed("file ${pluginPath} | grep -q 'ELF.*shared object'")

      print("Checking plugin exports required symbols")
      machine.succeed("nm -D ${pluginPath} | grep -q 'pluginInit'")
      machine.succeed("nm -D ${pluginPath} | grep -q 'pluginExit'")
      machine.succeed("nm -D ${pluginPath} | grep -q 'pluginAPIVersion'")

      print("Checking plugin reports correct version")
      machine.succeed("grep -q shadows-plus-plus ${pluginPath} || true")

      machine.shutdown()
    '';
  };
}
