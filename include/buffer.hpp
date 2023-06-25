#pragma once

#include <citro3d.h>

#include "logfile.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawBuffer
    {
      public:
        DrawBuffer(size_t size)
        {
            this->data = linearAlloc(size);

            this->info = C3D_GetBufInfo();
            BufInfo_Init(this->info);
        }

        ~DrawBuffer()
        {
            linearFree(this->data);
        }

        void Upload(vertex::Vertex* vertices, size_t size)
        {
            std::memcpy(this->data, (void*)vertices, size);

            if (BufInfo_Add(info, this->data, vertex::VERTEX_SIZE, 3, 0x210) < 0)
                LOG("Failed to add buffer.");
        }

      private:
        void* data;
        C3D_BufInfo* info;
    };
} // namespace love