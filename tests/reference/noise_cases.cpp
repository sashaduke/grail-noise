// Emits tests/noise/cases.grl: FastNoiseLite 1.1.1 outputs (double coordinates, float results)
// for each configuration Abundance uses, as exact binary32 bit patterns, and a Grail program that
// recomputes them with lib/noise.grl and compares bits.
//   clang++ -std=c++17 -O1 -I<fastnoiselite>/include noise_cases.cpp -o cases && ./cases > ../../noise/cases.grl
#include <FastNoiseLite.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

struct Config {
    const char *name, *setup; // setup: Grail statements configuring `n`
    FastNoiseLite noise;
    bool warp;
};
static uint32_t bits(float f) { uint32_t u; std::memcpy(&u, &f, 4); return u; }
static uint64_t bits64(double d) { uint64_t u; std::memcpy(&u, &d, 8); return u; }

int main() {
    const int points = 50;
    double xs[points], ys[points];
    for (int i = 0; i < points; ++i) { // spread over negative and positive, fractional coordinates
        xs[i] = -517.25 + i * 23.4567891;
        ys[i] = 311.125 - i * 17.8901234 + (i % 7) * 0.375;
    }
    Config cases[16];
    int count = 0;
    auto add = [&](const char *name, const char *setup, bool warp) -> FastNoiseLite & {
        cases[count].name = name; cases[count].setup = setup; cases[count].warp = warp;
        cases[count].noise = FastNoiseLite(1337);
        return cases[count++].noise;
    };
    { auto &n = add("os2", "", false); (void)n; }
    { auto &n = add("os2s", "noise::set_noise_type(&mut n, noise::NOISE_OPENSIMPLEX2S);", false); n.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S); }
    { auto &n = add("perlin", "noise::set_noise_type(&mut n, noise::NOISE_PERLIN);", false); n.SetNoiseType(FastNoiseLite::NoiseType_Perlin); }
    { auto &n = add("value", "noise::set_noise_type(&mut n, noise::NOISE_VALUE);", false); n.SetNoiseType(FastNoiseLite::NoiseType_Value); }
    { auto &n = add("cellular", "noise::set_noise_type(&mut n, noise::NOISE_CELLULAR);", false); n.SetNoiseType(FastNoiseLite::NoiseType_Cellular); }
    { auto &n = add("cellular_value_euclid", "noise::set_noise_type(&mut n, noise::NOISE_CELLULAR); noise::set_cellular_distance(&mut n, noise::CELL_EUCLIDEAN); noise::set_cellular_return(&mut n, noise::CELL_VALUE);", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_Cellular); n.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean); n.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue); }
    { auto &n = add("cellular_d2div_euclid", "noise::set_noise_type(&mut n, noise::NOISE_CELLULAR); noise::set_cellular_distance(&mut n, noise::CELL_EUCLIDEAN); noise::set_cellular_return(&mut n, noise::CELL_DISTANCE2_DIV);", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_Cellular); n.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean); n.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div); }
    { auto &n = add("cellular_hybrid_d2add", "noise::set_noise_type(&mut n, noise::NOISE_CELLULAR); noise::set_cellular_distance(&mut n, noise::CELL_HYBRID); noise::set_cellular_return(&mut n, noise::CELL_DISTANCE2_ADD); noise::set_cellular_jitter(&mut n, f32(0.75));", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_Cellular); n.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid); n.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add); n.SetCellularJitter(0.75f); }
    { auto &n = add("os2_fbm5", "noise::set_fractal_type(&mut n, noise::FRACTAL_FBM); noise::set_fractal_octaves(&mut n, 5); noise::set_fractal_gain(&mut n, f32(0.55)); noise::set_frequency(&mut n, f32(0.0125));", false);
      n.SetFractalType(FastNoiseLite::FractalType_FBm); n.SetFractalOctaves(5); n.SetFractalGain(0.55f); n.SetFrequency(0.0125f); }
    { auto &n = add("os2s_fbm_weighted", "noise::set_noise_type(&mut n, noise::NOISE_OPENSIMPLEX2S); noise::set_fractal_type(&mut n, noise::FRACTAL_FBM); noise::set_fractal_octaves(&mut n, 4); noise::set_fractal_lacunarity(&mut n, f32(2.25)); noise::set_fractal_weighted_strength(&mut n, f32(0.4));", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S); n.SetFractalType(FastNoiseLite::FractalType_FBm); n.SetFractalOctaves(4); n.SetFractalLacunarity(2.25f); n.SetFractalWeightedStrength(0.4f); }
    { auto &n = add("perlin_ridged", "noise::set_noise_type(&mut n, noise::NOISE_PERLIN); noise::set_fractal_type(&mut n, noise::FRACTAL_RIDGED); noise::set_fractal_octaves(&mut n, 6); noise::set_fractal_weighted_strength(&mut n, f32(0.25));", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_Perlin); n.SetFractalType(FastNoiseLite::FractalType_Ridged); n.SetFractalOctaves(6); n.SetFractalWeightedStrength(0.25f); }
    { auto &n = add("value_fbm", "noise::set_noise_type(&mut n, noise::NOISE_VALUE); noise::set_fractal_type(&mut n, noise::FRACTAL_FBM); noise::set_frequency(&mut n, f32(0.003));", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_Value); n.SetFractalType(FastNoiseLite::FractalType_FBm); n.SetFrequency(0.003f); }
    { auto &n = add("cellular_ridged", "noise::set_noise_type(&mut n, noise::NOISE_CELLULAR); noise::set_fractal_type(&mut n, noise::FRACTAL_RIDGED);", false);
      n.SetNoiseType(FastNoiseLite::NoiseType_Cellular); n.SetFractalType(FastNoiseLite::FractalType_Ridged); }
    { auto &n = add("warp_single", "noise::set_domain_warp_amp(&mut n, f32(30.0)); noise::set_frequency(&mut n, f32(0.004));", true);
      n.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2); n.SetDomainWarpAmp(30.0f); n.SetFrequency(0.004f); }
    { auto &n = add("warp_progressive", "noise::set_domain_warp_amp(&mut n, f32(45.0)); noise::set_frequency(&mut n, f32(0.002)); noise::set_fractal_type(&mut n, noise::FRACTAL_WARP_PROGRESSIVE); noise::set_fractal_octaves(&mut n, 3); noise::set_fractal_gain(&mut n, f32(0.6));", true);
      n.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2); n.SetDomainWarpAmp(45.0f); n.SetFrequency(0.002f); n.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive); n.SetFractalOctaves(3); n.SetFractalGain(0.6f); }

    // 3D OpenSimplex2S (rock kit: FBm, 5 octaves, frequency varies) over the same coordinates with z.
    FastNoiseLite r1(1337); r1.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    FastNoiseLite r2(4242); r2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S); r2.SetFrequency(0.035f);
    r2.SetFractalType(FastNoiseLite::FractalType_FBm); r2.SetFractalOctaves(5); r2.SetFractalLacunarity(2.0f); r2.SetFractalGain(0.5f); r2.SetFractalWeightedStrength(0.0f);
    std::printf("// Generated by tests/reference/noise/noise_cases.cpp from FastNoiseLite 1.1.1; do not edit.\n");
    std::printf("use noise;\nuse noise_tables;\n\n");
    std::printf("fn f32_bits(v: f32) -> u64 {\n    // binary32 bits of an f32 (held as the binary64 word of its value)\n");
    std::printf("    b := bits(f64(v));\n    sign := b >> 63;\n    if (b & 9223372036854775807) == 0 {\n        return sign * 2147483648;\n    }\n");
    std::printf("    // normal binary32 values only (the noise outputs are well inside the normal range)\n");
    std::printf("    exp := ((b >> 52) & 2047) + 127 - 1023;\n    mant := (b >> 29) & 8388607;\n    return sign * 2147483648 + exp * 8388608 + mant;\n}\n\n");
    std::printf("fn coords() -> f64[] {\n    return f64[");
    for (int i = 0; i < points; ++i) std::printf("%s%.17g, %.17g", i ? ", " : "", xs[i], ys[i]);
    std::printf("];\n}\n\nfn main() {\n    c := coords();\n");
    for (int k = 0; k < count; ++k) {
        Config &cfg = cases[k];
        std::printf("    {\n        mut n = noise::new(1337);\n        %s\n", cfg.setup);
        if (cfg.warp) {
            std::printf("        want := u64[");
            for (int i = 0; i < points; ++i) {
                double x = xs[i], y = ys[i];
                cfg.noise.DomainWarp(x, y);
                std::printf("%s%llu, %llu", i ? ", " : "", (unsigned long long)bits64(x), (unsigned long long)bits64(y));
            }
            std::printf("];\n        mut bad = 0;\n        for i in %d {\n", points);
            std::printf("            p := noise::warp(&n, noise::Point { x: c[2 * i], y: c[2 * i + 1] });\n");
            std::printf("            if (bits(p.x) != want[2 * i]) || (bits(p.y) != want[2 * i + 1]) {\n                bad += 1;\n            }\n        }\n");
        } else {
            std::printf("        want := u64[");
            for (int i = 0; i < points; ++i) std::printf("%s%u", i ? ", " : "", bits(cfg.noise.GetNoise(xs[i], ys[i])));
            std::printf("];\n        mut bad = 0;\n        for i in %d {\n", points);
            std::printf("            if f32_bits(noise::get(&n, c[2 * i], c[2 * i + 1])) != want[i] {\n                bad += 1;\n            }\n        }\n");
        }
        std::printf("        println(\"%s: {} of %d differ\", bad);\n    }\n", cfg.name, points);
    }
    struct { const char *name, *setup; FastNoiseLite *n; } three[] = {
        {"os2s_3d", "noise::set_noise_type(&mut n, noise::NOISE_OPENSIMPLEX2S);", &r1},
        {"os2s_3d_fbm5", "noise::set_noise_type(&mut n, noise::NOISE_OPENSIMPLEX2S); noise::set_frequency(&mut n, f32(0.035)); noise::set_fractal_type(&mut n, noise::FRACTAL_FBM); noise::set_fractal_octaves(&mut n, 5);", &r2},
    };
    for (auto &t : three) {
        int seed = t.n == &r2 ? 4242 : 1337;
        std::printf("    {\n        mut n = noise::new(%d);\n        %s\n        want := u64[", seed, t.setup);
        for (int i = 0; i < points; ++i) {
            double z = -73.5 + i * 3.125;
            std::printf("%s%u", i ? ", " : "", bits(t.n->GetNoise(xs[i], ys[i], z)));
        }
        std::printf("];\n        mut bad = 0;\n        for i in %d {\n", points);
        std::printf("            z := -73.5 + f64(i) * 3.125;\n");
        std::printf("            if f32_bits(noise::get3(&n, noise::Point3 { x: c[2 * i], y: c[2 * i + 1], z: z })) != want[i] {\n                bad += 1;\n            }\n        }\n");
        std::printf("        println(\"%s: {} of %d differ\", bad);\n    }\n", t.name, points);
    }
    std::printf("}\n");
    return 0;
}
