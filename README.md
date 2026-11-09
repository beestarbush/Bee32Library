# Bee32 Library Feature Configuration

## Notification Service

`SSL::NTF::Service` (`services/NTF/NTF.h`) is a volatile, UI-independent queue.
Own it in the service container and inject it by reference into producers and consumers.
`activate(id, severity, text)` copies up to 63 text bytes; an already-active ID is not added
again. Producers call `deactivate(id)` when a condition clears. Reactivation appends a new item.

Severities are `High`, `Medium`, `Low`, and `Info`. Each activation receives three seconds
of presentation, in FIFO order. Afterwards the oldest item at the highest active severity
stays selected, without rotation. Deactivation discards queued or selected items immediately.
Only info automatically expires, after three seconds on screen. `begin()`/`end()` clear state.

`registerNotificationListener(callback)` returns an ID for
`unregisterNotificationListener(id)`. Multiple listeners receive `Activated`, `Deactivated`,
and `SelectionChanged` events and can ignore unwanted types. Event data is copied;
`hasNotification == false` denotes an empty selection. Register before processing the events
you need and unregister before destroying the consumer. Historical events are not replayed.

Call `run()` on the application loop to advance presentation and dispatch callbacks.
`run(false)` dispatches events without starting presentation timers, for example for an
outbox awaiting remote delivery or a hidden boot screen. Callbacks can register/unregister
listeners and activate/deactivate notifications; newly generated events wait until the next
`run()`. Do not retain `getAt()`/`getCurrent()` pointers across queue mutations.

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
