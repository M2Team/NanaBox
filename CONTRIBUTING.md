# Contributing to NanaBox

## How to become a contributor

- Direct contributions
  - We use a code style similar but not identical to that of Windows NT kernel
    drivers. You must read the code style guidelines carefully, word by word,
    before submitting your pull request. To maintain NanaBox's source code
    quality, and to respect our reviewers' time, we will not accept PRs that
    don't follow these guidelines.
  - We expect all contributions to match our existing style WITHOUT EXCEPTION.
    If you have any questions about our coding standards, please open an issue
    for discussion before submitting your pull request.
  - By submitting a pull request, you agree to license your contribution under
    the MIT license. We reserve the right to reuse and rewrite contributors' PRs
    as needed.
  - You are forbidden to modify any content in any files and folders starting
    with the "Mile." prefix because those implementations are shared across many
    projects, or your pull request will not be merged and will be closed
    immediately, unless you get the permission from Kenji Mouri.
  - It's OK to use AI/LLM tools to help you write code, but you must ensure that
    the final commit is written and reviewed by yourself, following our coding
    style and conventions. If we find that your pull request has no contribution
    from you, or the contribution is not reviewed by you, your pull request will
    not be merged and will be closed immediately.
- Feedback suggestions and bugs.
  - We use GitHub issues to track bugs and features.
  - For bugs and general issues please 
    [file a new issue](https://github.com/M2Team/NanaBox/issues/new).

## Code contribution guidelines

### Prerequisites

- Visual Studio 2022 or later.
  - You also need install ARM64 components (MSVC Toolchain and ATL/MFC) if you
    want to compile the ARM64 version of NanaBox.
- Windows 11 SDK or later.
  - You also need to install ARM64 components if you want to compile ARM64
    version of NanaBox.

### How to build all targets of NanaBox

Run `BuildAllTargets.bat` in the root of the repository.

### How to modify or debugging NanaBox

Open `NanaBox.slnx` in the root of the repository.

### Code style and conventions

Read Kenji Mouri's [MD24: The coding style for all my open-source projects] for
more information.

[MD24: The coding style for all my open-source projects]: https://github.com/MouriNaruto/MouriDocs/tree/main/docs/24

For all languages respect the [.editorconfig](https://editorconfig.org/) file 
specified in the source tree. Many IDEs natively support this or can with a 
plugin.

#### Translation contribution notice

All `comment` in `resw` files should be kept English for make it better to 
maintenance in the future.
