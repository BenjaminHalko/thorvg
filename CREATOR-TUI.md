# creator-tui: the ThorVG Creator TUI builds

This branch is what Creator TUI builds, through thorvg-rs's `creator-tui-1.1.3` branch. It is
`fix/exit-globals-1.1.3` (upstream v1.1.3 with our commits) plus a few fixes cherry-picked onto
it. Each fix is its own commit, the same change as the one on its fix branch.

The `creator-tui` branch is the same on v1.1.2, kept as it was.

## The branches

- `fix/exit-globals-1.1.3`: the line under this branch. Upstream v1.1.3 with the bare-metal
  series, plus the fixes that keep ThorVG's static mutexes, memory pools, cached loaders and (new
  in v1.1.3) its start and stop lock alive past exit. `fix/exit-globals` is the same on v1.1.2.
- One branch per fix, each a single commit on upstream `thorvg/thorvg` main, kept there so it
  can be opened upstream as a pull request on its own:
  - `fix/text-anchor-init`: a text layer's alignment anchor starts at (0, 0) when the file has
    no `m.a`, instead of being left uninitialized.
  - `fix/text-range-ease-low`: a text range with an ease low and no ease high no longer eases
    through a null pointer.
  - `fix/text-range-random-whole`: a randomized text range as wide as the text no longer
    divides by zero.
  - `fix/text-range-basis-percent`: a text range in percent counts the units of its basis
    (words, lines, characters without spaces), not every character.
  - `fix/text-range-utf8`: text ranges count letters, not UTF-8 bytes.
  - `fix/text-search-glyph-scope`: a lower case letter in all capitals or small capitals is
    looked up as its capital through a buffer still in scope, not a stack slot gone by then.
- `creator-tui-1.1.3` (this branch): `fix/exit-globals-1.1.3` with each fix branch's commit
  cherry-picked in that order.

## The one overlap

`fix/text-range-basis-percent` and `fix/text-range-utf8` both change `RenderText` in
`src/loaders/lottie/tvgLottieBuilder.h`. One adds `nSpaces` and `nLines`, counted in its
constructor. The other turns `nChars` from `strlen` into a count of letters. Here the two are
joined into one loop that counts all three:

    int nChars = 0, nSpaces = 0, nLines = 1;
    ...
    for (auto c = p; *c; ++c) {
        if ((*c & 0xc0) != 0x80) ++nChars;
        if (*c == ' ') ++nSpaces;
        else if (*c == 13 || *c == 3) ++nLines;
    }

## Rebuilding it

When a fix branch changes, or one is added:

    git checkout -B creator-tui-1.1.3 origin/fix/exit-globals-1.1.3
    git cherry-pick -x fix/text-anchor-init fix/text-range-ease-low \
        fix/text-range-random-whole fix/text-range-basis-percent fix/text-range-utf8 \
        fix/text-search-glyph-scope
    # (join the overlap above when the last one stops on it, then: git cherry-pick --continue)
    git cherry-pick -x <this file's commits>
    git push --force-with-lease origin creator-tui-1.1.3

Then point thorvg-rs's `creator-tui-1.1.3` branch's `thorvg-sys/thorvg` at the new commit, and
the app's `thorvg-sys` pin at that thorvg-rs commit.
