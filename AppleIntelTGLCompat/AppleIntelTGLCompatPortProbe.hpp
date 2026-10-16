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
 * Gestione delle modalità di trasporto per porte Tiger Lake:
 * - Bit 0 (1U << 0): DDI A (Combo PHY A -> eDP Monitor Interno Laptop)
 * - Bit 1 (1U << 1): DDI B (Combo PHY B -> HDMI Fisico)
 * - Bit 4 (1U << 4): TC2   (Type-C / DP Legacy MI646)
 */
template<class IO> uint32_t tglMode(IO &io, uint32_t ddi) {
    // Se la richiesta è per la porta interna DDI 0 (eDP)
    if (ddi == 0) {
        // Modalità 0x02 identifica un connettore di tipo eDP integrato
        return 0x02;
    }
    
    // Per tutte le altre porte esterne, usa la maschera combinata
    constexpr uint32_t admittedMask = (1U << 0) | (1U << 1) | (1U << 4);
    return XELP::legacyTGLMode(io, ddi, admittedMask);
}

} // namespace XELPPortProbe
#endif
