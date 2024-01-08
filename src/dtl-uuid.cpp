#include "dtl-uuid.hpp"

#include <array>
#include <cstddef>
#include <uuid.h>

namespace dtl {

UUID
uuid4(void) {
    uuid_t data;
    uuid_generate_random(data);
    return UUID(reinterpret_cast<std::array<std::byte, 16>&>(data));
}

} /* namespace dtl */
