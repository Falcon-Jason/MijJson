//
// Created by Jason on 2021/1/21.
//

#include "MijJson.h"

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cmath>

namespace mij_json {
    Value::~Value() {
        setNull();
    }

    struct Value::Context {
        const char *json;
    };

    ValueType Value::getType() const {
        return type;
    }

    void Value::setNull() {
        if(this->type == MIJ_STRING) {
            delete[] string;
        }
        this->type = MIJ_NULL;
    }

    bool Value::getBoolean() const {
        assert(this->type == MIJ_TRUE || this->type == MIJ_FALSE);
        return this->type == MIJ_TRUE;
    }

    void Value::setBoolean(bool boolean) {
        this->type = boolean ? MIJ_TRUE : MIJ_FALSE;
    }

    double Value::getNumber() const {
        assert(this->type == MIJ_NUMBER);
        return number;
    }

    void Value::setNumber(double number) {
        this->type = MIJ_NUMBER;
        this->number = number;
    }

    const char *Value::getString() const {
        assert(this->type == MIJ_STRING);
        return string;
    }

    size_t Value::getStringLength() const {
        assert(this->type == MIJ_STRING);
        return length;
    }

    void Value::setString(const char *str, size_t length) {
        assert(str != nullptr || length == 0);
        setNull();
        this->string = new char[length];
        this->length = length;
        memcpy(this->string, str, length);
        this->string[length] = '\0';
        this->type = MIJ_STRING;
    }

    ParseError Value::parse(const char *json) {
        ParseError error;
        Context c{json};

        type = MIJ_NULL;

        parseWhitespaces(&c);

        if ((error = parseValue(&c)) == MIJ_PARSE_OK) {
            if (*c.json != '\0' && !isspace(*c.json)) {
                error = MIJ_PARSE_INVALID_VALUE;
            } else {
                parseWhitespaces(&c);
                if (*c.json != '\0') {
                    error = MIJ_PARSE_ROOT_NOT_SINGULAR;
                }
            }
        }

        if (error != MIJ_PARSE_OK) {
            type = MIJ_NULL;
        }
        return error;
    }

    ParseError Value::parseValue(Context *c) {
        switch (*c->json) {
            case 'n':
                return parseLiteral(c, "null", 4, MIJ_NULL);
            case 't':
                return parseLiteral(c, "true", 4, MIJ_TRUE);
            case 'f':
                return parseLiteral(c, "false", 5, MIJ_FALSE);
            case '0' ... '9':
            case '-':
                return parseNumber(c);
            case '\0':
                return MIJ_PARSE_EXPECT_VALUE;
            default:
                return MIJ_PARSE_INVALID_VALUE;
        }
    }

    void Value::parseWhitespaces(Context *c) {
        auto p = c->json;
        while (isspace(*p)) { p++; }
        c->json = p;
    }

    ParseError Value::parseLiteral(Context *c, const char *literal,
                                   int literal_length, ValueType literal_type) {
        if (strncmp(c->json, literal, literal_length) != 0) {
            return MIJ_PARSE_INVALID_VALUE;
        }
        c->json += literal_length;
        this->type = literal_type;
        return MIJ_PARSE_OK;
    }

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define PARSE_DIGITS(p)                      \
    do {                                     \
        if (ISDIGIT(*p)) {                   \
            do { p++; } while (ISDIGIT(*p)); \
        } else {                             \
            return MIJ_PARSE_INVALID_VALUE;  \
        }                                    \
    } while (0)

    ParseError Value::parseNumber(Context *c) {
        const char *p = c->json;

        /* parsing '-' */
        if (*p == '-') { p++; }

        /* parsing int */
        if (*p == '0') {
            p++;
        } else {
            PARSE_DIGITS(p);
        }

        /* parsing frac */
        if (*p == '.') {
            p++;
            PARSE_DIGITS(p);
        }

        /* parsing exp */
        if (*p == 'e' || *p == 'E') {
            p++;
            if (*p == '+' || *p == '-') { p++; }
            PARSE_DIGITS(p);
        }

        this->type = MIJ_NUMBER;
        errno = 0;
        this->number = strtod(c->json, nullptr);
        if (errno == ERANGE && (this->number == HUGE_VAL || this->number == -HUGE_VAL)) {
            return MIJ_PARSE_NUMBER_TOO_LARGE;
        }
        c->json = p;
        return MIJ_PARSE_OK;
    }

#undef PARSE_DIGITS
#undef ISDIGIT

}  // namespace mij_json