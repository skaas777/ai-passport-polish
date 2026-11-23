English | [简体中文](/.github/CONTRIBUTING.zh_CN.md)

# Contributing to Pocket Polish

Course corrections, pronunciation improvements, interface changes and bug reports are welcome. Read the [course and controls](/README.md) and the [community code of conduct](/.github/CODE_OF_CONDUCT.md). AI-assisted development follows [the repository guidelines](/AGENTS.md).

## Course changes

Include the affected topic and Polish phrase. For a translation or IPA correction, show the current text, your suggested wording and the reason for the change. Course text lives in `main/polish_course.c`. Update the corresponding recording and glyph coverage when the displayed or spoken text changes. See the [asset guide](/assets/README.md).

## Build and review

Start from this project's default branch. Open a pull request for one focused change. Explain how a learner can exercise it and which checks you ran. Include the board and observed results for changes tested on a device. Check both the English and Simplified Chinese pages when editing documentation.

Run `./tools/validate.sh --static` for the repository checks and host tests. With ESP-IDF 5.5.3 activated, run `./tools/validate.sh` for the complete gate and verified merged image. [Build guide](/docs/development/engineering/build-and-test.md) · [Pull request conventions](/docs/contribution/commit-and-pr.md)

Original code contributions use the project's [MIT license](/LICENSE). Include the source and applicable license with third-party assets. Use this repository's Issues tab for ordinary bugs or suggestions and follow the [security policy](/.github/SECURITY.md) for private reports.
