# Bee32 Library

Reusable C++ drivers and services for ESP32 projects using the Arduino framework. Bee32 provides
hardware abstraction and shared services; the consuming application owns product behavior,
hardware pin assignments, message protocols and presentation.

## Using the Library

Add this repository to a PlatformIO project's library directory (directly or as a Git submodule),
or use the Git dependency declared by [library.json](library.json):

```ini
lib_deps =
    https://github.com/beestarbush/Bee32Library.git
```

Use a C++17-capable compiler. Include the required module headers directly, for example
`<services/NTF/NTF.h>` or `<services/NET/NET.h>`; there is no umbrella header or automatic service
container. The consumer constructs the required objects, injects their dependencies and schedules
their `begin()`, `run()` and `end()` calls. Preserve dependency lifetimes through shutdown.

External component dependencies are not declared in the library manifest. The consuming project
must provide the libraries and board support needed by the enabled modules; inspect their source
includes and disable unused components. OTA, for example, needs `ayushsharma82/ElegantOTA` in the
consumer's `lib_deps`. Review external dependency licenses separately from the library metadata.

## Components

| Area | Modules and public interfaces |
|---|---|
| Hardware abstraction | [GPIO](src/hal/IO/IO.h), [buttons](src/hal/BTN/BTN.h), [PWM](src/hal/PWM/PWM.h), [UART](src/hal/UART/UART.h), [I2C](src/hal/I2C/I2C.h), [Wi-Fi](src/hal/WIFI/WIFI.h), [Bluetooth](src/hal/BLT/BLT.h) |
| Displays and sensors | [SSD1306](src/hal/SSD1306/SSD1306.h), [MAX72XX](src/hal/MAX72XX/MAX72XX.h), [RTC](src/hal/RTC/RTC.h), [SCD40](src/hal/SCD40/SCD40.h), [MQ3](src/hal/MQ3/MQ3.h) |
| Network and time | [NET policy](src/services/NET/NET.h), [NOW transport](src/services/NOW/NOW.h), [TIM synchronization](src/services/TIM/TIM.h), [OTA](src/services/OTA/OTA.h) |
| State and notifications | [NVS storage](src/services/NVS/NVS.h), [NTF queue](src/services/NTF/NTF.h), [version](src/services/VER/VER.h) |
| Application-facing services | [CLI](src/services/CLI/CLI.h), [HTTP](src/services/HTTP/HTTP.h), [telnet](src/services/TNT/TNT.h), [teletype](src/services/TTY/TTY.h), [GUI](src/services/GUI/GUI.h), [display](src/services/DSP/DSP.h), [LED](src/services/LED/LED.h), [sound](src/services/SND/SND.h), [Bluetooth keyboard](src/services/BKY/BKY.h) |

Modules can depend on board-specific Arduino/ESP32 facilities and third-party libraries. The
manifest's broad platform selector is not a claim that every driver runs on every platform.

## Network, Time and Updates

`SSL::NET::Service` owns Wi-Fi connection policy when used by an application:

- `OFFLINE` finishes pending work before disconnecting; it does not mean immediate disconnection.
- `ONLINE` keeps station Wi-Fi connected. `UPDATE` also enables OTA.
- Boot association is controlled by `CONFIG_SERVICE_NET_BOOT_CONNECT`. Configuration keys are
  `ssid`, `password` and the fallback/learned `channel`; valid channels are 1-13.
- `addConnectionRequirement(callback)` registers pending work. A true result keeps the connection
  required. Register during initialization; the callback's owner must outlive NET. The owner is
  responsible for completion and timeout rules. TIM is registered by NET automatically.
- `isConnected()` reports actual connectivity, independently of requested mode. Offline channel
  changes through `setChannel()` are rejected during connection attempts or pending work.

`SSL::TIM::Service::requestSync()` keeps a bounded request pending through Wi-Fi acquisition,
NTP attempts and retry delays. It allows three 10-second NTP attempts, 60 seconds between failed
attempts, and a 180-second total deadline including connection time. Success, exhausted attempts
or the deadline ends pending work. Calling `requestSync()` again while pending does not restart
the budget. The application must schedule both NET and TIM; NET is not a replacement for TIM's
`run()` call. Read the actual system clock with `time()`; `getEpochTime()` is not implemented as
a system-clock accessor.

