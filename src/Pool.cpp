#include "Pool.hpp"

#include <cstddef>

namespace MemoryManagement::task2
{

void Pool::init(std::span<std::byte> shard,std::size_t chunk_size,std::size_t chunk_alignment) noexcept
{
    base_ = shard.data();
    shard_size_ = shard.size_bytes();

    chunk_size_ = chunk_size;
    chunk_alignment_ = chunk_alignment;

    capacity_ = shard_size_ / chunk_size_;

    next_unused_ = 0;
    released_head_ = nullptr;
    in_use_ = 0;
}

bool Pool::isInitialized() const noexcept
{
    return base_ != nullptr;
}

std::size_t Pool::chunkSize() const noexcept
{
    return chunk_size_;
}

std::size_t Pool::chunkAlignment() const noexcept
{
    return chunk_alignment_;
}

std::size_t Pool::capacity() const noexcept
{
    return capacity_;
}

std::size_t Pool::inUse() const noexcept
{
    return in_use_;
}

void* Pool::acquire() noexcept
{
    /*
    two sources of free chunks:
    * released list
    * not issued
    */

    if (released_head_ != nullptr)
    {
        FreeNode* node = released_head_;
        released_head_ = node->next;
        ++in_use_;
        return node;
    }

    if (next_unused_ < capacity_)
    {
        //start of chunk
        std::byte* addr = base_ + (next_unused_ * chunk_size_);
        ++next_unused_;
        ++in_use_;
        return addr;
    }

    return nullptr;
}

bool Pool::release(void* ptr) noexcept
{
    if (!owns(ptr))
    {
        return false;
    }

    FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
    node->next = released_head_;
    released_head_ = node;

    if (in_use_ > 0)
    {
        --in_use_;
    }

    return true;
}

bool Pool::owns(const void* ptr) const noexcept
{
    if (ptr == nullptr || base_ == nullptr)
    {
        return false;
    }

    const std::byte* p = reinterpret_cast<const std::byte*>(ptr);
    // [begin , end)
    const std::byte* begin = base_;
    const std::byte* end = base_ + shard_size_;

    if (p < begin || p >= end)
    {
        return false;
    }

    //how much byte pointer will be off the beginning of the pool
    const std::size_t offset = static_cast<std::size_t>(p - begin);
    return (offset % chunk_size_) == 0;
}

bool Pool::isExhausted() const noexcept
{
    return released_head_ == nullptr && next_unused_ == capacity_;
}

void Pool::reset() noexcept
{
    next_unused_ = 0;
    released_head_ = nullptr;
    in_use_ = 0;
}

} // namespace MemoryManagement::task2
