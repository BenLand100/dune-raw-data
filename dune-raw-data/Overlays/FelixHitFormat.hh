#ifndef artdaq_dune_Overlays_FelixHitFormat_hh
#define artdaq_dune_Overlays_FelixHitFormat_hh

namespace dune
{
    struct TriggerPrimitive
    {
        uint16_t channel;
        uint16_t startTimeOffset; // relative to 64-bit timestamp
        uint16_t charge;
        uint16_t timeOverThreshold;
    };
}

#endif // include guard