`SSL::OTA::Service` wraps ElegantOTA on port 81 at `/update`. It serves only while enabled and
station-connected, not through an access-point-only connection. NET controls enablement when
the two services are used together. OTA has no authentication; any client able to reach it can
replace firmware. Use a trusted network and a board partition layout that supports OTA.

## ESP-NOW Transport

`SSL::NOW::Service` owns peers, unicast/broadcast and a bounded receive queue. Wi-Fi must be in
STA or AP+STA mode, and peers must share the current channel. `begin()` returns success/failure;
call `setPrimaryKey()` after successful initialization, then use `addEncryptedPeer()` with the
shared 16-byte local key for encrypted unicast. `addPeer()` does not enable encryption.
Broadcast remains unencrypted and must not carry secrets.

Register one `SSL::NOW::Listener` with `registerINow()`. Receive callbacks enqueue data;
`run()` delivers queued messages to the listener on the caller's task. Unregister the listener
before destroying it. The payload limit is 250 bytes and the receive queue holds eight messages.
Send success is not application-level acknowledgement: protocol versioning, retries, deduplication,
delivery confirmation and notification transport belong to the consuming application.

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

Minimal Arduino lifecycle example:

```cpp
#include <services/NTF/NTF.h>

SSL::NTF::Service notifications;

void setup()
{
  notifications.begin();
  notifications.activate(1U, SSL::NTF::Service::Severity::Info, "Ready");
}

void loop()
{
  notifications.run();
}
```

This maintains the queue only. A consumer must register a listener or inspect the current
selection to present notifications; the service itself does not render a UI or send radio messages.

## Compile-Time Configuration

Bee32 components can be enabled or disabled at compile time with build defines.

Defaults are defined in [Bee32Config.h](src/Bee32Config.h). HAL flags default to `1`; service
defaults can follow another flag or be opt-in. NTF and TIM have no dedicated feature flag.

### Configuration Example

In PlatformIO, add defines under `build_flags`:

```ini
build_flags =
  -std=gnu++17
    -DCONFIG_HAL_DRIVER_SSD1306=0
    -DCONFIG_SERVICE_GUI=0
```

Remove conflicting older language-standard flags with the consuming project's `build_unflags`
when necessary. Define each feature macro only once; do not rely on conflicting `-U`/`-D` order.

### Available Flags

#### HAL Drivers

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

#### Services

- `CONFIG_SERVICE_GUI`
  - Controls `src/services/GUI/*`
  - Default: `CONFIG_HAL_DRIVER_SSD1306`
- `CONFIG_SERVICE_HTTP`
  - Controls `src/services/HTTP/*`
  - Default: `CONFIG_HAL_DRIVER_WIFI`
- `CONFIG_SERVICE_NOW`
  - Controls `src/services/NOW/*`
  - Default: `CONFIG_HAL_DRIVER_WIFI`
- `CONFIG_SERVICE_OTA`
  - Controls `src/services/OTA/*`; requires ElegantOTA and Wi-Fi support
  - Default: `0` (opt-in)
- `CONFIG_SERVICE_NET`
  - Controls `src/services/NET/*`; requires WIFI, NVS, TIM and OTA services
  - Default: `CONFIG_SERVICE_OTA`
- `CONFIG_SERVICE_NET_BOOT_CONNECT`
  - Controls NET's boot-time station connection, not the availability of NET itself
  - Default: `1`; set `0` for an application that must stay offline at boot
- `CONFIG_SERVICE_BLUETOOTH_KEYBOARD`
  - Controls `src/services/BKY/*`
  - Default: `CONFIG_HAL_DRIVER_BLUETOOTH`

### Dependency Notes

- If a component is disabled, the related class declarations and implementation are not compiled.
- Service defaults follow their HAL dependencies where applicable.
- You can still override any default by defining the macro in your project build flags.
- Feature gates do not automatically resolve an inconsistent combination. In particular, NET
  uses OTA's interface, so enabling NET while disabling OTA is not a supported combination in
  the current implementation; network-related services also require the Wi-Fi driver.

## Verification

This repository contains no standalone PlatformIO project or test runner. Build enabled modules
through the consuming firmware project, and run that project's unit/integration suites against
the library sources. Host doubles can verify queue/policy logic but cannot prove physical I/O,
Wi-Fi/ESP-NOW radio delivery, NTP reachability or OTA installation.

Changes in a submodule checkout belong to this library's Git repository. Review and commit them
there first, then update the consuming project's pinned submodule revision separately.
