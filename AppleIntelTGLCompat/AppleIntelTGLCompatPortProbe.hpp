// SPDX-License-Identifier: BSD-2-Clause
#ifndef TGL_COMPAT_PORT_PROBE_HPP
#define TGL_COMPAT_PORT_PROBE_HPP

#include <stdint.h>
#include <stddef.h>
#include "kern_xelp_probe.hpp"

namespace XELPPortProbe {
constexpr uint32_t ABI = 1, ADLNPorts = 3;

inline bool adln(uint32_t pci, uint8_t revision) {
    const auto p = XELP::identify(pci, revision);
    return p.family == XELP::Family::AlderLakeN && p.admitted();
}

/**
 * Funzione hardware di basso livello (MMIO) per accendere il pannello LCD.
 * Risolve il freeze forzando l'alimentazione (VDD) prima del Link Training.
 */
template<class IO>
void powerOnInternalPanel(IO &io) {
    // Registri PPS (Panel Power Sequencing) Gen12
    constexpr uint32_t PP_STATUS  = 0x61200;
    constexpr uint32_t PP_CONTROL = 0x61204;
    
    // 1. Verifica se il pannello e' gia' alimentato (Bit 31: Panel Power On Status)
    if (io.read(PP_STATUS) & 0x80000000U) return;

    // 2. Forza VDD (Bit 3) e Power Target On (Bit 0). Chiave di sblocco scrittura = 0xABCD0000
    uint32_t ctrl = io.read(PP_CONTROL) & 0x0000FFFFU;
    io.write(PP_CONTROL, 0xABCD0000U | ctrl | 0x09U);

    // 3. Attendi che il chip TCON del display risponda e sia pronto (timeout di ~300 ms)
    for (unsigned i = 0; i < 300; ++i) {
        if (io.read(PP_STATUS) & 0x80000000U) break;
        io.sleepMs(1);
    }
}

template<class IO> uint32_t tglMode(IO &io, uint32_t ddi) {
    if (ddi == 0) {
        // Accendi l'elettricità del display prima che il driver Apple lo interroghi
        powerOnInternalPanel(io);
        
        // Dichiara la porta connessa per far avviare l'acceleratore
        return 1U; 
    }

    constexpr uint32_t admitted = (1U << 0) | (1U << 1) | (1U << 4);
    uint32_t mode = XELP::legacyTGLMode(io, ddi, admitted);
    return (mode != 0) ? mode : 1U;
}

} // namespace XELPPortProbe
#endif
