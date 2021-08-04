#include "csv-read/csv.hpp"
#include "common.hpp"

#include <iostream>
#include <array>
#include <string_view>


int main(int argc, char *argv[]) {
    auto cfg = read_config();
    auto iprefix = cfg.input;
    // nation
    {
      TableReader<unsigned, std::string_view, unsigned, std::string_view>
          reader("output/nation/", (iprefix + "nation.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for nation" << std::endl;
    }
    // customer
    {
      TableReader<unsigned, std::string_view, std::string_view, unsigned, std::string_view, double, std::string_view, std::string_view>
          reader("output/customer/", (iprefix + "customer.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for customer" << std::endl;
    }
    // lineitem
    {
      TableReader<unsigned, unsigned, unsigned, unsigned, unsigned, double, double, double, char, char, std::string_view, std::string_view, std::string_view, std::string_view, std::string_view, std::string_view>
          reader("output/lineitem/", (iprefix + "lineitem.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for lineitem" << std::endl;
    }
    // orders
    {
      TableReader<unsigned, unsigned, char, double, std::string_view, std::string_view, std::string_view, unsigned, std::string_view>
          reader("output/orders/", (iprefix + "orders.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for orders" << std::endl;
    }
    // part
    {
      TableReader<unsigned, std::string_view, std::string_view, std::string_view, std::string_view, unsigned, std::string_view, double, std::string_view>
          reader("output/part/", (iprefix + "part.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for part" << std::endl;
    }
    // partsupp
    {
      TableReader<unsigned, unsigned, unsigned, double, std::string_view>
          reader("output/partsupp/", (iprefix + "partsupp.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for partsupp" << std::endl;
    }
    // region
    {
      TableReader<unsigned, std::string_view, std::string_view>
          reader("output/region/", (iprefix + "region.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for region" << std::endl;
    }
    // supplier
    {
      TableReader<unsigned, std::string_view, std::string_view, unsigned, std::string_view, double, std::string_view>
          reader("output/supplier/", (iprefix + "supplier.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << "rows for supplier" << std::endl;
    }
    // io::csv::read_file<'|', '\n', decltype(consume_cell)>(cfg.input.c_str(), nation_cols, consume_cell);
    return 0;
}
