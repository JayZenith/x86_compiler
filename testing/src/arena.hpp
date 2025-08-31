#pragma once 

#include "./parser.hpp"
#include <stdlib.h>

class ArenaAllocator{
public:
    ArenaAllocator(size_t bytes)
        : size(bytes)
        {
            buffer = static_cast<std::byte*>(malloc(size));
            offset = buffer; //offset starts at buffer 
        }

    template<typename T> 
    T* alloc(){
        void* aoffset = offset; //cant go from byte* to T*
        offset += sizeof(T); 
        return static_cast<T*>(aoffset); //must typecast from byte* to T*
    }

    //copy constructor now allowed 
    ArenaAllocator(const ArenaAllocator& other) = delete;

    //copy assignment operator not allowed 
    ArenaAllocator& operator=(const ArenaAllocator& other) = delete;

    ~ArenaAllocator(){ free(buffer); }

private:
    size_t size;
    std::byte* buffer;
    std::byte* offset;
};