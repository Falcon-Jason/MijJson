//
// Created by Jason on 2021/1/22.
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
    struct Value::Context {
        const char *json;
    };

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
}