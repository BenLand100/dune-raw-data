#ifndef dune_artdaq_Overlays_FelixHitFragment_hh
#define dune_artdaq_Overlays_FelixHitFragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"
#include "dune-raw-data/Overlays/FelixFormat.hh"
#include "dune-raw-data/Overlays/FelixHitFormat.hh"

#include <ostream>
#include <vector>

// Implementation of "FelixHitFragment".

namespace dune {
    class FelixHitFragment;

    // Let the "<<" operator dump the FelixHitFragment's data to stdout
    std::ostream & operator << (std::ostream &, FelixHitFragment const &);
}

class dune::FelixHitFragment {
public:

    struct Metadata {
        typedef uint32_t data_t;

        Metadata(data_t v) : fragment_version(v) {}
        data_t fragment_version;
        data_t num_hits;

        static size_t const size_words = 2ul; // Units of Metadata::data_t
    };

    // Constructor.  This class keeps a copy of a pointer to the fragment.
    // Basically that is how it works.
public: 
    FelixHitFragment(artdaq::Fragment& f ) : artdaq_Fragment_(f) 
    {}

    // The following structure is overlayed onto the data in the
    // fragment, starting at the beginning.
    struct Body {
        uint64_t timestamp;
        uint32_t nhits;
        // We have an array of TriggerPrimitive of unknown size
        // beginning in memory at this point, so use a "flexible array
        // member". Not sure how to do this more nicely, when we have to
        // have contiguous memory for the fragment
        dune::TriggerPrimitive primitives[];
    };
    // Update this version number if you update anything in the class!
    static constexpr uint32_t VERSION = 1;

    // Here are the getters
    uint64_t get_timestamp() const  { return body_()->timestamp;  }
    uint32_t get_nhits() const    { return body_()->nhits;    }
    const dune::TriggerPrimitive& get_primitive(int i) const { return body_()->primitives[i]; }
    dune::TriggerPrimitive& get_primitive(int i) { return body_()->primitives[i]; }

    void set_timestamp(uint64_t timestamp) { body_()->timestamp=timestamp;  }
    void set_nhits(uint32_t nhits) {  body_()->nhits=nhits;    }

protected:
    // This function allows the getter classes below to be simple.  
    // It is modelled after header_() in ToyFragment.hh
    // header_() in ToyFragment.hh is protected as well.
    Body const * body_() const {
        return reinterpret_cast<FelixHitFragment::Body const *>( artdaq_Fragment_.dataBeginBytes());
    }
    Body * body_() {
        return reinterpret_cast<FelixHitFragment::Body*>( artdaq_Fragment_.dataBeginBytes());
    }

private:
    artdaq::Fragment& artdaq_Fragment_;
};

#endif /* dune_artdaq_Overlays_FelixHitFragment_hh */
