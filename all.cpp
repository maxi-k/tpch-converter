#include "csv-read/csv.hpp"
#include "common.hpp"
#include "tpch.hpp"

#include <iostream>
#include <array>
#include <string_view>


int main(int argc, char *argv[]) {
    auto cfg = read_config();
    auto iprefix = cfg.input;
    // nation
    {
      tpch::nation::reader reader("output/nation/", (iprefix + "nation.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for nation" << std::endl;
    }
    // customer
    {
      tpch::customer::reader reader("output/customer/", (iprefix + "customer.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for customer" << std::endl;
    }
    // lineitem
    {
      tpch::lineitem::reader reader("output/lineitem/", (iprefix + "lineitem.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for lineitem" << std::endl;
    }
    // orders
    {
      tpch::orders::reader reader("output/orders/", (iprefix + "orders.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for orders" << std::endl;
    }
    // part
    {
     tpch::part::reader reader("output/part/", (iprefix + "part.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for part" << std::endl;
    }
    // partsupp
    {
     tpch::partsupp::reader reader("output/partsupp/", (iprefix + "partsupp.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for partsupp" << std::endl;
    }
    // region
    {
      tpch::region::reader reader("output/region/", (iprefix + "region.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for region" << std::endl;
    }
    // supplier
    {
      tpch::supplier::reader reader("output/supplier/", (iprefix + "supplier.tbl").c_str());
      auto rows = reader.read();
      std::cout << "read " << rows << " rows for supplier" << std::endl;
    }
    // io::csv::read_file<'|', '\n', decltype(consume_cell)>(cfg.input.c_str(), nation_cols, consume_cell);
    return 0;
}
