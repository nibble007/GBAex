#pragma once

#include <cstdint>
#include <vector>
#include <fstream>
#include <string>

class ROM {
public:
    bool Load(const std::string& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        data.resize(static_cast<size_t>(size));
        if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
            return false;
        }

        return true;
    }

    uint8_t Read8(uint32_t address) const {
        uint32_t offset = address & 0x01FFFFFF; // ROM region spans 0x08000000-0x09FFFFFF etc, mask to offset
        if (offset >= data.size()) {
            return 0; // open bus / out-of-range read
        }
        return data[offset];
    }

    uint16_t Read16(uint32_t address) const {
        uint16_t lo = Read8(address);
        uint16_t hi = Read8(address + 1);
        return static_cast<uint16_t>(lo | (hi << 8));
    }

    uint32_t Read32(uint32_t address) const {
        uint32_t b0 = Read8(address);
        uint32_t b1 = Read8(address + 1);
        uint32_t b2 = Read8(address + 2);
        uint32_t b3 = Read8(address + 3);
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }

private:
    std::vector<uint8_t> data;
};