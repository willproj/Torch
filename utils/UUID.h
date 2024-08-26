#pragma once
#include <pch/pch.h>

namespace utils
{
    class UUID {
    public:
        UUID();
        std::string ToString() const;
        void SetUUIDStr(const std::string& uuidStr);
    private:
        std::string m_UUIDStr;
        static std::string Generate();
    };
}
