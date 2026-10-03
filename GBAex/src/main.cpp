#include "cpu/memory.hpp"
#include "loader.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom path>\n";
        return 1;
    }

    ROM rom;
    if (!rom.Load(argv[1])) {
        std::cerr << "Failed to load ROM: " << argv[1] << "\n";
        return 1;
    }

    MEMORY memory;

    // REMOVE IT LATER.
    std::cout << "First ROM byte: " << std::hex << (int)rom.Read8(0x08000000) << "\n";

    memory.Write8(0x02000000, 0xAB);
    std::cout << "EWRAM readback: " << std::hex << (int)memory.Read8(0x02000000) << "\n";

    return 0;
}