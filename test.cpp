#include <iostream>
#include <gtest/gtest.h>
#include "MijJson.h"

#define EXPECT_MEMEQ(s1, s2, len) EXPECT_TRUE(memcmp(s1, s2, len) == 0)

namespace mij_json {
struct DocumentTest {
    static inline void testLiteral(ValueType type, const char *json) {
        Value v{};
        v.type = MIJ_OBJECT;
        EXPECT_EQ(MIJ_PARSE_OK, v.parse(json));
        EXPECT_EQ(type, v.getType());
    }

    static inline void testParseLiteral() {
        testLiteral(MIJ_NULL, "null");
        testLiteral(MIJ_NULL, "    null\n\t\r");
        testLiteral(MIJ_TRUE, "true");
        testLiteral(MIJ_FALSE, "false");
    }

    static inline void testNumber(double expect, const char *json) {
        Value v;
        EXPECT_EQ(MIJ_PARSE_OK, v.parse(json));
        EXPECT_EQ(MIJ_NUMBER, v.getType());
        EXPECT_DOUBLE_EQ(expect, v.getNumber());
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
        testNumber(0.0, "1e-10000");  // underflow

        testNumber(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
        testNumber( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
        testNumber(-4.9406564584124654e-324, "-4.9406564584124654e-324");
        testNumber( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
        testNumber(-2.2250738585072009e-308, "-2.2250738585072009e-308");
        testNumber( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
        testNumber(-2.2250738585072014e-308, "-2.2250738585072014e-308");
        testNumber( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
        testNumber(-1.7976931348623157e+308, "-1.7976931348623157e+308");
    }

    static void testString(const char* expect, const char* json) {
        Value v{};
        EXPECT_EQ(MIJ_PARSE_OK, v.parse(json));
        EXPECT_EQ(MIJ_STRING, v.getType());
        EXPECT_MEMEQ(expect, v.getString(), v.getStringLength());
    }

    static void testParseString() {
//        testString("", "\"\"");
//        testString("Hello", "\"Hello\"");
//        testString("Hello\nWorld", R"("Hello\nWorld")");
//        testString("\" \\ / \b \f \n \r \t", R"("\" \\ \/ \b \f \n \r \t")");
    }

    static inline void testError(int error, const char *json) {
        Value v;
        v.type = MIJ_TRUE;
        EXPECT_EQ(error, v.parse(json));
        EXPECT_EQ(MIJ_NULL, v.getType());
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

    static void testParseMissingQuotationMark() {
//        testError(MIJ_PARSE_MISS_QUOTATION_MARK, "\"");
//        testError(MIJ_PARSE_MISS_QUOTATION_MARK, "\"abc");
    }

    static void testParseInvalidStringEscape() {
//        testError(MIJ_PARSE_INVALID_STRING_ESCAPE, R"("\v")");
//        testError(MIJ_PARSE_INVALID_STRING_ESCAPE, R"("\'")");
//        testError(MIJ_PARSE_INVALID_STRING_ESCAPE, R"("\0")");
//        testError(MIJ_PARSE_INVALID_STRING_ESCAPE, R"("\x12")");
    }

    static void testParseInvalidStringChar() {
//        testError(MIJ_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
//        testError(MIJ_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
    }

    static void testAccessNull() {
        Value v{};
        v.setString("a", 1);
        v.setNull();
        EXPECT_EQ(MIJ_NULL, v.getType());
    }

    static void testAccessBoolean() {
        Value v{};
        v.setBoolean(true);
        EXPECT_TRUE(v.getBoolean());

        v.setBoolean(false);
        EXPECT_FALSE(v.getBoolean());
    }

    static void testAccessNumber() {
        Value v{};
        v.setNumber(12345.678);
        EXPECT_DOUBLE_EQ(v.getNumber(), 12345.678);
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
};
}  // namespace mij_json

using namespace mij_json;

TEST(documentTest, parse) {
    DocumentTest::testParseLiteral();
    DocumentTest::testParseNumber();
    DocumentTest::testParseExpectValue();
    DocumentTest::testParseInvalidValue();
    DocumentTest::testParseRootNotSingular();
    DocumentTest::testParseNumberTooLarge();
    DocumentTest::testParseString();
}

TEST(documentTest, access) {
    DocumentTest::testAccessNull();
    DocumentTest::testAccessBoolean();
    DocumentTest::testAccessNumber();
    DocumentTest::testAccessString();
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
