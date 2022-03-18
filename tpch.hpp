#pragma once
#include <string_view>
#include "common.hpp"

namespace tpch {

    template<typename... Ts>
    struct TableDef {
        using import = TableImport<Ts...>;
        using reader = TableReader<Ts...>;
    };

    /**
     * CREATE TABLE NATION  (
     *  N_NATIONKEY  INTEGER NOT NULL,
     *  N_NAME       CHAR(25) NOT NULL,
     *  N_REGIONKEY  INTEGER NOT NULL,
     *  N_COMMENT    VARCHAR(152)
     * );
     **/
    using nation = TableDef<int, std::string_view, int, std::string_view>;
    constexpr std::array nation_c {"n_nationkey", "n_name", "n_regionkey", "n_comment"};

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
    using customer = TableDef<int, std::string_view, std::string_view, int, std::string_view, double, std::string_view, std::string_view>;
    constexpr std::array customer_c {"c_custkey", "c_name", "c_address", "c_nationkey", "c_phone", "c_acctbal", "c_mktsegment", "c_comment"};

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
    using lineitem = TableDef<int, int, int, int, int, double, double, double, char, char, std::string_view, std::string_view, std::string_view, std::string_view, std::string_view, std::string_view>;
    constexpr std::array lineitem_c {"l_orderkey", "l_partkey", "l_suppkey", "l_linenumber", "l_quantity", "l_extendedprice", "l_discount", "l_tax", "l_returnflag", "l_linestatus", "l_shipdate", "l_commitdate", "l_receiptdate", "l_shipinstruct", "l_shipmode", "l_comment"};

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
    using orders = TableDef<int, int, char, double, std::string_view, std::string_view, std::string_view, int, std::string_view>;
    constexpr std::array orders_c {"o_orderkey", "o_custkey", "o_orderstatus", "o_totalprice", "o_orderdate", "o_orderpriority", "o_clerk", "o_shippriority", "o_comment"};

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
    using part = TableDef<int, std::string_view, std::string_view, std::string_view, std::string_view, int, std::string_view, double, std::string_view>;
    constexpr std::array part_c { "p_partkey", "p_name", "p_mfgr", "p_brand", "p_type", "p_size", "p_container", "p_retailprice", "p_comment"};

    /**
     * CREATE TABLE PARTSUPP (
     *  PS_PARTKEY     INTEGER NOT NULL,
     *  PS_SUPPKEY     INTEGER NOT NULL,
     *  PS_AVAILQTY    INTEGER NOT NULL,
     *  PS_SUPPLYCOST  DECIMAL(15,2)  NOT NULL,
     *  PS_COMMENT     VARCHAR(199) NOT NULL
     * );
     **/
    using partsupp = TableDef<int, int, int, double, std::string_view>;
    constexpr std::array partsupp_c { "ps_partkey", "ps_suppkey", "ps_availqty", "ps_supplycost", "ps_comment"};
    /**
     * CREATE TABLE REGION (
     *   R_REGIONKEY  INTEGER NOT NULL,
     *   R_NAME       CHAR(25) NOT NULL,
     *   R_COMMENT    VARCHAR(152)
     * );
     **/
    using region = TableDef<int, std::string_view, std::string_view>;
    constexpr std::array region_c {"r_regionkey", "r_name", "r_comment"};

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
    using supplier = TableDef<int, std::string_view, std::string_view, int, std::string_view, double, std::string_view>;
    constexpr std::array supplier_c {"s_suppkey", "s_name", "s_address", "s_nationkey", "s_phone", "s_acctbal", "s_comment"};
} // namespace tpch
