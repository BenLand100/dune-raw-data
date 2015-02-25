#ifndef lbne_artdaq_Overlays_PennMilliSliceFragment_hh
#define lbne_artdaq_Overlays_PennMilliSliceFragment_hh

#include "lbne-raw-data/Overlays/PennMilliSlice.hh"
#include "lbne-raw-data/Overlays/PennMicroSlice.hh"
#include "artdaq-core/Data/Fragment.hh"

namespace lbne {
  class PennMilliSliceFragment;
}

class lbne::PennMilliSliceFragment : public lbne::PennMilliSlice {

public:

  // This constructor accepts an artdaq::Fragment that contains an existing
  // PennMilliSlice and allows the the data inside the PennMilliSlice to be accessed
  PennMilliSliceFragment(artdaq::Fragment const& frag);

protected:

  // returns a pointer to the header
  Header const* header_() const;

  // returns a pointer to the requested MicroSlice
  uint8_t* data_(int index) const;

private:

  artdaq::Fragment const& artdaq_fragment_;
};

#endif /* lbne_artdaq_Overlays_PennMilliSliceFragment_hh */
