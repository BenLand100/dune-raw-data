// Frame14Format.hh
// Ben Land (2020)

#ifndef artdaq_dune_Overlays_FrameFormat_hh
#define artdaq_dune_Overlays_FrameFormat_hh

#include <cstddef>
#include <stdint.h>
#include <vector>

namespace dune {

namespace frame14 {

// Words in the binary format of the Frame frame14 from the WIB
typedef struct {
    uint32_t start_frame;
    uint8_t crate_num : 8, frame_version: 4, slot_num : 3, fiber_num : 1;
    uint8_t femb_valid : 2, link_mask : 8, reserved : 6;
    uint32_t wib_data;
    uint64_t timestamp;
    uint32_t femb_a_seg[56];
    uint32_t femb_b_seg[56];
    uint32_t crc20 : 20, flex12 : 12;
    uint32_t eof: 8, flex24 : 24;
    uint32_t idle_frame;
} frame14;


// Return a single value from packed channel data (56 uint32 words from the WIB)
inline uint16_t unpack14(const uint32_t *packed, size_t i) {
    const size_t low_bit = i*14;
    const size_t low_word = low_bit / 32;
    const size_t high_bit = (i+1)*14-1;
    const size_t high_word = high_bit / 32;
    //printf("word %li :: low %li (%li[%li]) high %li (%li[%li])\n",i,low_bit,low_word,low_bit%32,high_bit,high_word,high_bit%32);
    if (low_word == high_word) { //all the bits are in the same word
        return (packed[low_word] >> (low_bit%32)) & 0x3FFF;
    } else { //some of the bits are in the next word
        size_t high_off = high_word*32-low_bit;
        //printf("pre_mask 0x%X post_mask 0x%X\n", (0x3FFF >> (14-high_off)), ((0x3FFF << high_off) & 0x3FFF) );
        uint16_t result = (packed[low_word] >> (low_bit%32)) & (0x3FFF >> (14-high_off));
        result |= (packed[high_word] << high_off) & ((0x3FFF << high_off) & 0x3FFF);
        return result;
    }
}

} // namespace frame14

} // namespace dune

#endif /* artdaq_dune_Overlays_Frame14Format_hh */
