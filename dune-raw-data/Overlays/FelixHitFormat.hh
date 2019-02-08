#ifndef artdaq_dune_Overlays_FelixHitFormat_hh
#define artdaq_dune_Overlays_FelixHitFormat_hh

namespace dune
{
    struct TriggerPrimitive
    {
        TriggerPrimitive(uint16_t channel_,
                         uint64_t startTime_,
                         uint16_t charge_,
                         uint16_t timeOverThreshold_)
            : channel(channel_), 
              startTime(startTime_), 
              charge(charge_), 
              timeOverThreshold(timeOverThreshold_)
        {}

        uint16_t channel;
        uint64_t startTime;
        uint16_t charge;
        uint16_t timeOverThreshold;
    };
}

#endif // include guard
