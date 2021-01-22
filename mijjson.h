/**
/* @author Jason Cheung
 * @date 2021/1/21.
 */

#ifndef MIJJSON_MIJJSON_H
#define MIJJSON_MIJJSON_H

#ifndef MIJ_PARSE_STACK_INIT_SIZE
#define MIJ_PARSE_STACK_INIT_SIZE 256
#endif

#include <cstddef>
#include <memory>

namespace mijjson {

    enum ParseError {
        MIJ_PARSE_OK = 0,
        MIJ_PARSE_EXPECT_VALUE,
        MIJ_PARSE_INVALID_VALUE,
        MIJ_PARSE_ROOT_NOT_SINGULAR,
        MIJ_PARSE_NUMBER_TOO_LARGE,
        MIJ_PARSE_MISS_QUOTATION_MARK,
        MIJ_PARSE_INVALID_STRING_ESCAPE,
        MIJ_PARSE_INVALID_STRING_CHAR
    };

    enum ValueType {
        MIJ_NULL,
        MIJ_FALSE,
        MIJ_TRUE,
        MIJ_NUMBER,
        MIJ_STRING,
        MIJ_ARRAY,
        MIJ_OBJECT
    };

    class Value {
    private:
        ValueType type;
        union {
            double number;
            struct {
                size_t length;
                char *string;
            };
        };

    public:
        Value() : type{MIJ_NULL} {};

        ~Value();

        ParseError parse(const char *json);

        ValueType getType() const;

        void setNull();

        bool getBoolean() const;

        void setBoolean(bool boolean);

        double getNumber() const;

        void setNumber(double number);

        const char *getString() const;

        size_t getStringLength() const;

        void setString(const char *, size_t length);

        friend class DocumentTest;

    private:
        struct Context;

        static void parseWhitespaces(Context *c);

        ParseError parseValue(Context *c);

        ParseError parseLiteral(Context *c, const char *literal, int literal_length, ValueType literal_type);

        ParseError parseNumber(Context *c);

        ParseError parseString(Context *c);
    };
}  /* namespace mijjson */

#endif  //MIJJSON_MIJJSON_H
