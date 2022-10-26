#pragma once

#include <memory>
#include <string>
#include <vector>

#include "dtl-location.hpp"
#include "dtl-uuid.hpp"

namespace dtl {
namespace manifest {

struct Column {
    std::string name;
    dtl::UUID array;
};

struct Snapshot {
    dtl::Location start;
    dtl::Location end;
    std::vector<Column> columns;
};

struct Mapping {
    // The identifiers of the two arrays that this mapping connects.
    dtl::UUID src_array;
    dtl::UUID tgt_array;

    // TODO more efficient representations for common cases.
    // Pair of arrays forming a table that maps indexes in the source array into
    // indexes in the target array.
    dtl::UUID src_index_array;
    dtl::UUID tgt_index_array;
};

class Manifest {
  public:
    std::string source;
    std::vector<Snapshot> snapshots;
    std::vector<Mapping> mappings;
};

} /* namespace manifest */
} /* namespace dtl */
