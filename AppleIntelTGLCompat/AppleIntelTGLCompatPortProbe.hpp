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
 * Hook di Forzatura DDI A / eDP:
 * Il driver donor per Mac mini scarta DDI 0 se non trova HPD esterno.
 * Forziamo il ritorno del bit 0 (DDI A) attivo e abilitato come Combo Transport.
 */
template<class IO> uint32_t tglMode(IO &io, uint32_t ddi) {
    // Se la richiesta è per la porta primaria (DDI 0 / DDI A)
    if (ddi == 0) {
        // Restituisce 1 per forzare l'adozione della pipe GOP iniziale
        return 1U;
    }

    // Ammetti DDI A (1U<<0) e DDI B (1U<<1)
    constexpr uint32_t laptopTopologyMask = (1U << 0) | (1U << 1);
    uint32_t mode = XELP::legacyTGLMode(io, ddi, laptopTopologyMask);

    return (mode != 0) ? mode : 1U;
}

} // namespace XELPPortProbe
#endif
