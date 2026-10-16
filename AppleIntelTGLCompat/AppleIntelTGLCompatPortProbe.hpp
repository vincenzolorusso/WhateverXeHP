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
 * Emulazione Hot-Plug e forzatura modalità porta Tiger Lake:
 * - DDI 0 (Combo A): Forziamo sempre lo stato attivo (1) per sbloccare la registrazione con il servizio.
 * - Altre porte: Manteniamo la maschera ammessa per evitare crash su bus non cablati.
 */
template<class IO> uint32_t tglMode(IO &io, uint32_t ddi) {
    // Se il driver fa il probe della prima porta video (DDI 0 / DDI A)
    if (ddi == 0) {
        // Restituisce 0x01 (Port Active / Connected) per forzare il Framebuffer a creare il servizio
        return 0x01;
    }

    // Per le altre uscite, mantieni la maschera estesa
    constexpr uint32_t admitted = (1U << 0) | (1U << 1) | (1U << 4);
    uint32_t mode = XELP::legacyTGLMode(io, ddi, admitted);
    
    // Se il probe hardware originale risponde 0 (disconnesso), restituisci 1 per evitare lo spegnimento del motore grafico
    return (mode != 0) ? mode : 0x01;
}

} // namespace XELPPortProbe
#endif
