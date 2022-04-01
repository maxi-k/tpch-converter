#pragma once
#include <string_view>
#include "types.hpp"
#include "types-parse.hpp"
#include "common.hpp"

namespace tpch {
    using namespace types;
    using Decimal = Numeric<12, 2>;

    template<typename... Ts>
    struct TableDef {
        using import = TableImport<Ts...>;
        using reader = TableReader<Ts...>;
        using columns = typename import::tuple_type;

        template <template <typename> class Container>
        using map_columns = std::tuple<Container<Ts>...>;
    };

    /**
     * CREATE TABLE NATION  (
     *  N_NATIONKEY  INTEGER NOT NULL,
     *  N_NAME       CHAR(25) NOT NULL,
     *  N_REGIONKEY  INTEGER NOT NULL,
     *  N_COMMENT    VARCHAR(152)
     * );
     **/
    using nation = TableDef<Integer, Char<25>, Integer, Varchar<152>>;
    [[maybe_unused]] constexpr std::array nation_c { "n_nationkey", "n_name", "n_regionkey", "n_comment" };
    enum nation_columns : uint8_t { n_nationkey, n_name, n_regionkey, n_comment };

    /**
     * CREATE TABLE CUSTOMER (
     *  C_CUSTKEY     INTEGER NOT NULL,
     *  C_NAME        VARCHAR(25) NOT NULL,
     *  C_ADDRESS     VARCHAR(40) NOT NULL,
     *  C_NATIONKEY   INTEGER NOT NULL,
     *  C_PHONE       CHAR(15) NOT NULL,
     *  C_ACCTBAL     DECIMAL(15,2)   NOT NULL,
     *  C_MKTSEGMENT  CHAR(10) NOT NULL,
     *  C_COMMENT     VARCHAR(117) NOT NULL
     * );
     **/
    using customer = TableDef<Integer, Varchar<25>, Varchar<40>, Integer, Char<15>, Decimal, Char<10>, Varchar<117>>;
    [[maybe_unused]] constexpr std::array customer_c { "c_custkey", "c_name", "c_address", "c_nationkey", "c_phone", "c_acctbal", "c_mktsegment", "c_comment" };
    enum customer_columns { c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment };

    /**
     * CREATE TABLE LINEITEM (
     *  L_ORDERKEY       INTEGER NOT NULL,
     *  L_PARTKEY        INTEGER NOT NULL,
     *  L_SUPPKEY        INTEGER NOT NULL,
     *  L_LINENUMBER     INTEGER NOT NULL,
     *  L_QUANTITY       DECIMAL(15,2) NOT NULL,
     *  L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
     *  L_DISCOUNT       DECIMAL(15,2) NOT NULL,
     *  L_TAX            DECIMAL(15,2) NOT NULL,
     *  L_RETURNFLAG     CHAR(1) NOT NULL,
     *  L_LINESTATUS     CHAR(1) NOT NULL,
     *  L_SHIPDATE       DATE NOT NULL,
     *  L_COMMITDATE     DATE NOT NULL,
     *  L_RECEIPTDATE    DATE NOT NULL,
     *  L_SHIPINSTRUCT   CHAR(25) NOT NULL,
     *  L_SHIPMODE       CHAR(10) NOT NULL,
     *  L_COMMENT        VARCHAR(44) NOT NULL
     * );
     **/
    using lineitem = TableDef<Integer, Integer, Integer, Integer, Decimal, Decimal, Decimal, Decimal, Char<1>, Char<1>, Date, Date, Date, Char<25>, Char<10>, Varchar<44>>;
    [[maybe_unused]] constexpr std::array lineitem_c { "l_orderkey", "l_partkey", "l_suppkey", "l_linenumber", "l_quantity", "l_extendedprice", "l_discount", "l_tax", "l_returnflag", "l_linestatus", "l_shipdate", "l_commitdate", "l_receiptdate", "l_shipinstruct", "l_shipmode", "l_comment" };
    enum lineitem_columns : uint8_t { l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment };

    /**
     * CREATE TABLE ORDERS (
     *  O_ORDERKEY       INTEGER NOT NULL,
     *  O_CUSTKEY        INTEGER NOT NULL,
     *  O_ORDERSTATUS    CHAR(1) NOT NULL,
     *  O_TOTALPRICE     DECIMAL(15,2) NOT NULL,
     *  O_ORDERDATE      DATE NOT NULL,
     *  O_ORDERPRIORITY  CHAR(15) NOT NULL,
     *  O_CLERK          CHAR(15) NOT NULL,
     *  O_SHIPPRIORITY   INTEGER NOT NULL,
     *  O_COMMENT        VARCHAR(79) NOT NULL
     * );
     **/
    using orders = TableDef<Integer, Integer, Char<1>, Decimal, Date, Char<15>, Char<15>, Integer, Varchar<79>>;
    [[maybe_unused]] constexpr std::array orders_c { "o_orderkey", "o_custkey", "o_orderstatus", "o_totalprice", "o_orderdate", "o_orderpriority", "o_clerk", "o_shippriority", "o_comment" };
    enum orders_columns : uint8_t { o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment };

