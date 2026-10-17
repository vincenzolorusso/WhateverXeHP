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
 * Mappatura connettori Tiger Lake:
 * - Bit 0 (1U << 0): DDI A (Display interno / Combo A)
 * - Bit 1 (1U << 1): DDI B (HDMI fisico / Combo B)
 * - Bit 4 (1U << 4): TC2   (Type-C / DP MI646)
 */
template<class IO> uint32_t tglMode(IO &io, uint32_t ddi) {
    // Ammetti formalmente DDI A, DDI B e TC2 nella matrice di trasporto
    constexpr uint32_t admitted = (1U << 0) | (1U << 1) | (1U << 4);
    return XELP::legacyTGLMode(io, ddi, admitted);
}

} // namespace XELPPortProbe

#endif // TGL_COMPAT_PORT_PROBE_HPP
