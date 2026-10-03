## get a ROM loaded and bytes reachable
1. ~arm and memory class creation~.
2. ~ROM/Cartridge loader — reads a `.gba` file off disk into a `std::vector<uint8_t>`, read-only.~
3. Wire ROM into the address space — CPU accesses at `0x08000000+` need to reach that loaded data. Either `MEMORY` gets a reference/pointer to the `Cartridge` and routes `0x08` addresses to it, or the CPU itself checks "is this a ROM address?" before going to `MEMORY` at all — pick one design.
4. Minimal BIOS stand-in (HLE) — not a real BIOS, just: define what's at the interrupt vectors, and handle the handful of `SWI` calls real ROMs actually use (division, memset/memcpy, etc.) as direct C++ functions instead of real BIOS code. Covered a couple turns back — skip this and real commercial ROMs will likely break on first `SWI`.

## ARM class, functional pieces
5. CPSR flag read/set helpers.
6. Mode switching logic — actually swaps banked SP/LR/SPSR based on `currentMode`.
7. Instruction fetch — read from `MEMORY`/ROM at `r[15]`.
8. ARM-state decoder (32-bit instruction encoding).
9. THUMB-state decoder (16-bit instruction encoding) — separate from #7.
10. Instruction execution — start with a small working subset (data processing, branch, basic load/store), expand from there.
11. Barrel shifter logic — needed once you hit instructions that use shifted operands.
12. Pipeline-correct PC behavior (+8 ARM / +4 THUMB reads).
13. Exception entry handling — automatic SPSR save + mode switch + vector jump on IRQ/SWI.

## memory, finishing touches
14. ~Make `MEMORY`'s six arrays `private`, now that Read/Write methods exist.~
15. Per-access cycle cost (ties into step 9 — once instructions are executing, you need timing to go with them).

## PPU, timers, DMA, input
16. These haven't been designed yet — they're what let the CPU's output actually become a visible frame, and what your OS's VBlank-driven scheduler (discussed earlier) will hook into. Worth scoping once Phase 2 execution is working, not before.

## OS ROM
17. Everything from the earlier discussion — scheduler, task context save/restore, cooperative handoff into the game — sits on top of Phase 2's exception handling (#12) and Phase 4's VBlank interrupt. Can't meaningfully start until those exist.
