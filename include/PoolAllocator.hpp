#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "Pool.hpp"

namespace MemoryManagement::task2
{

class PoolAllocator
{
public:
    struct Config
    {
        std::size_t shard_size = 256 * 1024;
        std::size_t min_chunk_size = 32;
        std::size_t max_chunk_size = 8 * 1024;
    };

public:
    explicit PoolAllocator(std::span<std::byte> global_memory, Config config);

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    [[nodiscard]] void* acquire(std::size_t size) noexcept;
    void release(void* ptr) noexcept;

private:
    struct ControlBlock
    {
        Pool pool;
        std::size_t chunk_size = 0;

        int next = -1;

        bool active = false;
    };

private:
    std::span<std::byte> global_memory_;
    /*
    * shard_begin = global_base + i * shard_size
    * shard_end   = shard_begin + shard_size
    */

    Config config_;

    //global_memory_.size / shard_size.
    std::size_t shard_count_ = 0;

    std::vector<int> bin_heads_;
    /*
    bin_heads_[0] -> pools for 32
    bin_heads_[1] -> pools for 64
    bin_heads_[2] -> pools for 128
    */

    std::vector<ControlBlock> control_;

    int free_head_ = -1;
};

} // namespace MemoryManagement::task2
