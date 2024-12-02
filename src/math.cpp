
#include <druck/math.hpp>
#include <Reputeless/PerlinNoise.hpp>


namespace druck::math {

    double perlin_noise(uint32_t seed, const Vec<2>& pos) {
        return siv::PerlinNoise(seed).octave2D_01(pos.x(), pos.y(), 4);
    }


    static const uint64_t lcg_mltpl = 6364136223846793005;
    static const uint64_t lcg_incr = 1442695040888963407;

    uint64_t random_int(uint64_t input) {
        return input * lcg_mltpl + lcg_incr;
    }

    double random_float(uint64_t input) {
        return fabs((double) random_int(input)) / (double) UINT64_MAX;
    }


    StatefulRNG::StatefulRNG(uint64_t seed) {
        this->state = seed;
    }

    uint64_t StatefulRNG::next_int() {
        this->state = random_int(this->state);
        return this->state;
    }

    double StatefulRNG::next_float() {
        return fabs((double) this->next_int()) / (double) UINT64_MAX;
    }

}