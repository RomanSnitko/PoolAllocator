#include "PoolAllocator.hpp"

#include <bit>
#include <vector>

namespace MemoryManagement::task2
{

PoolAllocator::PoolAllocator(std::span<std::byte> global_memory, Config config)
    : global_memory_(global_memory), config_(config)
{
    shard_count_ = global_memory_.size_bytes() / config_.shard_size;

    control_.resize(shard_count_);

    //min=32 (2^5), max=8192 (2^13)
    const std::size_t bin_count = std::bit_width(config_.max_chunk_size) 
                                    - std::bit_width(config_.min_chunk_size) + 1;

    bin_heads_.assign(bin_count, -1);

    if (shard_count_ == 0)
    {
        free_head_ = -1;
    }
    else
    {
        free_head_ = 0;

        for (std::size_t i = 0; i < shard_count_; ++i)
        {
            ControlBlock& cb = control_[i];
            cb.active = false;
            cb.chunk_size = 0;

            cb.next = (i + 1 < shard_count_) ? i + 1 : -1;
        }
    }
}


void* PoolAllocator::acquire(std::size_t size) noexcept
{
    std::size_t chunk_size = std::bit_ceil(size);

    if (chunk_size < config_.min_chunk_size)
    {
        chunk_size = config_.min_chunk_size;
    }

    if (chunk_size > config_.max_chunk_size)
    {
        return nullptr;
    }

    const std::size_t bin = std::bit_width(chunk_size) - std::bit_width(config_.min_chunk_size);

    int idx = bin_heads_[bin];
    while (idx != -1)
    {
        void* p = control_[idx].pool.acquire();

        if (p != nullptr)
        {
            return p;
        }

        idx = control_[idx].next;
    }

    if (free_head_ == -1)
    {
        return nullptr;
    }

    const int new_idx = free_head_;
    ControlBlock& cb = control_[new_idx];
    free_head_ = cb.next;

    const int shard_i = new_idx;
    std::byte* shard_begin = global_memory_.data() + (shard_i * config_.shard_size);

    cb.pool.init(std::span<std::byte>(shard_begin, 
                                             config_.shard_size),
                                             chunk_size,
                                             alignof(std::max_align_t));

    cb.chunk_size = chunk_size;
    cb.active = true;

    cb.next = bin_heads_[bin];
    bin_heads_[bin] = new_idx;

    return cb.pool.acquire();
}

void PoolAllocator::release(void* ptr) noexcept
{
    if (ptr == nullptr)
    {
        return;
    }

    const std::byte* base = global_memory_.data();
    const std::byte* p = static_cast<const std::byte*>(ptr);

    const std::size_t shard_index =
        static_cast<std::size_t>(p - base) / config_.shard_size;

    control_[shard_index].pool.release(ptr);
}

} // namespace MemoryManagement::task2
