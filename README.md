# Bee32 Library Feature Configuration

Bee32 components can be enabled or disabled at compile time with build defines.

The defaults are defined in `src/Bee32Config.h` and are set to `1` (enabled) unless noted otherwise.

## How to configure

In PlatformIO, add defines under `build_flags`:

```ini
build_flags =
    -DCONFIG_HAL_DRIVER_SSD1306=0
    -DCONFIG_SERVICE_GUI=0
```

## Available flags

### HAL drivers

- `CONFIG_HAL_DRIVER_BLUETOOTH`
  - Controls `src/hal/BLT/*`
  - Default: `1`
- `CONFIG_HAL_DRIVER_WIFI`
  - Controls `src/hal/WIFI/*`
  - Default: `1`
- `CONFIG_HAL_DRIVER_SSD1306`
  - Controls `src/hal/SSD1306/*`
  - Default: `1`
- `CONFIG_HAL_DRIVER_DS3231`
  - Controls `src/hal/RTC/*`
  - Default: `1`
- `CONFIG_HAL_DRIVER_SCD40`
  - Controls `src/hal/SCD40/*`
  - Default: `1`
- `CONFIG_HAL_DRIVER_MAX72XX`
  - Controls `src/hal/MAX72XX/*`
  - Default: `1`

### Services

- `CONFIG_SERVICE_GUI`
  - Controls `src/services/GUI/*`
  - Default: `CONFIG_HAL_DRIVER_SSD1306`
- `CONFIG_SERVICE_HTTP`
  - Controls `src/services/HTTP/*`
  - Default: `CONFIG_HAL_DRIVER_WIFI`
- `CONFIG_SERVICE_BLUETOOTH_KEYBOARD`
  - Controls `src/services/BKY/*`
  - Default: `CONFIG_HAL_DRIVER_BLUETOOTH`

## Notes

- If a component is disabled, the related class declarations and implementation are not compiled.
- Service defaults follow their HAL dependencies where applicable.
- You can still override any default by defining the macro in your project build flags.
