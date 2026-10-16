# libsns-sc1035-universal

Universal SC1035 sensor library (HiSilicon hi3518ev100, MPP V1.0.B.0).
One `.so` for 720p and 960p, any fps 5..30 — geometry and fps are resolved
at runtime, no per-mode builds.

## How it works

- **Window**: read from VI introspection (`HI_MPI_VI_GetChnAttr(0)`),
  the official MPI — `Isp_H <= 720` selects the 720-crop window
  (YOUT 0x2d0 / VOFFS 0x80), anything above selects the full 960 window.
  Priority: `sc1035_set_mode()` API > VI introspection > env
  `SC1035_ISP_H` / `SC1035_FPS` > default 960@30.
- **fps**: `VTS = round(30000 / fps)`, clamped to 1000..6000,
  at PLL 54 MHz / HTS 1800. `cmos_fps_set()` computes it — no fps table.
  `ini` / yaml fps are not read by the library.

## Files

| File | Purpose |
|---|---|
| `src/sc1035_cmos.c` | ISP sensor-expert interface (AE/AWB/ISP defaults, `cmos_fps_set`) |
| `src/sc1035_sensor_ctl.c` | Register table, I2C access (`hi_i2c.ko`), VI introspection |
| `include/` | MPP V1.0.B.0 ABI headers (Hisilicon SDK, closure for both .c) |
| `sc1035_960p.ini` | Sensor ini for majestic: `DllFile` -> this library, default 1280x960@18 |

Install paths used by firmware integration (OpenIPC `libsns-sc1035-universal`
package, branch `feature/hi3518ev100-rtl` of the firmware fork):

- `libsns_sc1035_universal.so` -> `/usr/lib/sensors/`
- `sc1035_960p.ini` -> `/etc/sensors/sc1035_960p.ini` (overwrites the stock one)

## Build

```sh
make                          # native
make CC=<cross-gcc>           # cross, e.g. the divinus hi3516cv100 toolchain
```

Flags: `-Dhi3518 -DHICHIP=0x35180100 -DISP_V1 -Iinclude -Wall -Os -fPIC -shared`.

## Supported modes (hardware-validated)

The VTS formula accepts any fps 5..30 for both windows, but the encoder
budget of the SoC (hi3518ev100 VEDU ≈ 23 Mpix/s) caps what is actually
usable. Only these two pairs are validated:

| Mode | Mpix/s | VEDU budget | Status |
|---|---|---|---|
| `1280x720 @ 25` | 9.2 | ~40% | verified — 24.99 fps measured |
| `1280x960 @ 18` | 22.1 | ~96% | verified — 18.00 fps measured, multi-hour soaks, 0 stalls / seq gaps |

**960p ceiling is 18 fps.** 960@19 is already ~101% of the budget;
720p can go up to 30 fps (60% budget) but 25 is the validated duty mode.
Any mode outside this table is untested — use at your own risk.

## Why not 960@30 (observed problems)

`1280x960@30` = 36.9 Mpix/s = **~160% of the encoder budget** — outside
the hardware envelope. What was actually observed on this board:

- **Silent fps substitution (stock vendor library)**: an fps the vendor
  lib does not implement (e.g. 18) is silently ignored — the sensor stays
  at the default 30 fps while majestic still logs «@18fps». Config and
  reality diverge; only register reads (fps = PLL / (HTS x VTS), VTS from
  0x320e/0x320f) or a stream probe show the true rate. This library
  fixes it via the VTS formula, but the trap is worth knowing.
- **Short-term 960@30 looks deceptively clean**: ≈2 minutes at 960@30
  delivered 29.99 fps with 0 gaps — no immediate wedge. The mode was
  never soak-tested; sustained 160% load risks VENC/VEDU overload
  (dropped frames, encoder wedge → watchdog reboot → ~50 s outage loop
  — the documented failure chain of this platform under over-budget
  load).
- **fps mismatch drops frames**: with the sensor at 30 and the codec
  configured lower (ini 40 / yaml 18 case) VENC drops to the yaml rate —
  measured 13.5 fps delivery. Sensor fps and `majestic.yaml` fps must
  always agree.

18 fps was chosen as the highest 960p mode that fits the budget (96%)
and is verified stable over multi-hour runs.

## Notes

- Dead ABI slots (`set_image_mode` / `set_resolution`) are left NULL — the
  MPP V1.0.B.0 runtime never calls them (verified by disassembly).
- Requires `reset_sensor()` in `load_hisilicon` (devmem pulse on
  0x200f0124 / 0x20140400 / 0x20140008) — without the GPIO reset the sensor
  stays dead after a cold boot (proven on the Netis SEC111 board).
- Library code: MIT. `include/` headers are Hisilicon SDK material and keep
  their original copyright notices.

## Verified on hardware

- 1280x960@18: VTS 1667, PLL 0x46, YOUT 0x3c0 — 18.00 fps measured
- 1280x720@25: VTS 1200, YOUT 0x2d0 — 25.00 fps measured
- fps clamp: requests > 30 fall back to VTS 1000 = 30 fps
