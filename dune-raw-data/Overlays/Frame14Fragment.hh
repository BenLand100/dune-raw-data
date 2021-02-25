// Frame14Fragment.hh edited from FelixFragment.hh by Ben Land (2020)

#ifndef artdaq_dune_Overlays_Frame14Fragment_hh
#define artdaq_dune_Overlays_Frame14Fragment_hh

#include "FragmentType.hh"
#include "artdaq-core/Data/Fragment.hh"
#include "dune-raw-data/Overlays/Frame14Format.hh"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <iostream>
#include <map>
#include <vector>
#include <zlib.h>

namespace dune {

//============================
// Frame14 fragment base class
//============================
class Frame14Fragment {
 public:
  /* Struct to hold Frame14Fragment Metadata copied directly from FelixFragment. */
  struct Metadata {
    typedef uint32_t data_t;
    
    data_t control_word : 12, version : 4, reordered : 8, compressed : 8;
    data_t num_frames;
    data_t offset_frames;
    data_t window_frames;

    bool operator==(const Metadata& rhs) {
      return (control_word == rhs.control_word) &&
             (version == rhs.version) &&
             (reordered == rhs.reordered) &&
             (compressed == rhs.compressed) &&
             (num_frames == rhs.num_frames) &&
             (offset_frames == rhs.offset_frames) &&
             (window_frames == rhs.window_frames);
    }
  };
  
  typedef uint16_t adc_t;
  typedef std::vector<uint16_t> adc_v;

  /* Frame field and accessors. */
  virtual uint8_t link_mask(const unsigned& frame_ID = 0) const = 0;
  virtual uint8_t femb_valid(const unsigned& frame_ID = 0) const = 0;
  virtual uint8_t fiber_no(const unsigned& frame_ID = 0) const = 0;
  virtual uint8_t slot_no(const unsigned& frame_ID = 0) const = 0;
  virtual uint8_t crate_no(const unsigned& frame_ID = 0) const = 0;
  virtual uint8_t frame_version(const unsigned& frame_ID = 0) const = 0;
  
  virtual uint32_t wib_data(const unsigned& frame_ID = 0) const = 0;
  virtual uint64_t timestamp(const unsigned& frame_ID = 0) const = 0;
  virtual uint32_t crc20(const unsigned& frame_ID = 0) const = 0;
  virtual uint32_t flex12(const unsigned& frame_ID = 0) const = 0;
  virtual uint32_t flex24(const unsigned& frame_ID = 0) const = 0;
  
  virtual uint8_t loss_of_lock(const unsigned& frame_ID = 0) const {
    throw cet::exception("Frame14Fragment") << "loss_of_lock not implemented\n";
  }
  virtual uint8_t link_timestamp(const unsigned& frame_ID, 
                                  const uint8_t block_ID, 
                                  const uint8_t link_ID) const {
    throw cet::exception("Frame14Fragment") << "link_timestamp not implemented\n";
  }

  // Functions to return a certain ADC value.
  virtual adc_t get_ADC(const unsigned& frame_ID, const uint8_t block_ID,
                        const uint8_t channel_ID) const = 0;
  virtual adc_t get_ADC(const unsigned& frame_ID,
                        const uint8_t channel_ID) const = 0;

  // Function to return all ADC values for a single channel.
  virtual adc_v get_ADCs_by_channel(const uint8_t block_ID,
                                    const uint8_t channel_ID) const = 0;
  virtual adc_v get_ADCs_by_channel(const uint8_t channel_ID) const = 0;
  
  // Function to return all ADC values for all channels in a map.
  virtual std::map<uint8_t, adc_v> get_all_ADCs() const = 0;

  Frame14Fragment(const artdaq::Fragment& fragment)
      : meta_(*(fragment.metadata<Metadata>())),
        artdaq_Fragment_(fragment.dataBeginBytes()),
        sizeBytes_(fragment.dataSizeBytes()) { }
  virtual ~Frame14Fragment() {}
  
  // The number of frames in the current event.
  virtual size_t total_frames() const = 0;


 protected:
  Metadata meta_;
  const void* artdaq_Fragment_;
  size_t sizeBytes_;
}; // class dune::FrameFragmentBase

//==================================================
// Frame fragment for an array of bare frames
//==================================================
class Frame14FragmentUnordered : public Frame14Fragment {
 public:
  /* Frame field and accessors. */
  uint8_t link_mask(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->link_mask;
  }
  uint8_t femb_valid(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->femb_valid;
  }
  uint8_t fiber_no(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->fiber_num;
  }
  uint8_t slot_no(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->slot_num;
  }
  uint8_t crate_no(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->crate_num;
  }
  uint8_t frame_version(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->frame_version;
  }
  
  uint32_t wib_data(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->wib_data;
  }
  uint64_t timestamp(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->timestamp;
  }
  uint32_t crc20(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->crc20;
  }
  uint32_t flex12(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->flex12;
  }
  uint32_t flex24(const unsigned& frame_ID = 0) const {
    return frame_(frame_ID)->flex24;
  }

  size_t total_frames() const {
    return meta_.num_frames; 
  }
  
