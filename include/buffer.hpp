#pragma once

#include <citro3d.h>

#include "logfile.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawBuffer
    {
      public:
        DrawBuffer(size_t size) : info {}, data(nullptr), size(size), valid(true)
        {
            this->data = (vertex::Vertex*)linearAlloc(size);

            BufInfo_Init(&this->info);

            int result = BufInfo_Add(&this->info, (void*)this->data, vertex::VERTEX_SIZE, 3, 0x210);

            if (result < 0)
                this->valid = false;
        }

        vertex::Vertex* GetBuffer()
        {
            return this->data;
        }

        DrawBuffer(DrawBuffer&&) = delete;

        DrawBuffer(const DrawBuffer&) = delete;

        DrawBuffer& operator=(const DrawBuffer&) = delete;

        ~DrawBuffer()
        {
            if (this->data != nullptr)
                linearFree(this->data);
        }

        bool IsValid()
        {
            return this->valid;
        }

        void FlushDataCache()
        {
            Result result = GSPGPU_FlushDataCache((void*)this->data, this->size);

            if (R_FAILED(result))
                this->valid = false;
            else
                C3D_SetBufInfo(&this->info);
        }

      private:
        C3D_BufInfo info;

        vertex::Vertex* data;
        uint32_t size;

        bool valid;
    };
} // namespace love
