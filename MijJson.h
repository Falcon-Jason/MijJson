//
// Created by Jason on 2021/1/21.
//

#ifndef MIJJSON_MIJJSON_H
#define MIJJSON_MIJJSON_H

#include <cstddef>
#include <memory>

namespace mij_json {

enum ParseError {
    MIJ_PARSE_OK = 0,
    MIJ_PARSE_EXPECT_VALUE,
    MIJ_PARSE_INVALID_VALUE,
    MIJ_PARSE_ROOT_NOT_SINGULAR,
    MIJ_PARSE_NUMBER_TOO_LARGE
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

    const char* getString() const;
    size_t getStringLength() const;
    void setString(const char*, size_t length);

    friend class DocumentTest;

private:
    struct Context;
    static void parseWhitespaces(Context *c);
    ParseError parseValue(Context *c);
    ParseError parseLiteral(Context *c, const char *literal, int literal_length, ValueType literal_type);
    ParseError parseNumber(Context *c);
};
}  // namespace mij_json

#endif  //MIJJSON_MIJJSON_H