  // Functions to return a certain ADC value.
  adc_t get_ADC(const unsigned& frame_ID, const uint8_t block_ID,
                const uint8_t channel_ID) const {
    if (block_ID == 0) {
      return frame14::unpack14(frame_(frame_ID)->femb_a_seg,channel_ID);
    } else if (block_ID == 1) {
      return frame14::unpack14(frame_(frame_ID)->femb_b_seg,channel_ID);
    } else {
      throw cet::exception("Frame14FragmentUnordered") << "invalid block_ID\n";
    }
  }
  adc_t get_ADC(const unsigned& frame_ID, const uint8_t channel_ID) const {
    return get_ADC(frame_ID,channel_ID/128,channel_ID%128);
  }

  // Function to return all ADC values for a single channel.
  adc_v get_ADCs_by_channel(const uint8_t block_ID,
                            const uint8_t channel_ID) const {
    adc_v output(total_frames());
    for (size_t i = 0; i < total_frames(); i++) {
      output[i] = get_ADC(i, block_ID, channel_ID);
    }
    return output;
  }
  adc_v get_ADCs_by_channel(const uint8_t channel_ID) const {
    adc_v output(total_frames());
    for (size_t i = 0; i < total_frames(); i++) {
      output[i] = get_ADC(i, channel_ID);
    }
    return output;
  }
  // Function to return all ADC values for all channels in a map.
  std::map<uint8_t, adc_v> get_all_ADCs() const {
    std::map<uint8_t, adc_v> output;
    for (int i = 0; i < 256; i++)
      output.insert(std::pair<uint8_t, adc_v>(i, get_ADCs_by_channel(i)));
    return output;
  }

  Frame14FragmentUnordered(artdaq::Fragment const& fragment)
      : Frame14Fragment(fragment) {}

 protected:
  // Allow access to individual frames according to the dune::frame14::frame14 structure.
  frame14::frame14 const* frame_(const unsigned& frame_num = 0) const {
    return static_cast<frame14::frame14 const*>(artdaq_Fragment_) + frame_num;
  }
}; // class dune::Frame14FragmentUnordered

class Frame14FragmentV2Unordered : public Frame14FragmentUnordered {
 public:
  /* Frame field and accessors. */

  
  uint32_t wib_data(const unsigned& frame_ID = 0) const {
    throw cet::exception("Frame14FragmentV2Unordered") << "wib_data no longer exists\n";
  }
  uint32_t crc20(const unsigned& frame_ID = 0) const {
    return frame_v2_(frame_ID)->crc20;
  }
  uint32_t flex12(const unsigned& frame_ID = 0) const {
    throw cet::exception("Frame14FragmentV2Unordered") << "flex12 no longer exists\n";
  }
  uint32_t flex24(const unsigned& frame_ID = 0) const {
    throw cet::exception("Frame14FragmentV2Unordered") << "flex24 no longer exists\n";
  }
  
  uint8_t loss_of_lock(const unsigned& frame_ID = 0) const {
    return frame_v2_(frame_ID)->loss_of_lock;
  }
  uint8_t link_timestamp(const unsigned& frame_ID, 
                                  const uint8_t block_ID, 
                                  const uint8_t link_ID) const {
    if (block_ID == 0) {
      switch (link_ID) {
        case 0:
          return frame_v2_(frame_ID)->femb_a_link_0_ts;
        case 1:
          return frame_v2_(frame_ID)->femb_a_link_1_ts;
        case 2:
          return frame_v2_(frame_ID)->femb_a_link_2_ts;
        case 3:
          return frame_v2_(frame_ID)->femb_a_link_3_ts;
        default:
          throw cet::exception("Frame14FragmentV2Unordered") << "invalid link_ID\n";
      }
    } else if (block_ID == 1) {
      switch (link_ID) {
        case 0:
          return frame_v2_(frame_ID)->femb_b_link_0_ts;
        case 1:
          return frame_v2_(frame_ID)->femb_b_link_1_ts;
        case 2:
          return frame_v2_(frame_ID)->femb_b_link_2_ts;
        case 3:
          return frame_v2_(frame_ID)->femb_b_link_3_ts;
        default:
          throw cet::exception("Frame14FragmentV2Unordered") << "invalid link_ID\n";
      }
    } else {
      throw cet::exception("Frame14FragmentV2Unordered") << "invalid block_ID\n";
    }
  }

  Frame14FragmentV2Unordered(artdaq::Fragment const& fragment)
      : Frame14FragmentUnordered(fragment) {}

 protected:
  // Allow access to individual frames according to the dune::frame14::frame14_v2 structure.
  frame14::frame14_v2 const* frame_v2_(const unsigned& frame_num = 0) const {
    return static_cast<frame14::frame14_v2 const*>(artdaq_Fragment_) + frame_num;
  }
}; // class dune::Frame14FragmentV2Unordered

const Frame14Fragment* ParseFrame14Fragment(const artdaq::Fragment& fragment) {
  frame14::frame14 const* frame = (frame14::frame14 const*)(fragment.dataBeginBytes());
  switch (frame->frame_version) {
    case 1:
      return new Frame14FragmentUnordered(fragment);
    case 2:
      return new Frame14FragmentV2Unordered(fragment);
    default:
      return new Frame14FragmentUnordered(fragment);
  }
}

}

#endif /* artdaq_dune_Overlays_Frame14Fragment_hh */
