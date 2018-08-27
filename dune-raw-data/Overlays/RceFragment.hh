#ifndef artdaq_dune_Overlays_RceFragment_hh
#define artdaq_dune_Overlays_RceFragment_hh

#include <memory>
#include <string>
#include <ostream>
#include "dam/DataFragmentUnpack.hh"
#include "dam/TpcFragmentUnpack.hh"
#include "artdaq-core/Data/Fragment.hh"

namespace dune
{
    class RceFragment;
    typedef std::vector<dune::RceFragment> RceFragments;
}

class TpcStreamUnpack;

class dune::RceFragment
{
    public:
       static const uint64_t PATTERN_WORD = 0x8b309e;

       RceFragment(artdaq::Fragment const & fragment );
       RceFragment(const uint64_t* data_ptr);
       int size() const { return _n_streams; }
       TpcStreamUnpack const * get_stream(size_t i) const;
       void hexdump(std::ostream& out, int n_words=10) const;
       void save(const std::string& filepath) const;
       const uint64_t* data_ptr() const { return _data_ptr; }

       // quick access to good streams
       // user can still access individual stream (good or bad)
       // via get_stream(size_t i)
       const std::vector<TpcStreamUnpack const *> & get_streams() const {
          return _streams;
       }

       static RceFragments from_container_frags(const artdaq::Fragments& frags);

    private:
       void _init();
       static const uint64_t * _align(const uint8_t *ptr);

       std::unique_ptr<DataFragmentUnpack> _data_fragment;
       std::unique_ptr<TpcFragmentUnpack> _tpc_fragment;
       const uint64_t* _data_ptr;

       size_t _n_streams = 0;
       std::vector<TpcStreamUnpack const *> _streams;
};
#endif 
