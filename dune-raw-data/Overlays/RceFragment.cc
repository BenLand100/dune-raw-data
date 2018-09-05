#include "dune-raw-data/Overlays/RceFragment.hh"

#include "dam/HeaderFragmentUnpack.hh"
#include "dam/TpcStreamUnpack.hh"

#include "artdaq-core/Data/ContainerFragment.hh"

#include <iostream>
#include <fstream>

dune::RceFragment::RceFragment(const uint64_t* data_ptr) :
   _data_ptr(data_ptr)
{
   _init();
}

dune::RceFragment::RceFragment(artdaq::Fragment const& afrag) :
   _data_ptr(_align(afrag.dataBeginBytes()))
{
   _init();
}

void dune::RceFragment::_init()
{
   // Check header and trailer
   if (!_data_ptr) {
      std::cerr << "RceFragment(nullptr)" << std::endl;
      return;
   }
   
   uint64_t header  = *_data_ptr;
   size_t   n_words = (header >> 8) & 0xffffff;
   if ((header >> 40) != PATTERN_WORD) {
      std::cerr << "invalid header "
         << std::hex << header
         << std::dec << std::endl;
      return;
   }

   uint64_t trailer = *(_data_ptr + n_words - 1);
   if (trailer != ~header) {
      std::cerr << "invalid trailer " 
         << std::hex << trailer
         << std::dec << std::endl;
   }

   // Allow user to look at each stream (could be damaged)
   HeaderFragmentUnpack const hdr(_data_ptr);
   if ( hdr.isData() )
   {
      _data_fragment = std::make_unique<DataFragmentUnpack>(_data_ptr);
      _tpc_fragment = std::make_unique<TpcFragmentUnpack>(*_data_fragment);
      _n_streams = _tpc_fragment->getNStreams();

      for (size_t i = 0; i < _n_streams; ++i) {
         auto* stream = get_stream(i);
         // FIXME check individual stream
         // 2018-08-23 checking stream->isTpcNormal() is not available
         if (_data_fragment->isTpcNormal())
            _streams.push_back(stream);
      }
   }
}

TpcStreamUnpack const *
dune::RceFragment::get_stream(size_t i) const
{
   if (_n_streams <= 0)
      return nullptr;
   return _tpc_fragment->getStream(i);
}

void dune::RceFragment::hexdump(std::ostream& out, int n_words) const
{
   if (!_data_ptr)
      return;

   out << std::hex;
   for (int i = 0; i < n_words; ++i)
      out << *(_data_ptr + i) << " ";
   out << std::dec << std::endl;
}

void dune::RceFragment::save(const std::string& filepath) const
{
   if (!_data_ptr) {
      std::cerr << "RceFragment(nullptr)" << std::endl; 
      return;
   }

   uint64_t header = *_data_ptr;
   if ((header >> 40) != PATTERN_WORD) {
      std::cerr << "Invalid header" << std::endl; 
      return;
   }
 
   size_t bytes = (header >> 8) & 0xffffff;
   bytes *= sizeof(uint64_t);

   std::ofstream b_stream(filepath.c_str(),
         std::fstream::out | std::fstream::binary);

   char const* data = reinterpret_cast<decltype(data)>(_data_ptr);
   b_stream.write(data, bytes);
   b_stream.close();
}

// Create std::vector<RceFragment> from ContainerFragment
// A hack to avoid copying when creating artdaq::Fragment
dune::RceFragments
dune::RceFragment::from_container_frags(const artdaq::Fragments& frags)
{
   RceFragments rces;

   for (auto const& frag: frags) {
      artdaq::ContainerFragment cfrag(frag);
		for (size_t ii = 0 ; ii < cfrag.block_count(); ++ii) {
         auto* data_ptr = (uint8_t*) (cfrag.dataBegin());
 			data_ptr += cfrag.fragmentIndex(ii);

         // just copy a few bytes to get the header size of artdaq::Fragment 
 			size_t afrag_size = 128;
			artdaq::Fragment afrag;
			afrag.resizeBytes(afrag_size);
			memcpy(afrag.headerAddress(), data_ptr, afrag_size);

         // offset for artdaq::Fragment header, meta ...
         data_ptr += afrag.dataBeginBytes() - afrag.headerBeginBytes();

         dune::RceFragment rce(_align(data_ptr));
         rces.push_back(std::move(rce)); 
      }
   }
   return rces;
}

const uint64_t *
dune::RceFragment::_align(const uint8_t *ptr)
{
   // Align data pointer to the begining of uint64_t
   size_t max_words = sizeof(uint64_t) / sizeof(artdaq::Fragment::byte_t);
   for (size_t i = 0; i < max_words; ++i) {
      auto *header = reinterpret_cast<const uint64_t*>(ptr + i);
      if ((*header >> 40) == PATTERN_WORD)
         return header;
   }

   // Backward compatibility for obselete meta from 35-tons
   // An unsed 12 words leftover from the millislices era
   {
      auto *header = reinterpret_cast<const uint64_t*>(ptr + 12);
      if ((*header >> 40) == PATTERN_WORD)
         return header;
   }

   // No valid header found
   // return the orignal ptr
   // _init method will take care of data w/ invalid header
   return reinterpret_cast<const uint64_t*>(ptr);
}
