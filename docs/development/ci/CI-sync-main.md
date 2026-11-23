<p align="right">
  <a href="CI-sync-main.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Adopting upstream board fixes

The standalone Pocket Polish repository has removed the scheduled `sync-main.yml` job from the board template. Its maintainers can review individual changes in [FoloToy AI Passport](https://github.com/FoloToy/ai-passport) and propose relevant BSP or toolchain fixes through this project's pull requests.

For a ported change, cite the original upstream commit, describe its effect on the course firmware and run the shared validation gate. Report device observations for changes to board behavior. [Fork workflow](../../fork-guide.md) · [Validation](CI-validation.md)
