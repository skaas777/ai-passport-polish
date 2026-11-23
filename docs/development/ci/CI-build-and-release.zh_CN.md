<p align="right">
  <strong>简体中文</strong> · <a href="CI-build-and-release.md">English</a>
</p>

# 固件构建与 GitHub Release

维护者向本仓库推送标签时会发布随身波兰语版本。推送的任何标签都会触发构建，构建成功后，发布任务会创建以标签命名的 GitHub Release，并附上校验过的合并固件。从 Actions 页面手动运行时会构建并上传工作流产物，即使选择标签也是如此。只有推送标签才会运行发布任务。[工作流定义](../../../.github/workflows/build-firmware.yml)

## 构建与产物

构建任务使用 ESP-IDF 5.5.3，为 ESP32-C3 运行 `./tools/validate.sh --firmware`，生成并上传 `build/FoloToy-AI-Passport-full.bin`。验证涵盖启动程序、分区表、应用、烧录偏移及分区边界。构建任务对仓库内容只有读取权限，发布任务申请写入权限以上传发布附件。[固件验证](../engineering/build-and-test.zh_CN.md)

合并镜像的起始偏移为 `0x0`，直接烧写可能重置已保存的学习进度。需要保留设置时，可参考固件布局指南中的兼容分段升级方法。[烧录布局](../engineering/firmware-layout.zh_CN.md)

## 准备版本说明

请在给发布提交打标签前梳理上次发布以来的变化，并同步更新英文和简体中文变更日志。变更日志在准备发布时集中更新。版本说明应列出用户可见的变化、构建方法、目标板卡、对已存数据的影响，以及实际完成的真机测试。
