<p align="right">
  <a href="CI-build-and-release.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Firmware builds and GitHub Releases

A maintainer publishes a Pocket Polish release by pushing a tag in this repository. Any pushed tag triggers the build workflow. After a successful build, the release job attaches the verified merged firmware to a GitHub Release titled with the tag name. A manual run from the Actions tab builds and uploads a workflow artifact, including when the selected ref is a tag. Only a tag push runs the release job. [Workflow definition](../../../.github/workflows/build-firmware.yml)

## Build and artifact

The build job runs `./tools/validate.sh --firmware` with ESP-IDF 5.5.3 for the ESP32-C3 and uploads `build/FoloToy-AI-Passport-full.bin`. Verification checks the bootloader, partition table, application, flash offsets and partition bounds. The build has read access to contents. Only the release job requests write access to publish the asset. [Firmware validation](../engineering/build-and-test.md)

The merged image targets offset `0x0`. Flashing it can reset saved learning progress. The firmware layout guide describes the compatible segmented update path for saving existing settings. [Flash layout](../engineering/firmware-layout.md)

## Preparing release notes

Review changes since the last release and update the English and Simplified Chinese changelogs before tagging the release commit. Reserve these changelog edits for release preparation. The release notes should explain user-visible changes, the build procedure, the target board, any stored-data impact and the hardware checks actually completed.
