#pragma once

#include <array>
#include <cstddef>

namespace dtl {

class UUID {
    std::array<std::byte, 16> data;
    friend bool
    operator==(UUID const& lhs, UUID const& rhs) noexcept;

  public:
    constexpr UUID(std::array<std::byte, 16> const& source) noexcept :
        data{source} {};
};

UUID
uuid4(void);

} /* namespace dtl */
