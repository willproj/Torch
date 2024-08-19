#pragma once
#include <functional>


namespace utils
{
    template <typename T>
    class Ref {
    public:
        Ref(T& ref) noexcept : m_Ref(ref) {}

        Ref(const Ref& other) noexcept = default;

        Ref& operator=(const Ref& other) noexcept = default;

        T& get() const noexcept { return m_Ref.get(); }

        operator T& () const noexcept { return m_Ref.get(); }

        T* operator->() const noexcept { return &m_Ref.get(); }

    private:
        std::reference_wrapper<T> m_Ref; 
    };
}