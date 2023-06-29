#pragma once

#include <citro3d.h>

#include "logfile.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawBuffer
    {
      public:
        DrawBuffer(size_t size) : info {}, size(size), data(nullptr), valid(true)
        {
            this->data = (vertex::Vertex*)linearAlloc(size);

            BufInfo_Init(&this->info);
            if (BufInfo_Add(&this->info, (void*)this->data, vertex::VERTEX_SIZE, 3, 0x210) < 0)
                this->valid = false;
        }

        vertex::Vertex& operator*()
        {
            return this->data[0];
        }

        vertex::Vertex& operator[](size_t index)
        {
            return this->data[index];
        }

        vertex::Vertex* GetBuffer()
        {
            return this->data;
        }


        ~DrawBuffer()
        {
            linearFree(this->data);
        }

        bool IsValid()
        {
            return this->valid;
        }

        void FlushDataCache()
        {
            GSPGPU_FlushDataCache((void*)this->data, this->size);
        }

        void SetBufInfo() 
        {
            C3D_SetBufInfo(&this->info);
        }

        void Upload(vertex::Vertex* vertices, size_t size)
        {
            std::memcpy(this->data, vertices, size);            
        }

      private:
        C3D_BufInfo info;

        vertex::Vertex* data;
        size_t size;

        bool valid;
    };
} // namespace love
