<p align="right">
  <strong>简体中文</strong> · <a href="fork-guide.md">English</a>
</p>

# 维护随身波兰语派生仓库

随身波兰语基于 FoloToy AI Passport 开发，默认分支保存波兰语学习固件。提交贡献时，请从目标仓库的默认分支开始开发。

课程和学习界面位于 `main/`，板级驱动位于 `components/bsp/`，学习资源位于 `assets/`。根目录的[使用说明](../README.zh_CN.md)介绍应用本身。[文档索引](README.zh_CN.md)收录原平台资料和工程指南。

独立项目已移除模板中的上游自动同步工作流。引入硬件修复时，请核查相应上游改动及其对课程固件的影响，再提交适用的修改供评审。[上游改动](development/ci/CI-sync-main.zh_CN.md) · [贡献指南](../.github/CONTRIBUTING.zh_CN.md)
