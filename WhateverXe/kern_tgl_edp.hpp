// SPDX-License-Identifier: BSD-2-Clause
// Supporto sperimentale eDP (Embedded DisplayPort) per Intel Tiger Lake Xe-LP.
#ifndef KERN_TGL_EDP_HPP
#define KERN_TGL_EDP_HPP

#include <stdint.h>

namespace TGLEDP {

// Registri Display Engine Gen12 (Tiger Lake / Display13)
namespace Registers {
    // Panel Power Sequencing (PPS) Controller 1
    constexpr uint32_t PP_STATUS_1      = 0x61200;
    constexpr uint32_t PP_CONTROL_1     = 0x61204;
    constexpr uint32_t PP_ON_DELAYS_1   = 0x61208;
    constexpr uint32_t PP_OFF_DELAYS_1  = 0x6120C;
    
    // Backlight Controller PWM (PCH Display)
    constexpr uint32_t BXT_BLC_PWM_CTL1 = 0x61250;
    constexpr uint32_t BXT_BLC_PWM_FREQ = 0x61254;

    // Maschere Bit PP_CONTROL
    constexpr uint32_t PANEL_POWER_ON   = (1U << 0);
    constexpr uint32_t EDP_FORCE_VDD    = (1U << 3);
    constexpr uint32_t PP_UNLOCK_KEY    = 0xABCD0000; // Chiave di sblocco scrittura registri PPS

    // Maschere Bit PP_STATUS
    constexpr uint32_t PP_STATUS_ON     = (1U << 31);
    
    // Maschera Bit Retroilluminazione
    constexpr uint32_t BLC_PWM_ENABLE   = (1U << 31);
}

/**
 * Inizializza l'alimentazione del pannello eDP (Panel Power Sequencing).
 * Invia corrente al circuito VDD del display LCD e attende che il pannello sia pronto.
 */
template <typename IO>
inline bool powerOnPanel(IO &io, unsigned timeoutMs = 250) {
    uint32_t status = io.read32(Registers::PP_STATUS_1);
    
    // Se il pannello è già alimentato dal BIOS/GOP, confermiamo e usciamo
    if (status & Registers::PP_STATUS_ON) {
        return true;
    }

    // Forza l'alimentazione VDD e attiva il target di accensione
    uint32_t control = io.read32(Registers::PP_CONTROL_1);
    control &= ~0xFFFF0000; // Pulisci chiavi precedenti
    control |= Registers::PP_UNLOCK_KEY | Registers::PANEL_POWER_ON | Registers::EDP_FORCE_VDD;
    io.write32(Registers::PP_CONTROL_1, control);

    // Attendi che il rail di alimentazione hardware raggiunga lo stato stabile
    unsigned elapsed = 0;
    while (elapsed < timeoutMs) {
        status = io.read32(Registers::PP_STATUS_1);
        if (status & Registers::PP_STATUS_ON) {
            return true;
        }
        io.delayUs(1000); // 1 ms
        elapsed += 1;
    }

    return false;
}

/**
 * Forza l'attivazione della retroilluminazione e del ciclo PWM.
 */
template <typename IO>
inline void forceBacklight(IO &io, uint32_t dutyCycle = 0x0000FFFF) {
    uint32_t pwmCtl = io.read32(Registers::BXT_BLC_PWM_CTL1);
    pwmCtl |= Registers::BLC_PWM_ENABLE;
    io.write32(Registers::BXT_BLC_PWM_CTL1, pwmCtl);

    // Imposta una frequenza base e un ciclo di lavoro se azzerato
    uint32_t freq = io.read32(Registers::BXT_BLC_PWM_FREQ);
    if (!freq) {
        io.write32(Registers::BXT_BLC_PWM_FREQ, dutyCycle);
    }
}

/**
 * Valida se il DDI richiesto corrisponde a DDI A (Combo PHY A / eDP interno).
 */
inline bool isInternalDDI(uint32_t ddi) {
    return ddi == 0; // DDI 0 = Port A = eDP nativo
}

} // namespace TGLEDP

#endif // KERN_TGL_EDP_HPP
