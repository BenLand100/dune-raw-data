#ifndef dune_artdaq_Overlays_FragmentType_hh
#define dune_artdaq_Overlays_FragmentType_hh
#include "artdaq-core/Data/Fragment.hh"

namespace dune {

  namespace detail {
    enum FragmentType : artdaq::Fragment::type_t
    { MISSED = artdaq::Fragment::FirstUserFragmentType,
        TPC,
        PHOTON,
        TRIGGER,
        TOY1,
        TOY2,
        INVALID // Should always be last.
        };

    // Safety check.
    static_assert(artdaq::Fragment::isUserFragmentType(FragmentType::INVALID - 1),
                  "Too many user-defined fragments!");
  }

  using detail::FragmentType;

  FragmentType toFragmentType(std::string t_string);
  std::string fragmentTypeToString(FragmentType val);
}
#endif /* dune_artdaq_Overlays_FragmentType_hh */