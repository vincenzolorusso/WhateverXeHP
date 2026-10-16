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
 * Hook di bypass per l'accelerazione eDP:
 * Il driver nativo legge le porte in sola lettura (ProbeIO).
 * Forziamo la risposta software a "1U" (Connesso) per DDI 0, 
 * costringendo l'acceleratore 3D a registrarsi al servizio.
 */
template<class IO> uint32_t tglMode(IO &io, uint32_t ddi) {
    // Forza la connessione software per la porta DDI 0 (Schermo Interno Laptop)
    if (ddi == 0) {
        return 1U;
    }

    // Per le altre porte fisiche (HDMI/DP), applica la maschera estesa
    constexpr uint32_t admitted = (1U << 0) | (1U << 1) | (1U << 4);
    uint32_t mode = XELP::legacyTGLMode(io, ddi, admitted);
    
    return (mode != 0) ? mode : 1U;
}

} // namespace XELPPortProbe
#endif
