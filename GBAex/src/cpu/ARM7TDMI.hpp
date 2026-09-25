#pragma once
#include<cstdint>
#include<array>
class ARM7TDMI{
    public:
        enum Mode {
            USER,
            FIQ,
            IRQ,
            SUPERVISOR,
            ABORT,
            UNDEFINED,
            SYSTEM,
            MODE_COUNT
        };

        std::array<uint32_t,16> r{};
        /*
        r0-r12: General purpose registers, for use in every day operations
        r13 (SP): Stack pointer Register.
        r14 (LR): Link Register. Used primarily to store the address following a “bl” (branch and link) instruction (as used in function calls)
        r15 (PC): The Program Counter. Because the ARM7tdmi uses a 3-stage pipeline, this register always contains an address which is 2 instructions ahead of the one currrently being executed. In 32-bit ARM state, it is 8 bytes ahead.
        */

        // r8-r12 banked copies: ONLY FIQ has its own — every other mode
        // shares the r[8..12] slots above. Indexed 0-4 for r8-r12.
        std::array<uint32_t,5> r8_12Fiq{};

        // r13 (SP) and r14 (LR) banked copies: every privileged mode gets
        // its own, indexed by Mode. USER and SYSTEM share the plain r[13]/r[14].
        std::array<uint32_t,MODE_COUNT> r13Banked{};
        std::array<uint32_t,MODE_COUNT> r14Banked{};

        uint32_t cpsr{};

        // Saved CPSR per privileged mode — filled automatically by hardware
        // on exception entry, restored on return. USER/SYSTEM never get one
        // (not privileged, no exception entry into them).
        std::array<uint32_t,MODE_COUNT> spsr{};

        Mode currentMode = USER;

};