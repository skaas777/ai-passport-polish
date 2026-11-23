[English](README.md) · **简体中文**

# 课程资源

## 波兰语发音

[录音索引](audio/words.txt)每行对应 `audio/raw/` 中一个从 `000.wav` 开始编号的 WAV 文件，格式为 11025 Hz、16 位单声道 PCM。固件构建时按波兰语原文匹配音频、核对格式并将采样数据打包到 Flash。源码包已包含全部录音。

本版使用 eSpeak NG 1.52.0 内置波兰语语音，以每分钟 145 词从课文合成录音，再用 FFmpeg 8.0 重采样。需要补录时，在仓库根目录运行 `bash assets/audio/generate_audio.sh`，加上 `--all` 可重新生成全部录音。只有制作录音时才需要这两种工具。[eSpeak NG](https://github.com/espeak-ng/espeak-ng) · [FFmpeg](https://ffmpeg.org/)

## 屏幕字库与预览

| 用途 | 字体 | 许可 |
| --- | --- | --- |
| 波兰语，20 像素 | Liberation Sans Regular | [SIL OFL 1.1](fonts/LiberationSans-OFL.txt) |
| 中文界面与释义，16 像素 | Noto Sans SC Regular | [SIL OFL 1.1](fonts/NotoSansSC-OFL.txt) |
| 国际音标，16 像素 | DejaVu Sans | [DejaVu、Bitstream Vera 和 Arev 许可](fonts/DejaVuSans-LICENSE.txt) |

`fonts/` 内的点阵字库可直接参与构建。ESP-IDF 获取锁定版本的 LVGL 组件后，在装有 Node.js 和 npx 的电脑上运行 `python3 assets/fonts/course_glyphs.py --generate` 即可重新生成字库。省略 `--generate` 可检查字形覆盖情况。[预览图](images/course-preview.png)使用主机上的 LVGL 渲染实际课程界面。

## 电池参数（battery）

[`battery/wanli-500mah-profile2-xil40x-20261020.txt`](battery/wanli-500mah-profile2-xil40x-20261020.txt)
保存万锂新能源 500 mAh 电芯的 CW2017 参数（profile2、XIL40X、2026-10-20）。
该文件是项目提供的 `喜梨_智能工卡_万锂新能源500mAh_profile2_XIL40X_20261020.txt`
的原样副本，按维护者要求保留用于本次固件更新；原文件没有附带单独的许可说明。

格式为 80 行两位十六进制数字，每行一个字节，按寄存器顺序排列。
BSP 在 `components/bsp/src/bsp_battery.c` 中嵌入这 80 字节，主机检查会验证
源文件及数组内容完全一致。文本文件 SHA-256：
`7ea96eba4634f4495a37c25ffacd54c1deaa0693b162c892d671a2ba68460919`。
解码后 80 字节 SHA-256：
`fa7365671093ef449c5bdcfb4adb279e3d275f888109f50b6b2f21e9b96220d5`。
这是电量计参数，不是充电配置，也不能通用于所有 500 mAh 电芯；详见
[硬件指南](../docs/hardware-design/AI_HARDWARE_DEVELOPMENT_GUIDE.zh_CN.md#9-cw2017-电池计)。
