# creator-tui: the ThorVG Creator TUI builds

This branch is what Creator TUI builds, through thorvg-rs's `creator-tui-1.1.3` branch. It is
`fix/exit-globals-1.1.3` (upstream v1.1.3 with our commits) plus a few fixes and one feature
cherry-picked onto it. Each is its own commit, the same change as the one on its own branch.

The `creator-tui` branch is the same on v1.1.2, kept as it was. `creator-tui-1.1.3-wg` (the
scissor clamp) and `creator-tui-1.1.3-conic` (the conic gradient) were this branch with one of
them each; both are folded in here, and are kept only until nothing points at them.

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
  - `fix/neon-alpha-blend`: the NEON `ALPHA_BLEND` scales by `a + 1` as the scalar and AVX ones do,
    so a file draws the same with NEON as without (it left an opaque pixel under a translucent
    one at alpha 254).
  - `feat/lottie-conic-gradient`: a gradient of type 3, which Lottie doesn't have, is drawn as
    a conic gradient around its start, from its end's direction. Creator TUI keeps a conic in its
    own data and hands ThorVG that type; the file it saves has a radial (type 2) players draw.
  - `fix/wg-scissor-clamp`: every scissor rect in `WgCompositor` is clamped to the render
    target, as wgpu-native aborts the process on one outside it. (Here it came first as
    a5acae45, from `creator-tui-1.1.3-wg`; the branch is that commit on upstream main.)
  - `fix/wg-texture-target-format`: a texture target is drawn in its own format, BGRA8 or
    RGBA8, rather than always BGRA8, so Creator TUI's stage comes back as RGBA without a swap
    on the CPU.
- `creator-tui-1.1.3` (this branch): `fix/exit-globals-1.1.3` with each fix branch's commit
  cherry-picked in that order, then the scissor clamp, the conic gradient, the NEON fix and the
  texture target format.

The WebGPU fixes reach Creator TUI's GPU build as `crates/tui/gpu/patches/` (its build.py builds
upstream v1.1.3 with them); the CPU build doesn't compile the WebGPU engine.

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

A fix or a feature added goes on the end of this branch, which is never rewritten:

    git checkout creator-tui-1.1.3
    git cherry-pick -x <its commit>
    git push origin creator-tui-1.1.3

When a fix branch changes, the branch is made again from the start, as a new branch (pushed
under a name of its own rather than forced over this one):

    git checkout -b <new branch> origin/fix/exit-globals-1.1.3
    git cherry-pick -x fix/text-anchor-init fix/text-range-ease-low \
        fix/text-range-random-whole fix/text-range-basis-percent fix/text-range-utf8 \
        fix/text-search-glyph-scope
    # (join the overlap above when the last one stops on it, then: git cherry-pick --continue)
    git cherry-pick -x a5acae45 feat/lottie-conic-gradient fix/neon-alpha-blend \
        fix/wg-texture-target-format
    git cherry-pick -x <this file's commits>

Then point thorvg-rs's `creator-tui-1.1.3` branch's `thorvg-sys/thorvg` at the new commit, and
the app's `thorvg-sys` pin at that thorvg-rs commit.
