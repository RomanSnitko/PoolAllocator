#include <cstddef>
#include <iostream>
#include <span>
#include <vector>

#include "Pool.hpp"
#include "PoolAllocator.hpp"

using MemoryManagement::task2::Pool;
using MemoryManagement::task2::PoolAllocator;

int main()
{
    std::cout << "pool\n";

    const std::size_t pool_shard_size = 1024* 1024;
    const std::size_t pool_chunk_size = 64;

    std::vector<std::byte> pool_mem(pool_shard_size);

    Pool pool;
    pool.init(std::span<std::byte>(pool_mem.data(), pool_mem.size()),
              pool_chunk_size,
              alignof(std::max_align_t));

    std::cout << "capacity" << pool.capacity() << "\n";

    void* a = pool.acquire();
    void* b = pool.acquire();
    void* c = pool.acquire();

    std::cout << a << "\n";
    std::cout << b << "\n";
    std::cout << c << "\n";

    pool.release(b);
    pool.release(c);

    void* x = pool.acquire();
    void* y = pool.acquire();

    std::cout << x << "\n";
    std::cout << y << "\n";

    pool.release(a);
    pool.release(x);
    pool.release(y);

    std::cout << "use " << pool.inUse() << "\n";

    std::cout << "\nallocator\n";

    const std::size_t global_size = 512 *1024 *1024;
    std::vector<std::byte> global_mem(global_size);

    PoolAllocator::Config cfg;
    cfg.shard_size = 256*1024;
    cfg.min_chunk_size = 32;
    cfg.max_chunk_size = 8192;

    PoolAllocator alloc(std::span<std::byte>(global_mem.data(), global_mem.size()), cfg);

    void* p1 = alloc.acquire(10);
    void* p2 = alloc.acquire(40);
    void* p3 = alloc.acquire(100);
    void* p4 = alloc.acquire(1000);
    void* p5 = alloc.acquire(4000);

    std::cout << p1 << "\n";
    std::cout << p2 << "\n";
    std::cout << p3 << "\n";
    std::cout << p4 << "\n";
    std::cout << p5 << "\n";

    alloc.release(p2);
    alloc.release(p4);

    void* q1 = alloc.acquire(40);
    void* q2 = alloc.acquire(1000);

    std::cout << q1 << "\n";
    std::cout << q2 << "\n";

    alloc.release(p1);
    alloc.release(p3);
    alloc.release(p5);
    alloc.release(q1);
    alloc.release(q2);
}
