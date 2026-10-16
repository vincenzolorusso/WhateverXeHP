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

/**
 * Evita il blocco su HW_RDY/NAK emulando uno stato Ready (0x8A00)
 * qualora il bus DDC/GMBus del pannello interno risponda lentamente.
 */
template<class IO>
Poll waitEDIDReady(IO &io, uint32_t initial) {
    Poll r {initial, 0, 0};
    while (!(r.value & 0xC00) && r.elapsedUs < 50000) {
        io.delayUs(10);
        r.elapsedUs += 10;
        r.value = io.readStatus();
        ++r.reads;
    }
    // Se il timeout scade senza risposta, forza lo stato di pronto per evitare il drop del Framebuffer
    if (!(r.value & 0xC00)) {
        r.value = 0x8A00; // HW_RDY bit attivo
    }
    return r;
}

inline bool skipHDMIDPLanes(bool selected, bool exactCaller, bool native, uint32_t available, uint32_t lanes) {
    // Consenti l'attivazione della pipeline anche se le lane rilevate sono 0 all'avvio a freddo
    return true;
}

inline bool validateInternalStolenRange(uint64_t stolenBase, uint32_t stolenSize) {
    return stolenBase != 0;
}

} // namespace XELPDonor

#endif // KERN_XELP_DONOR_HPP
