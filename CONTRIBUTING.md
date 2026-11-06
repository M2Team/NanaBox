# Contributing to NanaBox

## How to become a contributor

- Direct contributions
  - We use a code style similar but not identical to that of Windows NT kernel
    drivers. You must read the code style guidelines carefully, word by word,
    before submitting your pull request. To maintain NanaBox's source code
    quality, and to respect our reviewers' time, we will not accept pull
    requests that don't follow these guidelines.
  - We expect all contributions to match our existing style WITHOUT EXCEPTION.
    If you have questions about our coding standards, ask in an issue or in your
    pull request description.
  - By submitting a pull request, you agree to license your contribution under
    the MIT license. We reserve the right to reuse and rewrite contributors'
    pull requests as needed.
  - You are forbidden to modify any content in any files and folders starting
    with the "Mile." prefix because those implementations are shared across many
    projects, or your pull request will not be merged and will be closed
    immediately, unless you get the permission from Kenji Mouri.
- Feedback suggestions and bugs.
  - We use GitHub issues to track bugs and features.
  - For bugs and general issues please 
    [file a new issue](https://github.com/M2Team/NanaBox/issues/new).

## AI/LLM use when contributing

- Before opening a pull request, read the basic guidance yourself, word for 
  word. AI/LLM assistance, including translation, must not replace your own
  reading and understanding. Readability-first requires human judgment.
- Ask about unclear points in an issue or the pull request description. Your
  questions must show that you have read the guidance; unstated uncertainty does
  not excuse style violations.
- Before committing, personally refine the draft so you can explain your
  reasoning for any added line when asked during review. This includes i18n
  entries and sources for translations outside your native language.
- Before pushing, review the changes yourself, then build and test locally.
- Write commit messages, pull request descriptions, and review comments
  yourself. Keep them concise, with no emojis or unnecessary tables.
  AI/LLM-assisted polishing must preserve your own reasoning.
- Commit messages and pull request descriptions must be in English. Keep commit
  messages to one sentence, adding lines only for essential context or
  attribution.

Pull requests will be closed unless you have personally refined and reviewed the
changes and successfully built and tested them locally.

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