    /**
     *  CREATE TABLE PART (
     *   P_PARTKEY     INTEGER NOT NULL,
     *   P_NAME        VARCHAR(55) NOT NULL,
     *   P_MFGR        CHAR(25) NOT NULL,
     *   P_BRAND       CHAR(10) NOT NULL,
     *   P_TYPE        VARCHAR(25) NOT NULL,
     *   P_SIZE        INTEGER NOT NULL,
     *   P_CONTAINER   CHAR(10) NOT NULL,
     *   P_RETAILPRICE DECIMAL(15,2) NOT NULL,
     *   P_COMMENT     VARCHAR(23) NOT NULL
     * );
     **/
    using part = TableDef<Integer, Varchar<55>, Char<25>, Char<10>, Varchar<25>, Integer, Char<10>, Decimal, Varchar<23>>;
    [[maybe_unused]] constexpr std::array part_c { "p_partkey", "p_name", "p_mfgr", "p_brand", "p_type", "p_size", "p_container", "p_retailprice", "p_comment"};
    enum part_columns : uint8_t { p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment};

    /**
     * CREATE TABLE PARTSUPP (
     *  PS_PARTKEY     INTEGER NOT NULL,
     *  PS_SUPPKEY     INTEGER NOT NULL,
     *  PS_AVAILQTY    INTEGER NOT NULL,
     *  PS_SUPPLYCOST  DECIMAL(15,2)  NOT NULL,
     *  PS_COMMENT     VARCHAR(199) NOT NULL
     * );
     **/
    using partsupp = TableDef<Integer, Integer, Integer, Decimal, Varchar<199>>;
    [[maybe_unused]] constexpr std::array partsupp_c { "ps_partkey", "ps_suppkey", "ps_availqty", "ps_supplycost", "ps_comment"};
    enum partsupp_columns : uint8_t { ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment};
    /**
     * CREATE TABLE REGION (
     *   R_REGIONKEY  INTEGER NOT NULL,
     *   R_NAME       CHAR(25) NOT NULL,
     *   R_COMMENT    VARCHAR(152)
     * );
     **/
    using region = TableDef<Integer, Char<25>, Varchar<152>>;
    [[maybe_unused]] constexpr std::array region_c { "r_regionkey", "r_name", "r_comment" };
    enum region_columns : uint8_t { r_regionkey, r_name, r_comment };

    /**
     * CREATE TABLE SUPPLIER (
     *  S_SUPPKEY     INTEGER NOT NULL,
     *  S_NAME        CHAR(25) NOT NULL,
     *  S_ADDRESS     VARCHAR(40) NOT NULL,
     *  S_NATIONKEY   INTEGER NOT NULL,
     *  S_PHONE       CHAR(15) NOT NULL,
     *  S_ACCTBAL     DECIMAL(15,2) NOT NULL,
     *  S_COMMENT     VARCHAR(101) NOT NULL
     * );
     **/
    using supplier = TableDef<Integer, Char<25>, Varchar<40>, Integer, Char<15>, Decimal, Varchar<101>>;
    [[maybe_unused]] constexpr std::array supplier_c { "s_suppkey", "s_name", "s_address", "s_nationkey", "s_phone", "s_acctbal", "s_comment" };
    enum supplier_columns : uint8_t { s_suppkey, s_name, s_address, s_nationkey, s_phone, s_acctbal, s_comment };

    //
    //
    //
    enum TPCH_TABLE : uint8_t   {   NATION,   CUSTOMER,   LINEITEM,   ORDERS,   PART,   PARTSUPP,   REGION,   SUPPLIER };
    std::tuple TPCH_READERS   = { nation(), customer(), lineitem(), orders(), part(), partsupp(), region(), supplier() };
    std::tuple TABLE_COLS     = { nation_c, customer_c, lineitem_c, orders_c, part_c, partsupp_c, region_c, supplier_c };
    std::string TABLE_NAME[]  = { "nation", "customer", "lineitem", "orders", "part", "partsupp", "region", "supplier" };
    char DBGEN_TARGET[]       = {      'n',        'c',        'L',      'O',    'P',        'S',      'r',        's' };

    constexpr unsigned TABLE_COUNT = std::tuple_size_v<decltype(TPCH_READERS)>;
} // namespace tpch
