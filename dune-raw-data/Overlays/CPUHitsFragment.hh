#ifndef artdaq_dune_Overlays_CPUHitsFragment_hh
#define artdaq_dune_Overlays_CPUHitsFragment_hh
#include "FragmentType.hh"
#include "artdaq-core/Data/Fragment.hh"

namespace dune
{
//==================================================
// FELIX fragment for an array of FELIX hits
//==================================================
    class CPUHitsFragment  {
    public:

        struct Metadata {
            typedef uint32_t data_t;

            Metadata(data_t v) : fragment_version(v) {}
            // "8 bits ought to be enough for anyone"
            data_t fragment_version : 8;
            // Give a name to the rest of the bits in case we ever want them for something else
            data_t unused : 24;

            static size_t const size_words = 1ul; // Units of Metadata::data_t
        };

        // The following structure is overlayed onto the data in the
        // fragment, starting at the beginning.
        struct Body {
            uint64_t timestamp;
            uint32_t window_offset;
            uint8_t fiber_no;
            uint8_t slot_no;
            uint8_t crate_no;
            uint32_t nhits;
            // We have an array of TriggerPrimitive of unknown size
            // beginning in memory at this point, so use a "flexible array
            // member". Not sure how to do this more nicely, when we have to
            // have contiguous memory for the fragment
            dune::TriggerPrimitive primitives[];
        };
    
        // Update this version number if you update anything in the class!
        static constexpr uint32_t VERSION = 1;

        // The constructor simply sets its const private member "artdaq_Fragment_"
        // to refer to the artdaq::Fragment object
        CPUHitsFragment(artdaq::Fragment& fragment)
            : artdaq_Fragment_(fragment)
        {}

        // -------------------------------------------------------
        // Getters

        uint64_t get_timestamp() const  { return body_()->timestamp;  }
        uint32_t get_window_offset() const  { return body_()->window_offset;  }
        uint8_t get_fiber_no() const { return body_()->fiber_no; }
        uint8_t get_slot_no() const { return body_()->slot_no; }
        uint8_t get_crate_no() const { return body_()->crate_no; }

        uint32_t get_nhits() const    { return body_()->nhits;    }
        const dune::TriggerPrimitive& get_primitive(int i) const { return body_()->primitives[i]; }
        dune::TriggerPrimitive& get_primitive(int i) { return body_()->primitives[i]; }

        // -------------------------------------------------------
        // Setters

        void set_timestamp(uint64_t timestamp) { body_()->timestamp=timestamp;  }
        void set_window_offset(uint32_t offset) { body_()->window_offset=offset;  }
        void set_fiber_no(uint8_t fiber_no) { body_()->fiber_no=fiber_no; }
        void set_slot_no(uint8_t slot_no) { body_()->slot_no=slot_no; }
        void set_crate_no(uint8_t crate_no) { body_()->crate_no=crate_no; }

        void set_nhits(uint32_t nhits) {  body_()->nhits=nhits;    }

        virtual ~CPUHitsFragment() {}

    protected:
        Body const * body_() const {
            return reinterpret_cast<dune::CPUHitsFragment::Body const *>( artdaq_Fragment_.dataBeginBytes() );
        }
        Body * body_() {
            return reinterpret_cast<dune::CPUHitsFragment::Body*>( artdaq_Fragment_.dataBeginBytes() );
        }

    private:
        artdaq::Fragment& artdaq_Fragment_;

    }; // class dune::CPUHitsFragment

} // end namespace dune

#endif // include guard
