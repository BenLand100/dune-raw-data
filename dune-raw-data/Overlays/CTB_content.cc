#include "dune-raw-data/Overlays/CTB_content.h"

#include <cstddef>

std::set<unsigned short> ptb::content::word::trigger_t::Triggers( size_t max_bit ) const {

  std::set<unsigned short> trs;

  mask_size_t mask = 0x1 ;
  
  for ( size_t i = 0 ; i < max_bit ; ++i ) {
    if ( trigger_word & (mask << i) ) {
      trs.insert( i ) ;
    }
  }
  
  return trs ;

}
