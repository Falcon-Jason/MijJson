/**
/* @author Jason Cheung
 * @date 2021/1/21.
 */

#include <cstdlib>

#ifndef MIJJSON_MIJJSON_VALUE_H
#define MIJJSON_MIJJSON_VALUE_H

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

    struct SimpleAllocator{
        static char *malloc(size_t size) {
            return static_cast<char *>(std::malloc(size));
        }

        static char *realloc(char *base, size_t size) {
            return static_cast<char *>(std::realloc(base, size));
        }

        static void free(char *base) {
            std::free(base);
        }
    };

    class Value {
    private:
        using Allocator = SimpleAllocator;
        ValueType type;
        union {
            double number;
            struct {
                bool boolean;
                size_t length;
                char *string;
            };
        };

    public:
        Value() : type{MIJ_NULL} {};

        ~Value();

        ParseError parse(const char *json);

        ValueType getType() const;

        bool isNull() const;

        void setNull();

        bool isBoolean() const;

        bool getBoolean() const;

        void setBoolean(bool boolean);

        bool isNumber() const;

        double getNumber() const;

        void setNumber(double number);

        bool isString() const;

        const char *getString() const;

        size_t getStringLength() const;

        void setString(const char *, size_t length);

        friend class Parser;
    };
}  /* namespace mijjson */

#endif  //MIJJSON_MIJJSON_VALUE_H
