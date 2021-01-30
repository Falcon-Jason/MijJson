/**
/* @author Jason Cheung
 * @date 2021/1/22.
 */

#include "mijjson_value.h"

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cmath>

namespace mijjson {


    struct Value::Context {
        const char *json;
        char *stack;
        size_t size;
        size_t top;

        void *push(size_t size) {
            char *ret;
            assert(size > 0);
            if (this->top + size >= this->size) {
                this->size = MIJ_PARSE_STACK_INIT_SIZE;
            }
            while (this->top + size >= this->size) {
                this->size += this->size >> 1;
            }
            this->stack = Allocator::realloc(this->stack, this->size);
            ret = this->stack + this->top;
            this->top += size;
            return ret;
        }

        void *pop(size_t size) {
            assert(this->top >= size);
            this->top -= size;
            return this->stack + this->top;
        }

        void push_char(char ch) {
            *static_cast<char *>(push(sizeof(char))) = ch;
        }
    };

    ParseError Value::parse(const char *json) {
        ParseError error;
        Context c{json, nullptr, 0, 0};

        setNull();

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

        assert(c.top == 0);
        Allocator::free(c.stack);

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
            case '\"':
                return parseString(c);
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

    ParseError Value::parseString(Context *c) {
        /* Need Optimized and Refactored */
        const char *p;
        size_t head = c->top;
        assert(*c->json == '\"');
        ++c->json;
        p = c->json;
        for (;;) {
            char ch = *p++;
            switch (ch) {
                case '\\': {
                    char nextCh = *p++;
                    switch (nextCh) {
                        case '\"':
                        case '\\':
                        case '/':
                            c->push_char(nextCh);
                            break;
                        case 'b':
                            c->push_char('\b');
                            break;
                        case 'f':
                            c->push_char('\f');
                            break;
                        case 'n':
                            c->push_char('\n');
                            break;
                        case 'r':
                            c->push_char('\r');
                            break;
                        case 't':
                            c->push_char('\t');
                            break;
                        default:
                            c->top = head;
                            return MIJ_PARSE_INVALID_STRING_ESCAPE;
                    }
                    break;
                }
                case '\"': {
                    size_t len = c->top - head;
                    setString((const char *) c->pop(len), len);
                    c->json = p;
                    return MIJ_PARSE_OK;
                }
                case '\0': {
                    c->top = head;
                    return MIJ_PARSE_MISS_QUOTATION_MARK;
                }
                case '\x01'...'\x1f': {
                    c->top = head;
                    return MIJ_PARSE_INVALID_STRING_CHAR;
                }
                default: {
                    c->push_char(ch);
                }
            }
        }
    }
} /* namespace mijjson */