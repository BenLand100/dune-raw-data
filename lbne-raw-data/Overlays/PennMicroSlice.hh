#ifndef lbne_artdaq_Overlays_PennMicroSlice_hh
#define lbne_artdaq_Overlays_PennMicroSlice_hh

#include <stdint.h>
#include <stddef.h>

namespace lbne {
  class PennMicroSlice;
}

class lbne::PennMicroSlice {

public:

  struct Header {

    typedef uint32_t data_t;
    
    typedef uint8_t  format_version_t;
    typedef uint8_t  sequence_id_t;
    typedef uint16_t block_size_t;

    format_version_t format_version : 8;
    sequence_id_t    sequence_id    : 8;
    block_size_t     block_size     : 16;

    static size_t const size_words = sizeof(data_t);

    //static constexpr size_t raw_header_words = 1;
    //data_t raw_header_data[raw_header_words];
  };

  struct Payload_Header {

    typedef uint32_t data_t;

    typedef uint8_t  data_packet_type_t;
    typedef uint32_t short_nova_timestamp_t;

    short_nova_timestamp_t short_nova_timestamp : 28;
    data_packet_type_t     data_packet_type     : 4;

    static size_t const size_words = sizeof(data_t);
  };

  typedef Header::block_size_t microslice_size_t;

  //the size of the payloads (neglecting the Payload_Header)
  static microslice_size_t const payload_size_counter   = 3 * sizeof(uint32_t); //96-bit payload
  static microslice_size_t const payload_size_trigger   = 1 * sizeof(uint32_t); //32-bit payload
  static microslice_size_t const payload_size_timestamp = 2 * sizeof(uint32_t); //64-bit payload


  // This constructor accepts a memory buffer that contains an existing
  // microSlice and allows the the data inside it to be accessed
  PennMicroSlice(uint8_t* address);

  // Get the contents of a payload
  uint8_t* get_payload(uint32_t word_id, Payload_Header::data_packet_type_t& data_packet_type, Payload_Header::short_nova_timestamp_t& short_nova_timestamp, size_t& size, bool swap_payload_header_bytes, size_t override_uslice_size = 0) const;

  // Returns the format version field from the header
  Header::format_version_t format_version() const;

  // Returns the sequence ID field from the header
  Header::sequence_id_t sequence_id() const;

  // Returns the block size field from the header
  Header::block_size_t block_size() const;

  // Returns the size of the PennMicroSlice
  lbne::PennMicroSlice::microslice_size_t size() const;

  // Returns the number of samples in the microslice
  typedef uint32_t sample_count_t;
  sample_count_t sampleCount(sample_count_t &n_counter_words, sample_count_t &n_trigger_words, sample_count_t &n_timestamp_words, bool swap_payload_header_bytes, size_t override_uslice_size = 0) const;

  // Returns a pointer to the first payload_header in data that has a time after boundary_time (returns 0 if that doesn't exist)
  // also sets remaining_size - the size of data that is after boundary_time
  uint8_t* sampleTimeSplit(uint64_t boundary_time, size_t& remaining_size, bool swap_payload_header_bytes, size_t override_uslice_size = 0) const;

  // Returns a pointer to the first payload_header in data that has a time after boundary_time (returns 0 if that doesn't exist)
  // also sets remaining_size - the size of data that is after boundary_time
  // and counts payloads before & after the time
  //NOTE this is prefered to calling sampleCount() and sampleTimeSplit() separately, as it loops through the data exactly once (instead of between once & exactly twice)
  uint8_t* sampleTimeSplitAndCount(uint64_t boundary_time, size_t& remaining_size,
				   sample_count_t &n_words_b, sample_count_t &n_counter_words_b, sample_count_t &n_trigger_words_b, sample_count_t &n_timestamp_words_b,
				   sample_count_t &n_words_a, sample_count_t &n_counter_words_a, sample_count_t &n_trigger_words_a, sample_count_t &n_timestamp_words_a,
				   bool swap_payload_header_bytes, size_t override_uslice_size = 0) const;

  // As sampleTimeSplitAndCount, but also gets information for a second time (overlap_time), in the same loop
  uint8_t* sampleTimeSplitAndCountTwice(uint64_t boundary_time, size_t& remaining_size,
					uint64_t overlap_time,  size_t& overlap_size,   uint8_t*& overlap_data_ptr,
					sample_count_t &n_words_b, sample_count_t &n_counter_words_b, sample_count_t &n_trigger_words_b, sample_count_t &n_timestamp_words_b,
					sample_count_t &n_words_a, sample_count_t &n_counter_words_a, sample_count_t &n_trigger_words_a, sample_count_t &n_timestamp_words_a,
					sample_count_t &n_words_o, sample_count_t &n_counter_words_o, sample_count_t &n_trigger_words_o, sample_count_t &n_timestamp_words_o,
					bool swap_payload_header_bytes, size_t override_uslice_size = 0) const;

  //Values used to handle the rollover.
  //The width of the millislice should be:
  // LESS than the ROLLOVER_LOW_VALUE
  // LESS than (max of a uint28_t - ROLLOVER_HIGH_VALUE)
  static const uint32_t ROLLOVER_LOW_VALUE  = 1 << 13; //8192 ticks = 0.128ms
  static const uint32_t ROLLOVER_HIGH_VALUE = 1 << 26;

  //The types of data words
  static const Payload_Header::data_packet_type_t DataTypeCounter   = 0x01; //0b0001
  static const Payload_Header::data_packet_type_t DataTypeTrigger   = 0x02; //0b0010
  static const Payload_Header::data_packet_type_t DataTypeTimestamp = 0x08; //0b1000

  static uint64_t getMask(int param){
	uint64_t mask=0;
	mask = (1 << param) - 1;//sets the mask to 0000...11111...11
	return mask;
  };

  uint32_t* raw() const;

protected:

  // returns a pointer to the header
  Header const* header_() const;

  // returns a pointer to the first sample word
  uint32_t const* data_() const;

  uint8_t* buffer_;
};

#endif /* lbne_artdaq_Overlays_PennMicroSlice_hh */