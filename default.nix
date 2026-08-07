{
  lib,
  hyprland,
  hyprlandPlugins,
  gtest,
  withTests ? false,
}:
hyprlandPlugins.mkHyprlandPlugin {
  pluginName = "shadows-plus-plus";
  version = "0.1";
  src = ./.;

  inherit (hyprland) nativeBuildInputs;

  buildInputs = lib.optionals withTests [ gtest ];

  cmakeFlags = lib.optionals withTests [ "-DBUILD_TESTING=ON" ];

  doCheck = withTests;

  checkPhase = lib.optionalString withTests ''
    runHook preCheck
    ctest --output-on-failure
    runHook postCheck
  '';

  meta = with lib; {
    homepage = "https://github.com/ttoino/shadows-plus-plus";
    description = "Hyprland shadows-plus-plus plugin";
    license = licenses.gpl3;
    platforms = platforms.linux;
  };
}
