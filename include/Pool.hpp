#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace MemoryManagement::task2
{

class Pool
{
public:
    Pool() = default;
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;

    Pool(Pool&&) noexcept = default;
    Pool& operator=(Pool&&) noexcept = default;

    // bind pool with memory
    void init(std::span<std::byte> shard, std::size_t chunk_size, std::size_t chunk_alignment) noexcept;

    // bind?
    [[nodiscard]] bool isInitialized() const noexcept;

    [[nodiscard]] std::size_t chunkSize() const noexcept;
    [[nodiscard]] std::size_t chunkAlignment() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;

    // how much id acquire
    [[nodiscard]] std::size_t inUse() const noexcept;

    // return free chunk
    [[nodiscard]] void* acquire() noexcept;

    // return back to the pool
    [[nodiscard]] bool release(void* ptr) noexcept;

    // inside the pool memory and at the start of chunk
    [[nodiscard]] bool owns(const void* ptr) const noexcept;
    // can the pool give the chunk
    [[nodiscard]] bool isExhausted() const noexcept;

    // from init
    void reset() noexcept;

private:
    struct FreeNode
    {
        FreeNode* next = nullptr;
    };

    std::byte* base_ = nullptr;
    std::size_t shard_size_ = 0;

    std::size_t chunk_size_ = 0;
    std::size_t chunk_alignment_ = 0;

    // capacity_ = shard_size_ / chunk_size_
    std::size_t capacity_ = 0;
    std::size_t next_unused_ = 0;

    FreeNode* released_head_ = nullptr;
    std::size_t in_use_ = 0;
};

} // namespace mm::task2
