#ifndef dune_artdaq_Overlays_HitFragment_hh
#define dune_artdaq_Overlays_HitFragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"
#include "dune-raw-data/Overlays/FelixFormat.hh"
#include "dune-raw-data/Overlays/FelixHitFormat.hh"

#include <ostream>
#include <vector>

// Implementation of "HitFragment".
//==================================================
// FELIX fragment for an array of FELIX hits
//==================================================
namespace dune{
  
  class HitFragment {
  public:
    // The following structure is overlayed onto the data in the
    // fragment, starting at the beginning.
    struct Body {
      dune::TriggerPrimitive primitive;
    };
    
    // The constructor simply sets its const private member "artdaq_Fragment_"
    // to refer to the artdaq::Fragment object
    HitFragment(artdaq::Fragment& fragment) : artdaq_Fragment_(fragment) {}


    const dune::TriggerPrimitive& get_primitive() const { return body_()->primitive; }
    dune::TriggerPrimitive& get_primitive() { return body_()->primitive; }

  protected:
    // artdaq_Fragment_ already points to dataBeginBytes(), thanks to the FelixFragmentBase constructor
    Body const * body_() const {
      return reinterpret_cast<HitFragment::Body const *>( artdaq_Fragment_.dataBeginBytes());
    }
    Body * body_() {
      return reinterpret_cast<HitFragment::Body*>(artdaq_Fragment_.dataBeginBytes());
    }
  private:

    artdaq::Fragment & artdaq_Fragment_;

  }; // class dune::HitFragment
}
#endif /* dune_artdaq_Overlays_HitFragment_hh */
