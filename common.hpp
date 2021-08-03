#pragma once

#include <string>
#include <cassert>
#include <vector>
#include "csv-reader/util.hpp"

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
  using page_t = io::DataColumn<T>;
  std::vector<T> items;

  ColumnOutput(const char* filename, unsigned expected_rows = 1024) {
    items.reserve(expected_rows);
  }

  ~ColumnOutput() {
    // std::size_t size = page->end() -
  }

  bool append(const T& val) {
    items.push_back(val);
  }

  page_t make_page();
};
