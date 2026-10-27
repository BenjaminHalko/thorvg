# creator-tui: the ThorVG Creator TUI builds

This branch is what Creator TUI builds, through thorvg-rs's `creator-tui-1.1.3` branch. It is
upstream v1.1.3 with the bare-metal series (e04d35a9) plus a few fixes and one feature
cherry-picked onto it. Each is its own commit, the same change as the one on its own branch.

It carries only changes upstream would take: each is merged upstream, in an upstream pull
request, or about to be sent. What upstream turned down comes off (reverted on the end) and is
solved in Creator TUI instead.

The `creator-tui` branch is the same on v1.1.2, kept as it was. `creator-tui-1.1.3-wg` (the
scissor clamp) and `creator-tui-1.1.3-conic` (the conic gradient) were this branch with one of
them each; both are folded in here, and are kept only until nothing points at them.

## The branches

- `fix/exit-globals-1.1.3`: upstream v1.1.3 with the bare-metal series, plus the fixes that
  kept ThorVG's static mutexes, memory pools, cached loaders and its start and stop lock alive
  past exit. Upstream's answer is that `Initializer::term()` is paired with `init()`
  (thorvg/thorvg#4951), so those three are reverted on the end of this branch: Creator TUI lets
  what is drawing with ThorVG finish, refuses more, and calls `term()` before it exits. `fix/exit-globals` is the
  same on v1.1.2.
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
  - `fix/gaussian-row-int`: the scalar gaussian blur row's bounds are `int`, so `clamp()`
    finds one type for them where `int32_t` is `long` (riscv32, which thorvg-rs's bare-metal CI
    builds), rather than failing to compile there.
  - `fix/bezier-segments-no-alloc`: `Bezier::segments()` keeps the halves still to flatten in a
    small array on the stack instead of an `Array` allocated and grown on every call (the GL and
    WebGPU tessellators call it for every curve; the CPU engine doesn't).
  - `fix/wg-threaded-shape-geometry`: the WebGPU engine makes its shapes' geometry on the task
    scheduler's threads, as the CPU engine prepares its shapes, rather than one after another on
    the calling thread.
  - `fix/wg-keep-gradient-ramp`: the WebGPU engine keeps a gradient's 512-texel ramp texture while
    its color stops are the same, rather than making and uploading it again on every update.
  - `fix/lottie-per-dimension-easing`: a keyframe whose handles have a value for each dimension
    (`"x": [0.8, 0.1]`, as After Effects writes them) eases a point's y by its own, rather than by
    x's; a move whose spatial tangents are both zero is straight, not a curve.
  - `lottie: a document's expressions run in one thread's JerryScript context` (bd5734b4, on this
    fork's `main` from #3, with no branch of its own): a JerryScript context found again by a reused
    thread id is bound before use, and a document with expressions is parsed, built and updated on
    the calling thread rather than on the task scheduler's workers, so expressions no longer crash
    when they run on more than one thread.
- `creator-tui-1.1.3` (this branch): `fix/exit-globals-1.1.3` with each fix branch's commit
  cherry-picked in that order, then the scissor clamp, the conic gradient, the NEON fix, the
  texture target format, the gaussian row's bounds, the bezier segments, the threaded geometry,
  the gradient ramps, the per-dimension easing and the expression thread contexts, then the exit
  fixes reverted.

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

    git checkout -b <new branch> e04d35a9   # upstream v1.1.3 and the bare-metal series
    git cherry-pick -x fix/text-anchor-init fix/text-range-ease-low \
        fix/text-range-random-whole fix/text-range-basis-percent fix/text-range-utf8 \
        fix/text-search-glyph-scope
    # (join the overlap above when the last one stops on it, then: git cherry-pick --continue)
    git cherry-pick -x a5acae45 feat/lottie-conic-gradient fix/neon-alpha-blend \
        fix/wg-texture-target-format fix/gaussian-row-int fix/bezier-segments-no-alloc \
        fix/wg-threaded-shape-geometry fix/wg-keep-gradient-ramp fix/lottie-per-dimension-easing \
        bd5734b4
    git cherry-pick -x <this file's commits>

Then point thorvg-rs's `creator-tui-1.1.3` branch's `thorvg-sys/thorvg` at the new commit, and
the app's `thorvg-sys` pin at that thorvg-rs commit.
