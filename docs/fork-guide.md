<p align="right">
  <a href="fork-guide.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Maintaining a Pocket Polish fork

Pocket Polish builds on FoloToy AI Passport. The default branch contains the Polish learning firmware. Start a contribution from the default branch of the repository that will receive your pull request.

The course and study screens live in `main/`, board support in `components/bsp/`, and the learning assets in `assets/`. The root [README](../README.md) describes the application. The [documentation index](README.md) links to the original platform references and engineering guides.

The standalone project has removed the template's automatic upstream synchronization workflow. When adopting a board fix, review the specific upstream change and its effect on this course, then submit the applicable change for review. [Upstream changes](development/ci/CI-sync-main.md) · [Contributing](../.github/CONTRIBUTING.md)
