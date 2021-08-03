#pragma once

#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <array>
#include <utility>
#include "csv-read/csv.hpp"
#include "csv-read/util.hpp"

using CharIter = io::csv::CharIter;
static constexpr char delim = '|';
struct RunConfig {
    std::string input;
};


RunConfig read_config() {
  auto file = getenv("INPUT");
  assert(file);
  return RunConfig{.input = file};
}

template<typename T>
struct ColumnOutput {
  using value_t = T;
  using page_t = io::DataColumn<T>;
  std::vector<T> items;

  ColumnOutput(unsigned expected_rows = 1024) {
    items.reserve(expected_rows);
  }

  ~ColumnOutput() {
    // std::size_t size = page->end() -
  }

  bool append(const T& val) {
    items.push_back(val);
    return true;
  }

  page_t make_page(const char* filename) const {
    // TODO
    return page_t(filename, O_CREAT, items.size() * sizeof(T));
  }
};

template<typename... Ts>
struct TableReader {

  using tuple_type = std::tuple<Ts...>;
  using outputs_t = std::tuple<ColumnOutput<Ts>...>;

  outputs_t outputs;
  std::array<std::string, sizeof...(Ts)> output_files;

  TableReader(const std::string& output_prefix) {
    for (auto i = 0u; i != sizeof...(Ts); ++i) {
      output_files[i] = output_prefix + std::to_string(i) + ".bin";
    }
  }

  ~TableReader() {
    fold_outputs(0, [&](const auto& output, unsigned idx, unsigned num) {
      auto page = output.make_page(output_files[idx].c_str());
      page.flush();
      return 0;
    });
  }

  unsigned read(const char* filename) {
    std::vector<unsigned> columns(sizeof...(Ts));
    for (auto i = 0u; i != sizeof...(Ts); ++i) {
      columns[i] = i;
    }

    unsigned rows = io::csv::read_file<delim>(filename, columns, [&](unsigned col, CharIter& pos) {
      fold_outputs(0, [&](auto& output, unsigned idx, unsigned num) {
        if (idx == col) {
          using value_t = typename std::remove_reference<decltype(output)>::type::value_t;
          io::csv::Parser<value_t> parser;
          auto value = parser.template parse_value<delim>(pos);
          output.append(value);
        }
        return 0;
      });
    });
    return rows;
  }

  template <typename T, typename F, unsigned I = 0>
  constexpr inline T fold_outputs(T init_value, const F &fn) {
    if constexpr (I == sizeof...(Ts)) {
      return init_value;
    } else {
      return fold_outputs<T, F, I + 1>(fn(std::get<I>(outputs), I, sizeof...(Ts)), fn);
    }
  }

}; // struct TableReader

