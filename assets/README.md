**English** · [简体中文](README.zh_CN.md)

# Course assets

## Pronunciation

Each line in [the recording index](audio/words.txt) names one 11,025 Hz mono PCM16 WAV in `audio/raw/`, starting with `000.wav`. The firmware build matches clips to the Polish text, verifies the WAV format, and packs the samples into flash. The recordings ship with the source.

We generated the recordings from the course text with the built-in Polish voice in eSpeak NG 1.52.0 at 145 words per minute, and resampled them with FFmpeg 8.0. To add missing clips, run `bash assets/audio/generate_audio.sh` from the repository root. Pass `--all` to regenerate every clip. These programs are authoring tools. [eSpeak NG](https://github.com/espeak-ng/espeak-ng) · [FFmpeg](https://ffmpeg.org/)

## Screen fonts and preview

| Use | Font | License |
| --- | --- | --- |
| Polish, 20 px | Liberation Sans Regular | [SIL OFL 1.1](fonts/LiberationSans-OFL.txt) |
| Chinese interface and translations, 16 px | Noto Sans SC Regular | [SIL OFL 1.1](fonts/NotoSansSC-OFL.txt) |
| IPA, 16 px | DejaVu Sans | [DejaVu, Bitstream Vera and Arev notices](fonts/DejaVuSans-LICENSE.txt) |

The subset bitmaps in `fonts/` are build inputs. After ESP-IDF fetches the locked LVGL component, run `python3 assets/fonts/course_glyphs.py --generate` with Node.js and npx installed to rebuild them. Run the same command without `--generate` to check glyph coverage. The [preview](images/course-preview.png) shows the actual LVGL course screens rendered on the host.

## Battery profiles

[`battery/wanli-500mah-profile2-xil40x-20261020.txt`](battery/wanli-500mah-profile2-xil40x-20261020.txt)
contains the supplied CW2017 profile for the Wanli New Energy 500 mAh cell
(profile2, XIL40X, 2026-10-20). It is an unchanged copy of the project-supplied
file, retained at the maintainer's request for this firmware update. Its
original filename is recorded in the paired Chinese document. No separate
license was included in the supplied file.

The format is 80 lines of two hexadecimal digits, one byte per line in register
order. The BSP embeds those bytes in `components/bsp/src/bsp_battery.c`; the
host gate checks the source file and initializer for an exact match. Text-file
SHA-256: `7ea96eba4634f4495a37c25ffacd54c1deaa0693b162c892d671a2ba68460919`.
Decoded 80-byte SHA-256:
`fa7365671093ef449c5bdcfb4adb279e3d275f888109f50b6b2f21e9b96220d5`.
This is a fuel-gauge profile, not a charging configuration or a generic profile
for every 500 mAh cell; see the [hardware guide](../docs/hardware-design/AI_HARDWARE_DEVELOPMENT_GUIDE.md#9-cw2017-fuel-gauge).
