#pragma once

#include <citro3d.h>

#include "logfile.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawBuffer
    {
      public:
        DrawBuffer(size_t size) : info {}, data(nullptr), valid(true)
        {
            this->data = linearAlloc(size);
            BufInfo_Init(&this->info);
        }

        ~DrawBuffer()
        {
            linearFree(this->data);
        }

        bool IsValid()
        {
            return this->valid;
        }

        void Upload(vertex::Vertex* vertices, size_t size)
        {
            std::memcpy(this->data, (void*)vertices, size);

            if (BufInfo_Add(&this->info, this->data, vertex::VERTEX_SIZE, 3, 0x210) < 0)
                this->valid = false;
        }

      private:
        C3D_BufInfo info;

        void* data;
        bool valid;
    };
} // namespace love