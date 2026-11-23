[English](/.github/CONTRIBUTING.md) | 简体中文

# 参与随身波兰语项目

欢迎纠正课程内容、改进发音和界面，或报告使用问题。请先了解[课程与按键](/README.zh_CN.md)，并遵守[社区行为准则](/.github/CODE_OF_CONDUCT.zh_CN.md)。使用 AI 辅助开发时请参阅[仓库开发规范](/AGENTS.zh_CN.md)。

## 修改课程

请注明主题和波兰语词句。修订翻译或国际音标时，请附上原文、建议改法和理由。课程文本在 `main/polish_course.c`。修改屏幕文字或朗读内容时，一并检查对应录音和字库覆盖情况。[资源指南](/assets/README.zh_CN.md)

## 构建与评审

从本项目的默认分支开始开发。每个拉取请求聚焦一项改动，说明学习者如何体验这项改动，以及你完成了哪些检查。如果做了真机测试，请注明板卡和实际观察结果。修改文档时核对英文和简体中文两个版本。

运行 `./tools/validate.sh --static` 检查仓库并执行主机测试。激活 ESP-IDF 5.5.3 后，运行 `./tools/validate.sh` 执行完整验证并生成校验过的合并固件。[构建指南](/docs/development/engineering/build-and-test.zh_CN.md) · [拉取请求规范](/docs/contribution/commit-and-pr.zh_CN.md)

自行编写的代码按项目的 [MIT 许可证](/LICENSE) 提交。第三方资源应附来源和适用许可。普通问题或建议请发到本仓库的 Issues，需私下报告的问题请参阅[安全策略](/.github/SECURITY.zh_CN.md)。
