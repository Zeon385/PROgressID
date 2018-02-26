#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-PID");
    BOOST_CHECK(GetBoolArg("-PID"));
    BOOST_CHECK(GetBoolArg("-PID", false));
    BOOST_CHECK(GetBoolArg("-PID", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BARo"));
    BOOST_CHECK(!GetBoolArg("-BARo", false));
    BOOST_CHECK(GetBoolArg("-BARo", true));

    ResetArgs("-PID=0");
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", false));
    BOOST_CHECK(!GetBoolArg("-PID", true));

    ResetArgs("-PID=1");
    BOOST_CHECK(GetBoolArg("-PID"));
    BOOST_CHECK(GetBoolArg("-PID", false));
    BOOST_CHECK(GetBoolArg("-PID", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", false));
    BOOST_CHECK(!GetBoolArg("-PID", true));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", false));
    BOOST_CHECK(!GetBoolArg("-PID", true));

    ResetArgs("-PID -noBAR");  // -PID should win
    BOOST_CHECK(GetBoolArg("-PID"));
    BOOST_CHECK(GetBoolArg("-PID", false));
    BOOST_CHECK(GetBoolArg("-PID", true));

    ResetArgs("-PID=1 -noBAR=1");  // -PID should win
    BOOST_CHECK(GetBoolArg("-PID"));
    BOOST_CHECK(GetBoolArg("-PID", false));
    BOOST_CHECK(GetBoolArg("-PID", true));

    ResetArgs("-PID=0 -noBAR=0");  // -PID should win
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", false));
    BOOST_CHECK(!GetBoolArg("-PID", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--PID=1");
    BOOST_CHECK(GetBoolArg("-PID"));
    BOOST_CHECK(GetBoolArg("-PID", false));
    BOOST_CHECK(GetBoolArg("-PID", true));

    ResetArgs("--noBAR=1");
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", false));
    BOOST_CHECK(!GetBoolArg("-PID", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PID", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PID", "eleven"), "eleven");

    ResetArgs("-PID -bar");
    BOOST_CHECK_EQUAL(GetArg("-PID", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PID", "eleven"), "");

    ResetArgs("-PID=");
    BOOST_CHECK_EQUAL(GetArg("-PID", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PID", "eleven"), "");

    ResetArgs("-PID=11");
    BOOST_CHECK_EQUAL(GetArg("-PID", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-PID", "eleven"), "11");

    ResetArgs("-PID=eleven");
    BOOST_CHECK_EQUAL(GetArg("-PID", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-PID", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PID", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-PID", 0), 0);

    ResetArgs("-PID -bar");
    BOOST_CHECK_EQUAL(GetArg("-PID", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-PID=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-PID", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-PID=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-PID", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--PID");
    BOOST_CHECK_EQUAL(GetBoolArg("-PID"), true);

    ResetArgs("--PID=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-PID", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", true));
    BOOST_CHECK(!GetBoolArg("-PID", false));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-PID"));
    BOOST_CHECK(!GetBoolArg("-PID", true));
    BOOST_CHECK(!GetBoolArg("-PID", false));

    ResetArgs("-noBAR=0");
    BOOST_CHECK(GetBoolArg("-PID"));
    BOOST_CHECK(GetBoolArg("-PID", true));
    BOOST_CHECK(GetBoolArg("-PID", false));

    ResetArgs("-PID --noBAR");
    BOOST_CHECK(GetBoolArg("-PID"));

    ResetArgs("-noBAR -PID"); // PID always wins:
    BOOST_CHECK(GetBoolArg("-PID"));
}

BOOST_AUTO_TEST_SUITE_END()
