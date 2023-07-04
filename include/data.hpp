#pragma once

#include "object.hpp"

#include <stddef.h>
#include <stdint.h>

namespace love
{
    class Data : public Object
    {
      public:
        static inline Type type = Type("Data", &Object::type);

        virtual ~Data()
        {}

        virtual Data* Clone() const = 0;

        virtual void* GetData() const = 0;

        virtual size_t GetSize() const = 0;
    };
} // namespace love
