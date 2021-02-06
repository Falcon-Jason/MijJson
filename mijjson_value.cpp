/**
/* @author Jason Cheung
 * @date 2021/1/21.
 */

#include "mijjson.h"

#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdlib>

namespace mijjson {
    Value::~Value() {
        setNull();
    }

    ValueType Value::getType() const {
        return type;
    }

    bool Value::isNull() const {
        return type == MIJ_NULL;
    }

    void Value::setNull() {
        if (this->type == MIJ_STRING) {
            Allocator::free(string);
        }
        this->type = MIJ_NULL;
    }

    bool Value::isBoolean() const {
        return type == MIJ_TRUE || type == MIJ_FALSE;
    }

    bool Value::getBoolean() const {
        assert(this->type == MIJ_TRUE || this->type == MIJ_FALSE);

        return this->type == MIJ_TRUE;
    }

    void Value::setBoolean(bool boolean) {
        setNull();
        this->type = boolean ? MIJ_TRUE : MIJ_FALSE;
    }

    bool Value::isNumber() const {
        return type == MIJ_NUMBER;
    }

    double Value::getNumber() const {
        assert(this->type == MIJ_NUMBER);

        return number;
    }

    void Value::setNumber(double number) {
        setNull();
        this->type = MIJ_NUMBER;
        this->number = number;
    }

    bool Value::isString() const {
        return type == MIJ_STRING;
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
        this->string = Allocator::malloc(length + 1);
        this->length = length;
        memcpy(this->string, str, length);
        this->string[length] = '\0';
        this->type = MIJ_STRING;
    }
}  /* namespace mijjson */