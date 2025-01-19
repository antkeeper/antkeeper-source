<!--
SPDX-FileCopyrightText: 2025 C. J. Howard
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Contributing to Antkeeper

Thanks for taking the time to make Antkeeper better! ❤🐜

## Table of Contents

-   [Reporting Bugs](#reporting-bugs)
-   [Suggesting Changes](#suggesting-changes)
-   [Contributing Code](#contributing-code)
-   [Adding a Language](#adding-a-language)

## Reporting Bugs

To report a bug, please do one of the following:

-   [Open an issue](https://github.com/antkeeper/antkeeper-source/issues) on GitHub.
-   [Email us](mailto:contact@antkeeper.com) a description of the bug.
-   [Chat with us](https://discord.gg/AQA955HbK3) about the bug on Discord.

## Suggesting Changes

To suggest changes or new features, please do one of the following:

-   [Open an issue](https://github.com/antkeeper/antkeeper-source/issues) on GitHub.
-   [Email us](mailto:contact@antkeeper.com) your suggestion.
-   [Chat with us](https://discord.gg/XpjNdXHCJK) about your suggestion on Discord.

## Contributing Code

If you have written code that fixes a bug or provides new functionality, please [open a pull request](https://github.com/antkeeper/antkeeper-source/pulls) on GitHub.

### Coding Conventions

Ensure that all code contributions adhere to the following conventions:

-   C++ STL naming conventions
-   Allman indent style with tabs

## Adding a Language

If you would like to translate Antkeeper into your native language, please do the following:

1.  [Create a new fork](https://github.com/antkeeper/antkeeper-source/fork) of this repository.
2.  Create a JSON file for the language at [localization/languages](../res/localization/languages)/`<target_language_tag>`.json, where `<target_language_tag>` is the [IETF language tag](https://en.wikipedia.org/wiki/IETF_language_tag) of the target language. Use [en.json](../res/localization/languages/en.json) or [zh-Hans.json](../res/localization/languages/zh-Hans.json) as a template.
3.  Add a column named `<target_language_tag>` to [strings.csv](../res/localization/strings.csv).
4.  Translate all strings from the `en` column into the `<target_language_tag>` column. Check the `context` column for additional information about each string.
5.  Append your name to [CONTRIBUTORS.md](./CONTRIBUTORS.md) to be properly credited for your contributions.
6.  Commit your changes to the fork.
7.  [Open a pull request](https://github.com/antkeeper/antkeeper-source/pulls) on GitHub.
