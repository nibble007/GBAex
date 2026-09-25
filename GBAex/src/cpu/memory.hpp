#pragma once
#include <cstdint>
#include <array>


class MEMORY {      
public:
    static const uint32_t extWRamSize = 0x40000; // 256KB
    static const uint32_t inWRamSize  = 0x8000;  // 32KB
    static const uint32_t ioRamSize   = 0x400;   // 1KB
    static const uint32_t palRamSize  = 0x400;   // 1KB
    static const uint32_t vRamSize    = 0x18000; // 96KB
    static const uint32_t oamSize     = 0x400;   // 1KB
    public:
    uint8_t Read8(uint32_t address) {
        return Decode(address);
    }

    uint16_t Read16(uint32_t address) {
        uint16_t lo = Read8(address);
        uint16_t hi = Read8(address + 1);
        return static_cast<uint16_t>(lo | (hi << 8));
    }

    uint32_t Read32(uint32_t address) {
        uint32_t b0 = Read8(address);
        uint32_t b1 = Read8(address + 1);
        uint32_t b2 = Read8(address + 2);
        uint32_t b3 = Read8(address + 3);
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }

    void Write8(uint32_t address, uint8_t value) {
        Decode(address) = value;
    }

    void Write16(uint32_t address, uint16_t value) {
        Write8(address,     static_cast<uint8_t>(value & 0xFF));
        Write8(address + 1, static_cast<uint8_t>((value >> 8) & 0xFF));
    }

    void Write32(uint32_t address, uint32_t value) {
        Write8(address,     static_cast<uint8_t>(value & 0xFF));
        Write8(address + 1, static_cast<uint8_t>((value >> 8) & 0xFF));
        Write8(address + 2, static_cast<uint8_t>((value >> 16) & 0xFF));
        Write8(address + 3, static_cast<uint8_t>((value >> 24) & 0xFF));
    }

private:
    std::array<uint8_t, extWRamSize> extWram{};
    std::array<uint8_t, inWRamSize>  inWRam{};
    std::array<uint8_t, ioRamSize>   ioRam{};
    std::array<uint8_t, palRamSize>  palRam{};
    std::array<uint8_t, vRamSize>    vRam{};
    std::array<uint8_t, oamSize>     oamRam{};
    uint8_t& Decode(uint32_t address) {
        switch ((address >> 24) & 0xFF) {
            case 0x02: 
                return extWram[address & (extWRamSize - 1)];      // EWRAM, mirrors every 0x40000
            case 0x03: 
                return inWRam[address & (inWRamSize - 1)];        // IWRAM, mirrors every 0x8000
            case 0x04: 
                return ioRam[address & (ioRamSize - 1)];          // IO, mirrors every 0x400 (simplified)
            case 0x05: 
                return palRam[address & (palRamSize - 1)];        // Palette, mirrors every 0x400
            case 0x06: {                                                  // VRAM, mirrors every 0x20000 with a fold
                uint32_t offset = address & 0x1FFFF;
                if (offset >= vRamSize) offset -= 0x8000;
                return vRam[offset];
            }
            case 0x07: 
                return oamRam[address & (oamSize - 1)];           // OAM, mirrors every 0x400
            default: {
                static uint8_t openBus = 0;                               // unmapped address fallback
                return openBus;
            }
        }
    }
};


/*
mirroring:in short very short what is mirroring
Same physical memory, visible at multiple addresses. 
Read/write a mirror address, 
and it's really reading/writing the same underlying byte as its "real" address
just at a different location in the address space. 
If you write to address A, and address B holds the exact same data automatically 
B is just A wearing a different address that's mirroring.
*/