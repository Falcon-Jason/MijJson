/**
/* @author Jason Cheung
 * @date 2021/1/22.
 */

#include "mijjson.h"

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cmath>

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define PARSE_DIGITS(p)                      \
    do {                                     \
        if (ISDIGIT(*p)) {                   \
            do { p++; } while (ISDIGIT(*p)); \
        } else {                             \
            return MIJ_PARSE_INVALID_VALUE;  \
        }                                    \
    } while (0)

namespace mijjson {
    class Parser{
    public:
        static ParseError Parse(Value *v, const char *json);

    private:
        struct Context{
            const char *json;
            char *stack;
            size_t size;
            size_t top;

            void *push(size_t s);
            void *pop(size_t s);
            void push_char(char ch);
        };

        using Allocator = SimpleAllocator;

        Value *v;
        Context c;

        Parser(Value *v, const char *json)
            : v{v}, c{json, nullptr, 0, 0} {}

        ParseError parse();

        ParseError parseValue();

        void parseWhitespaces();

        ParseError parseLiteral(const char *literal,
                                int literal_length, ValueType literal_type);

        ParseError parseNumber();

        ParseError parseString();
    };

    ParseError Parser::Parse(Value *v, const char *json) {
        Parser parser{v, json};
        return parser.parse();
    }

    ParseError Parser::parse() {
        ParseError error;
        v->setNull();

        parseWhitespaces();

        if ((error = parseValue()) == MIJ_PARSE_OK) {
            if (*c.json != '\0' && !isspace(*c.json)) {
                error = MIJ_PARSE_INVALID_VALUE;
            } else {
                parseWhitespaces();
                if (*c.json != '\0') {
                    error = MIJ_PARSE_ROOT_NOT_SINGULAR;
                }
            }
        }

        if (error != MIJ_PARSE_OK) {
            v->setNull();
        }

        assert(c.top == 0);
        Allocator::free(c.stack);

        return error;
    }

    void Parser::parseWhitespaces() {
        auto p = c.json;
        while (isspace(*p)) { p++; }
        c.json = p;
    }

    ParseError Parser::parseValue() {
        switch (*c.json) {
            case 'n':
                return parseLiteral("null", 4, MIJ_NULL);
            case 't':
                return parseLiteral("true", 4, MIJ_TRUE);
            case 'f':
                return parseLiteral("false", 5, MIJ_FALSE);
            case '0' ... '9':
            case '-':
                return parseNumber();
            case '\"':
                return parseString();
            case '\0':
                return MIJ_PARSE_EXPECT_VALUE;
            default:
                return MIJ_PARSE_INVALID_VALUE;
        }
    }

    ParseError Parser::parseLiteral(const char *literal,
                            int literal_length, ValueType literal_type) {
        if (strncmp(c.json, literal, literal_length) != 0) {
            return MIJ_PARSE_INVALID_VALUE;
        }
        c.json += literal_length;
        v->type = literal_type;
        return MIJ_PARSE_OK;
    }

    ParseError Parser::parseNumber() {
        const char *p = c.json;

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

        v->type = MIJ_NUMBER;
        errno = 0;
        v->number = strtod(c.json, nullptr);
        if (errno == ERANGE && (v->number == HUGE_VAL || v->number == -HUGE_VAL)) {
            return MIJ_PARSE_NUMBER_TOO_LARGE;
        }
        c.json = p;
        return MIJ_PARSE_OK;
    }

    ParseError Parser::parseString() {
        /* Need Optimized and Refactored */
        const char *p;
        size_t head = c.top;
        assert(*c.json == '\"');
        ++c.json;
        p = c.json;
        for (;;) {
            char ch = *p++;
            switch (ch) {
                case '\\': {
                    char nextCh = *p++;
                    switch (nextCh) {
                        case '\"':
                        case '\\':
                        case '/':
                            c.push_char(nextCh);
                            break;
                        case 'b':
                            c.push_char('\b');
                            break;
                        case 'f':
                            c.push_char('\f');
                            break;
                        case 'n':
                            c.push_char('\n');
                            break;
                        case 'r':
                            c.push_char('\r');
                            break;
                        case 't':
                            c.push_char('\t');
                            break;
                        default:
                            c.top = head;
                            return MIJ_PARSE_INVALID_STRING_ESCAPE;
                    }
                    break;
                }
                case '\"': {
                    size_t len = c.top - head;
                    v->setString((const char *) c.pop(len), len);
                    c.json = p;
                    return MIJ_PARSE_OK;
                }
                case '\0': {
                    c.top = head;
                    return MIJ_PARSE_MISS_QUOTATION_MARK;
                }
                case '\x01'...'\x1f': {
                    c.top = head;
                    return MIJ_PARSE_INVALID_STRING_CHAR;
                }
                default: {
                    c.push_char(ch);
                }
            }
        }
    }

    void *Parser::Context::push(size_t s) {
        char *ret;
        assert(s > 0);
        if (this->top + s >= this->size) {
            this->size = MIJ_PARSE_STACK_INIT_SIZE;
        }
        while (this->top + s >= this->size) {
            this->size += this->size / 2;
        }
        this->stack = Allocator::realloc(this->stack, this->size);
        ret = this->stack + this->top;
        this->top += s;
        return ret;
    }

    void *Parser::Context::pop(size_t s) {
        assert(this->top >= s);
        this->top -= s;
        return this->stack + this->top;
    }

    void Parser::Context::push_char(char ch) {
        *static_cast<char *>(push(sizeof(char))) = ch;
    }

    ParseError Value::parse(const char *json) {
        return Parser::Parse(this, json);
    }
} /* namespace mijjson */

#undef PARSE_DIGITS
#undef ISDIGIT