#pragma once

#include <citro3d.h>

#include "logfile.hpp"
#include "vertex.hpp"

namespace love
{
    using namespace vertex;

    struct DrawBuffer
    {
      public:
        DrawBuffer(size_t size) : info {}, data((Vertex*)linearAlloc(size)), size(size), valid(true)
        {
            BufInfo_Init(&this->info);

            int result = BufInfo_Add(&this->info, (void*)this->data, VERTEX_SIZE, 3, 0x210);

            if (result < 0)
                this->valid = false;
        }

        ~DrawBuffer()
        {
            if (this->data != nullptr)
                linearFree(this->data);
        }

        DrawBuffer(DrawBuffer&&) = delete;

        DrawBuffer(const DrawBuffer&) = delete;

        DrawBuffer& operator=(const DrawBuffer&) = delete;

        Vertex* GetData()
        {
            return this->data;
        }

        C3D_BufInfo* GetBuffer()
        {
            return &this->info;
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
        }

      private:
        C3D_BufInfo info;

        vertex::Vertex* data;
        uint32_t size;

        bool valid;
    };
} // namespace love