# Git Hooks

This directory contains project-managed git hooks. After cloning the repository,
run the following command once to activate them:

```bash
git config core.hooksPath .githooks
```

To unset, do the following:
```bash
git config --unset core.hooksPath
```

## Hooks

### `pre-commit`

Runs automatically before each commit:

- **clang-format** — formats any staged C/C++ source files and re-stages the result.

The commit proceeds normally after any auto-corrections are applied.
To bypass the hook in exceptional cases:

```bash
git commit --no-verify
```

## Matching the CI clang-format version

CI currently uses **clang-format 21** (LLVM 21). If your distro ships a different
default version, formatting results may differ and the CI check can fail.

Install the matching package:

Example for Ubuntu 24.04:
```bash
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-21 main" | sudo tee /etc/apt/sources.list.d/llvm.list
sudo apt-get -y update
sudo apt-get install clang-format-21
```

Example for Archlinux:
```bash
# Example for LLVM 21 — adjust if the CI version changes
sudo pacman -Sy clang21
```

Then set `OCUDU_CLANG_FORMAT` to point at the versioned binary. The hook and the
CI script both honour this variable and prefer it over whatever `clang-format` resolves
to on `$PATH`.

Add the following to your `~/.bashrc` (or `~/.zshrc`):

```bash
# Define clang-format version (package clang-format-21 / llvm21)
export OCUDU_CLANG_FORMAT="/usr/lib/llvm-21/bin/clang-format"
alias clang-format="${OCUDU_CLANG_FORMAT}"
```

Reload your shell afterwards:

```bash
source ~/.bashrc
```

Verify the active version:

```bash
"${OCUDU_CLANG_FORMAT}" --version
# clang-format version 21.x.x
```
