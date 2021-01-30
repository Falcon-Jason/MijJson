/**
/* @author Jason Cheung
 * @date 2021/1/21.
 */

#include "mijjson_value.h"

#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdlib>

namespace mijjson {
    Value::~Value() {
        setNull();
    }

    void Value::setNull() {
        if (this->type == MIJ_STRING) {
            Allocator::free(string);
        }
        this->type = MIJ_NULL;
    }

    ValueType Value::getType() const {
        return type;
    }

    bool Value::getBoolean() const {
        assert(this->type == MIJ_TRUE || this->type == MIJ_FALSE);

        return this->type == MIJ_TRUE;
    }

    double Value::getNumber() const {
        assert(this->type == MIJ_NUMBER);

        return number;
    }

    const char *Value::getString() const {
        assert(this->type == MIJ_STRING);

        return string;
    }

    size_t Value::getStringLength() const {
        assert(this->type == MIJ_STRING);

        return length;
    }

    void Value::setBoolean(bool boolean) {
        setNull();
        this->type = boolean ? MIJ_TRUE : MIJ_FALSE;
    }

    void Value::setNumber(double number) {
        setNull();
        this->type = MIJ_NUMBER;
        this->number = number;
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