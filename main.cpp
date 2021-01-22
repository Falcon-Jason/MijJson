#include <iostream>
#include <gtest/gtest.h>
#include "JsonDocument.h"
namespace mij_json {
struct DocumentTest {
    static inline void testLiteral(ValueType type, const char *json) {
        Value v{};
        v.type = MIJ_OBJECT;
        EXPECT_EQ(MIJ_PARSE_OK, v.parse(json));
        EXPECT_EQ(type, v.getType());
    }

    static inline void testError(int error, const char *json) {
        Value v;
        v.type = MIJ_TRUE;
        EXPECT_EQ(error, v.parse(json));
        EXPECT_EQ(MIJ_NULL, v.getType());
    }

    static inline void testNumber(double expect, const char *json) {
        Value v;
        EXPECT_EQ(MIJ_PARSE_OK, v.parse(json));
        EXPECT_EQ(MIJ_NUMBER, v.getType());
        EXPECT_DOUBLE_EQ(expect, v.getNumber());
    }

    static inline void testParseLiteral() {
        testLiteral(MIJ_NULL, "null");
        testLiteral(MIJ_NULL, "    null\n\t\r");
        testLiteral(MIJ_TRUE, "true");
        testLiteral(MIJ_FALSE, "false");
    }

    static void testParseExpectValue() {
        testError(MIJ_PARSE_EXPECT_VALUE, "");
        testError(MIJ_PARSE_EXPECT_VALUE, " ");
    }

    static void testParseInvalidValue() {
        /* invalid literals */
        testError(MIJ_PARSE_INVALID_VALUE, "?");
        testError(MIJ_PARSE_INVALID_VALUE, "fals");
        testError(MIJ_PARSE_INVALID_VALUE, "falsee");

        /* invalid numbers */
        testError(MIJ_PARSE_INVALID_VALUE, "0123");
        testError(MIJ_PARSE_INVALID_VALUE, "0x0");
        testError(MIJ_PARSE_INVALID_VALUE, "0x123");
        testError(MIJ_PARSE_INVALID_VALUE, "+123");
        testError(MIJ_PARSE_INVALID_VALUE, "-");
        testError(MIJ_PARSE_INVALID_VALUE, "123.");
        testError(MIJ_PARSE_INVALID_VALUE, "-123.45E");
        testError(MIJ_PARSE_INVALID_VALUE, "123.45e");
        testError(MIJ_PARSE_INVALID_VALUE, "123.45E+");
        testError(MIJ_PARSE_INVALID_VALUE, "123.45E-");
        testError(MIJ_PARSE_INVALID_VALUE, ".123");
        testError(MIJ_PARSE_INVALID_VALUE, "INF");
        testError(MIJ_PARSE_INVALID_VALUE, "inf");
        testError(MIJ_PARSE_INVALID_VALUE, "NAN");
        testError(MIJ_PARSE_INVALID_VALUE, "nan");
    }

    static void testParseRootNotSingular() {
        testError(MIJ_PARSE_ROOT_NOT_SINGULAR, "null x");
        testError(MIJ_PARSE_ROOT_NOT_SINGULAR, "null null");
    }

    static void testParseNumberTooLarge() {
        testError(MIJ_PARSE_NUMBER_TOO_LARGE, "1e309");
        testError(MIJ_PARSE_NUMBER_TOO_LARGE, "-1e309");
    }

    static void testParseNumber() {
        testNumber(0.0, "0");
        testNumber(0.0, "-0");
        testNumber(0.0, "0.0");
        testNumber(0.0, "-0.0");
        testNumber(1.0, "1");
        testNumber(-1.0, "-1");
        testNumber(1.5, "1.5");
        testNumber(-1.5, "-1.5");
        testNumber(3.14159, "3.14159");
        testNumber(-314.15926, "-314.15926");
        testNumber(1.5E2, "1.5E2");
        testNumber(1.5e2, "1.5e2");
        testNumber(1.5E+2, "1.5E+2");
        testNumber(1.5E-2, "1.5E-2");
        testNumber(-1.5E2, "-1.5E2");
        testNumber(-1.5E-2, "-1.5E-2");
        testNumber(1.234e+10, "1.234e+10");
        testNumber(-1.234e-10, "-1.234e-10");
        testNumber(0.0, "1e-10000");  // overflow
    }

    static void testAccessString() {
        Value v;
        v.setString("", 0);
        EXPECT_STREQ("", v.getString());
        EXPECT_EQ(0, v.getStringLength());
        v.setString("Hello", 5);
        EXPECT_STREQ("Hello", v.getString());
        EXPECT_EQ(5, v.getStringLength());
    }

    static void testParse() {
        testParseLiteral();
        testParseNumber();
        testParseExpectValue();
        testParseInvalidValue();
        testParseRootNotSingular();
        testParseNumberTooLarge();
    }
};
}  // namespace mij_json

using namespace mij_json;

int main() {
    DocumentTest::testParse();
    DocumentTest::testAccessString();
    return EXIT_SUCCESS;
}
