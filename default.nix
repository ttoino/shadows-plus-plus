{
  lib,
  hyprland,
  hyprlandPlugins,
}:
hyprlandPlugins.mkHyprlandPlugin hyprland {
  pluginName = "shadows-plus-plus";
  version = "0.1";
  src = ./.;

  inherit (hyprland) nativeBuildInputs;

  meta = with lib; {
    homepage = "https://github.com/ttoino/shadows-plus-plus";
    description = "Hyprland shadows-plus-plus plugin";
    license = licenses.gpl3;
    platforms = platforms.linux;
  };
}
