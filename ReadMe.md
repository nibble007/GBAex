# GBA Emulator + OS ROM

## Overview

A cycle-accurate Game Boy Advance emulator written in C++, paired with a custom bare-metal OS ROM that boots and runs on the emulated hardware. This targets the GBA only — no GBC/DMG backward-compatibility mode is implemented, since the GBA's BIOS-level GBC compatibility runs on a completely different CPU (the Sharp SM83, not ARM) and is out of scope.

The project has two halves:
1. **Emulator core** — emulates the ARM7TDMI CPU, memory bus, PPU, DMA, timers, and I/O to run unmodified GBA ROMs correctly.
2. **OS ROM** — a homebrew ROM, built for this emulator (and real hardware), implementing cooperative task scheduling, interrupt-driven ticking, and manual memory management with no underlying OS support.

## CPU

The GBA uses a single **ARM7TDMI**, clocked at 16.78 MHz on real hardware. It's a 32-bit RISC core that executes two instruction sets:

- **ARM** — 32-bit fixed-width instructions, full register/addressing flexibility.
- **THUMB** — 16-bit compressed instructions, a reduced subset, used for most GBA game code since it's denser and faster to fetch over the 16-bit-wide game ROM bus.

The CPU switches between the two sets at runtime via the `T` bit in CPSR — the emulator has to decode and execute both.

### CPU components

| Component | What it does |
|---|---|
| **Register file** | 16 general-purpose registers (R0–R15), with R13 (SP), R14 (LR), R15 (PC) having architectural roles. Several are *banked* — swapped out per CPU mode (e.g. IRQ, SVC each get their own R13/R14). |
| **CPSR (Current Program Status Register)** | Holds condition flags (N, Z, C, V), the current CPU mode, the IRQ/FIQ disable bits, and the T bit (ARM vs THUMB state). Every conditional instruction reads this. |
| **SPSR (Saved PSR)** | One per privileged mode; holds a copy of CPSR saved automatically on exception entry, restored on return. |
| **Instruction decoder** | Separate decode paths for ARM (32-bit) and THUMB (16-bit) encodings; maps opcode bits to an execution handler. |
| **Pipeline (fetch/decode/execute)** | 3-stage pipeline; PC reads are offset (+8 in ARM state, +4 in THUMB) because of this — a classic emulator bug source if handled naively. |
| **CPU modes** | User, IRQ, FIQ (unused on GBA), Supervisor, Abort, Undefined, System. Mode determines register banking and privilege. |
| **Exception/interrupt handling** | On IRQ/SWI/etc., CPU auto-switches mode, banks LR/SPSR, jumps to a fixed vector. GBA uses this for the whole interrupt system (VBlank, timers, DMA, etc.) via a BIOS-level IRQ handler. |
