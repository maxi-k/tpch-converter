#include "csv-read/csv.hpp"
#include "common.hpp"
#include "tpch.hpp"

#include <iostream>
#include <string_view>

using CharIter = io::csv::CharIter;


// static constexpr std::initializer_list<unsigned> nation_cols = {0, 1, 2, 3};
//
// void consume_cell(unsigned col, CharIter& pos) {
//     auto start = pos.iter;
//     switch(col) {
//         case 0:
//             std::cout << "0: " <<  io::csv::parse_unsigned<delim>(pos) << std::endl;
//             return;
//         case 1:
//             io::csv::find<delim>(pos);
//             std::cout << "1: " << std::string_view(start, pos.iter) << std::endl;
//             return;
//         case 2:
//             std::cout << "2: " <<  io::csv::parse_unsigned<delim>(pos) << std::endl;
//             return;
//         case 3:
//             io::csv::find<delim>(pos);
//             std::cout << "3: " << std::string_view(start, pos.iter) << std::endl;
//             return;
//     }
// }

int main(int argc, char *argv[]) {
    auto cfg = read_config();
    tpch::nation::reader reader("output/nation/", (cfg.input + "nation.tbl").c_str());
    auto rows = reader.read();
    std::cout << "read " << rows << " rows for nation" << std::endl;
    return 0;
}
