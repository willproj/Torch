#include "UUID.h"

namespace utils
{
    UUID::UUID() : m_UUIDStr(Generate()) {}

    std::string UUID::ToString() const {
        return m_UUIDStr;
    }

    void UUID::SetUUIDStr(const std::string& uuidStr)
    {
        m_UUIDStr = uuidStr;
    }

    std::string UUID::Generate() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();

        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        ss << std::setw(8) << static_cast<uint32_t>(now & 0xFFFFFFFF) << "-"; // Lower 32 bits of timestamp
        ss << std::setw(4) << (dis(gen) & 0xFFFF) << "-"; // Random
        ss << std::setw(4) << ((dis(gen) & 0x0FFF) | 0x4000) << "-"; // UUID version 4
        ss << std::setw(4) << ((dis(gen) & 0x3FFF) | 0x8000) << "-"; // UUID variant
        ss << std::setw(12) << dis(gen) << dis(gen); // Random

        return ss.str();
    }
}
