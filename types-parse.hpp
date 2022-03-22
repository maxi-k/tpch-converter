#pragma once

#include "csv-read/csv.hpp"
#include "types.hpp"

namespace io::csv {

    template<typename T> requires requires { T::TAG; }
    struct Parser<T> {
       static constexpr char const* TYPE_NAME = types::TYPE_NAMES[T::TAG];

       template <char delim, char eol = '\n'>
       inline T parse_value(CharIter& pos) {
           auto start = pos.iter;
           find_either<delim, eol>(pos);
           assert(pos.iter != nullptr && (*pos.iter == delim || *pos.iter == eol));
           return T::castString(start, pos.iter - start);
       }
    };

}
