# FastNoiseLite reference fixtures

Both programs build against the vendored FastNoiseLite 1.1.1 header (Abundance's
`ThirdParty/fastnoiselite`).

- `noise_tables.cpp` prints `lib/noise_tables.grl`: the 2D gradient and random-vector tables and
  the float constants the 2D kernels fold at compile time, each as the exact binary64 value of the
  binary32 constant, so `f32()` in Grail reproduces the same bits without decimal double rounding.
- `noise_cases.cpp` prints `tests/noise/cases.grl`: for 17 configurations (every noise type
  Abundance uses, cellular distance/return variants, FBm with gain and weighted strength, Ridged,
  single and progressive OpenSimplex2 domain warp) it evaluates 50 double-precision coordinates
  and records the exact float (or, for warps, double) bit patterns. The Grail program recomputes
  them with `lib/noise.grl` and reports any differing sample; the native case
  `lib-noise-reference` requires zero.

```sh
clang++ -std=c++17 -O0 -I<fastnoiselite>/include noise_tables.cpp -o tables && ./tables > ../../../lib/noise_tables.grl
clang++ -std=c++17 -O1 -I<fastnoiselite>/include noise_cases.cpp -o cases && ./cases > ../../noise/cases.grl
```
