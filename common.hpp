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

  uintptr_t output_size;
  std::vector<T> items;

  ColumnOutput(unsigned expected_rows = 1024) : output_size(page_t::GLOBAL_OVERHEAD), items() {
    items.reserve(expected_rows);
  }

  ~ColumnOutput() {
    // std::size_t size = page->end() -
  }

  bool append(const T& val) {
    items.push_back(val);
    if constexpr (page_t::size_tag::IS_VARIABLE) {
      output_size += val.size() + page_t::PER_ITEM_OVERHEAD;
    } else {
      output_size += sizeof(T);
    }
    return true;
  }

  page_t make_page(const char* filename) const {
    auto page = page_t(filename, O_CREAT, output_size);
    if constexpr (page_t::size_tag::IS_VARIABLE) {
      auto idx = 0ul;
      auto offset = page.file_size;
      char* data = reinterpret_cast<char*>(page.data());
      for (auto& str : items) {
        offset -= str.size();
        std::copy(str.begin(), str.end(), data + offset);
        page.slot_at(idx) = { str.size(), offset };
        ++idx;
      }
    } else {
      std::copy(items.begin(), items.end(), page.begin());
    }
    return page;
  }
};

template<typename... Ts>
struct TableReader {

  using tuple_type = std::tuple<Ts...>;
  using outputs_t = std::tuple<ColumnOutput<Ts>...>;

  outputs_t outputs;
  std::array<std::string, sizeof...(Ts)> output_files;

  TableReader(const std::string& output_prefix) {
    fold_outputs(0, [&](const auto& output, unsigned idx, unsigned num) {
      using value_t = typename std::remove_reference<decltype(output)>::type::value_t;
      using parser_t = io::csv::Parser<value_t>;
      output_files[idx] = output_prefix + std::to_string(idx) + "." + parser_t::TYPE_NAME + ".bin";
      return 0;
    });
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

