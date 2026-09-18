# Git Hooks

This directory contains project-managed git hooks. After cloning the repository, run the following command once to
activate them:

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

The commit proceeds normally after any auto-corrections are applied. To bypass the hook in exceptional cases:

```bash
git commit --no-verify
```

### `post-checkout` / `post-merge` / `post-rewrite`

Runs automatically after checkouts, merges (including `git pull`), and anything that rewrites commits (`git rebase`,
`git commit --amend`).

- **ensure-mr-refspec.sh** — adds a fetch refspec for `origin` so that each merge request's top commit is fetched to a
  local ref, `refs/remotes/mr/<iid>`. This lets you do things like:

  ```bash
  git log mr/1234
  git bisect run bash -c '
    git for-each-ref refs/remotes/mr --format="%(objectname)" | grep -qx "$(git rev-parse HEAD)" || exit 125
    <your test>
  '
  ```

  The refspec is only added once (the hook is idempotent) and takes effect on your next `git fetch`/`git pull`. If you
  don't use `core.hooksPath`, add it manually instead:

  ```bash
  git config --add remote.origin.fetch '+refs/merge-requests/*/head:refs/remotes/mr/*'
  ```

- **fixup-rebased-mrs.sh** — GitLab rebases an MR's source branch before a fast-forward merge whenever the target branch
  has moved on, which gives the landed commit a new SHA. When that happens, `refs/remotes/mr/<iid>` is left pointing at
  the pre-rebase commit and `git log` won't decorate the commit that actually landed. This script detects that case by
  matching patch-id (stable across the rebase) and repoints `refs/remotes/mr/<iid>` at the real landed commit, so every
  MR decorates the same way in `git log` regardless of whether it needed a rebase.

  State is cached under `.git/mr-tags/`. Because the fetch refspec above force-updates `refs/remotes/mr/*`, a bare
  `git fetch` can reset an already-fixed ref back to the stale value; the script detects that via a cheap snapshot
  compare and re-applies known-good fixes automatically. A checkout/merge where nothing under `refs/remotes/mr` changed
  does no work. The very first run backfills over the last 5000 commits on the default branch and can take ~20-30s;
  after that it's near-instant.

## Manual utilities

### `reset-mr-refs.sh`

Not a hook — run it yourself if the local MR ref state ever looks wrong. Deletes every `refs/remotes/mr/<iid>` ref
(however it got created), the cached `fixup-rebased-mrs.sh` state, and the MR fetch refspec, then rebuilds everything
from scratch (re-fetch + full fixup backfill). Takes ~20-30s.

```bash
.githooks/reset-mr-refs.sh
```

## Matching the CI clang-format version

CI currently uses **clang-format 21** (LLVM 21). If your distro ships a different default version, formatting results
may differ and the CI check can fail.

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

Then set `OCUDU_CLANG_FORMAT` to point at the versioned binary. The hook and the CI script both honour this variable and
prefer it over whatever `clang-format` resolves to on `$PATH`.

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
