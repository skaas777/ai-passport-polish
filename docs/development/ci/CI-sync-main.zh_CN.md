<p align="right">
  <strong>简体中文</strong> · <a href="CI-sync-main.md">English</a>
</p>

# 引入上游板级修复

独立的随身波兰语仓库已移除板卡模板中的 `sync-main.yml` 定时任务。维护者可审阅 [FoloToy AI Passport](https://github.com/FoloToy/ai-passport) 的单项改动，并通过本项目的拉取请求引入适用的 BSP 或工具链修复。

移植修改时，请注明原始上游提交，说明它对课程固件的影响，并运行仓库的验证命令。涉及板卡行为的改动还应说明真机观察结果。[派生仓库流程](../../fork-guide.zh_CN.md) · [持续集成验证](CI-validation.zh_CN.md)
