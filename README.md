# Shadows++

Inspired by [borders-plus-plus](https://github.com/hyprwm/hyprland-plugins/blob/main/borders-plus-plus), allows you to add additional shadows to your windows, using CSS syntax.

Example config:

```hyprlang
plugin:shadows-plus-plus {
    add_shadows = 2 # 0 - 10

    shadow_1 {
        offset = 0 4
        blur_radius = 4
        spread_radius = 0
        color = rgba(0,0,0,.30)
    }

    shadow_2 {
        offset = 0 8
        blur_radius = 12
        spread_radius = 6
        color = rgba(0,0,0,.15)
    }

    # Also supports :scale and :ignore_window like the default shadows
}
```

For the best results, I recommend disabling the default Hyprland shadows:

```hyprlang
decoration:shadow:enabled = false
```

Due to a limitation in the underlying Hyprland code, the shadow render power should be set in the default location and will be global. To match way the web renders shadows it should be set to 1.

```hyprlang
decoration:shadow:render_power = 1
```

To reproduce the default shadow algorithm, set `blur_radius` and `spread_radius` to `range` divided by 2:

```hyprlang
# These are equivalent
decoration:shadow:range = 4

plugin:shadows-plus-plus:shadow_1 {
    blur_radius = 2
    spread_radius = 2
}
```
