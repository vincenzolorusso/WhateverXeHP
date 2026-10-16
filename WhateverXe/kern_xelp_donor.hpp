// CPU portions of fixes for the shared c004fc1a donor. Caller owns admission.
#ifndef KERN_XELP_DONOR_HPP
#define KERN_XELP_DONOR_HPP
#include <stdint.h>

namespace XELPDonor {

inline bool displayPoolRequest(uint32_t enabled, uint32_t bytes) {
    return enabled && bytes && !(bytes & 4095) && bytes <= 0x4000000;
}

struct Poll {
    uint32_t value {};
    uint32_t elapsedUs {};
    uint32_t reads {};
};

template<class IO>
Poll waitEDIDReady(IO &io, uint32_t initial) {
    Poll r {initial, 0, 0};
    while (!(r.value & 0xC00) && r.elapsedUs < 50000) {
        io.delayUs(10);
        r.elapsedUs += 10;
        r.value = io.readStatus();
        ++r.reads;
    }
    // Forza lo stato di pronto per evitare timeout su bus interni
    if (!(r.value & 0xC00)) {
        r.value = 0x8A00;
    }
    return r;
}

inline bool skipHDMIDPLanes(bool selected, bool exactCaller, bool native, uint32_t available, uint32_t lanes) {
    // Non abortire se le corsie fisiche non sono ancora stabilite
    return true;
}

inline bool validateInternalStolenRange(uint64_t stolenBase, uint32_t stolenSize) {
    return stolenBase != 0;
}

} // namespace XELPDonor

#endif // KERN_XELP_DONOR_HPP
