# allocator
A simple and extensible interface for managing memory and its allocation.

Storage is owned by memory resource objects, and containers use those resources when dynamic storage is required. This library also comes with some containers supporting the MemoryResource concept.

The library itself is mostly about the interface, the protocol. We don't necessarily have a huge collection of complex allocation schemes, but you can easily import one from somewhere else, wrap it in this interface, and use the containers written against it.

# Usage
```c++
new_resource r; // resource that allocates with 'new', the most basic one

byte_span s = r.allocate(sizeof(int), align_v<int>); // align storage of proper size and alignment for 'int'
int* p = new(data(s)) int; // construct object on the allocated storage
*p = 0; // object is now valid
r.deallocate(s, align_v<int>); // the storage of 's' is no longer valid, and therefore neither is the 'int' object

static_resource<1024> buffer; // resource equivalent to byte[1024], on the stack
using buffer_ref = resource_reference<static_resource<1024>>;
vector<int, buffer_ref> v(buffer); // vector holds only a pointer to the buffer
v.reserve(1024/sizeof(int)); // allocate the full buffer
v.push_back(0); // no reallocations until full. try not to overflow :)
```

# What's new?
The basic concept of this library is the memory resource (ie: MemoryResource). The memory resource owns the memory, and has functionality to allocate parts of it to an external user.

Compared to the standard allocator model, memory resource removes more than it adds. There's less customization points for non-memory operations (ex: construct). Some features can be supported from the start rather than be opted-into through workarounds, such as the allocator propagation customization point (ie: POCCA, POCMA, ...). Some features are not supported at all, such as fancy pointers (they have their use, just not here). Some features aren't needed anymore - we don't do implicit container copies here, so we don't need to know how you propagate on copy construction or assignment. Overall, memory resource propagation follows normal C++ propagation rules (copyable memory resources use a copy constructor and a copy assignment operation, moveable memory resources use move operations, etc). Oh, and we deal in bytes only here.

But there's also some new features. First, allocation functions return spans rather than single pointers, which doesn't change much in itself. Deallocation functions also take a span as input rather than just a pointer. This means resources don't have to track internally the size of the storage: the user (usually a container) should already have this information. Alignment is also provided both on allocation and deallocation, so that the allocator does not have to figure this out on its own either.

Another benefit of the byte span on allocation is the possibility for another kind of allocation, which we call here "over-allocate" (ie: `over_allocate`). When used, the memory resource may choose to return more storage than requested, and the user can make use of that extra storage to get the "real capacity" of the allocated memory. This is usually done for memory resources that would allocate that extra memory regardless, and does not have to be supported by allocators that don't have any preferred allocation sizes. On deallocation, the user gives back the span _with the over-allocated size_ to `over_deallocate` (or `deallocate` if `over_deallocate` does not exist), again so that the allocator does not have to track the parameters of the requested allocation internally. Benefits of this approach compared to alternatives will be discussed later.

Hopefully with this model, it should be easy to write memory resources without needing to carefully pick from a dozen customization points, and without having to carry around extra runtime data to support the interface. It should also be easy to compose those memory resources, so that memory resource wrappers can easily provide some high-level functionality without having to, again, implement tons of customization points or add unnecessary bookkeeping. It should also be easy enough to implement containers using the memory resource, though the syntactic convience of memory resource is not a primary goal: they should be used primarily in reusable (and well-tested) container components, rather than used manually in normal procedures.

# Building
The main project is currently header-only; just add `/include` to your build system, and start using the provided templates.

There's a Visual Studio project for unit tests. No CMake at the moment, but contributions are welcome.

# Design goals
**Primary goals**
- Simplicity. Some features benefit few users while being an inconvenience to many, so those features should find another project.
- No runtime overhead for information the user should already have
- Design for ease-of-use in library code. Business-logic code should not use this beyond instantiating a resource object and passing it to a container
- No performance pessimizations. If something can be implemented more efficiently without compromising correctness or contriving the interface, it should be done.
- A feature can only be called as such if it's tested properly. If there's no automated tests associated with it, it's not a feature
- Design for composition and reuse. More reuse means less testing required.

**Secondary goals**
- Do not impose a strong compilation overhead. Minimize dependencies, minimize meta-programming, and give means for people to instantiate components where they want.
- Do not implement a feature that can already easily be found somewhere else
- Try to adopt standard C++ conventions for names when they achieve the same thing (ex: data, size, begin, capacity, ...)

**Non-goals** (we do not aim to do those)
- Fit everyone's needs
- Provide ABI stability on every component. ABI stability will be explicitly documented in-source when offered
- Obfuscate source heavily so that some compilers on some version can have better codegen in some use cases

# Design decisions
**Sized deallocation**
Sized deallocation, meaning passing the size of the storage to deallocate, is a must. Maybe when `malloc` was invented, people preferred having to carry only a single pointer around. Nowadays, passing extra information is not an issue to the programmer or the compiler, and is usually not an obstacle to optimizations.

We argue that the user of the resource should always know the size of the allocation anyway. Containers such as `vector` already stores the runtime value of the storage in the form of `capacity`. Other containers, such as node-based ones like `list` or `map`, only allocate in constant sizes (ex: `sizeof(node)`), and therefore can give back this value to the resource without adding runtime overhead.

Sized deallocation especially shines in composed resources. We have an example of the issue with unsized deallocation in the default `new[]` operator. When constructing the array object, the operator calls `operator new[]` to allocate memory, which will most likely track the size of the allocation. On top of the, `new[]` needs itself to allocate extra storage to store the number of elements in the array, as it will need to know how many elements to destroy. Should we instead provide size of deallocation, the operator could figure out the number of elements to destroy, and in addition provide the size of the storage to `operator delete[]`. No extra data necessary at all levels.

**Alignment**
Alignment is usually a second consideration for allocation. Bigger alignments are not a problem for correctness, and all types in C++ are natively aligned to `alignof(long double)`, so imposing this alignment on all allocations shouldn't be a problem for most users, right? 

This is of course a limitation for users that need over-alignment (ie: alignments bigger than `alignof(long double)`). Typically, allocators offer an alternate interface with an alignment input (ex: `aligned_alloc`, `operator new(..., align_val_t)`, ...). However, those interfaces usually still align minimally to at least `alignof(void*)`, if not bigger. Also, having a separate interface is extra complexity for both the user and the memory resource implementer. As a result, we always provide an alignment parameter, knowing that:

- Our target users can always provide this information with a constant, and not be stuck with tracking this value at runtime
- Memory resources don't have to support over-alignment if it's an issue, though they should document those limitations so that they're not misused
- Having a high default alignment and ignoring the requested parameter is not an issue for correctness

We also provide the requested alignment back on deallocation. Resources may have special allocation mechanisms depending on the initially requested alignment, and we don't want the resource to have to internally track or compute the alignment of the storage on deallocation.

The ability for resources to align under the default value can sometimes be beneficial for internal fragmentation. This can be relevant for, say, UTF8 strings, which may use lots of arbitrary-sized, 1-byte aligned allocations. Those allocations should be able to fit anywhere.

**Over-allocate**
It's no secret that some allocation schemes, such as block-based allocations, may sometimes end up allocating more than requested (ex: in order to avoid internal fragmentation). On the other end, some containers like `vector` need to dynamically grow a given storage as the value is built. If there's more room to the storage than initially requested, `vector` should be able to use that information to avoid redundant reallocations. There are a few approaches to achieve this

- `realloc` is found on many allocator APIs. When one needs more storage, the old storage is passed to `realloc` along with a new required capacity, and the implementation can decide whether the current storage fits or whether a new one is required. This is a bit too high-level as an operation for an allocator, as it has a pretty specific use case in mind.
- `malloc_usable_size` exists on many implementations of malloc-like resources. After a pointer to storage is returned, the user can query the actual capacity of the returned storage through a separate function call. With this, a container could use that real capacity to avoid prematurely reallocating. This is a better building block, as one could easily reimplement `realloc` given a "usable size" function, but not the other way around. However, it assumes that the allocator can later find the size information given a pointer to storage, which may be too constraining for some resources. 

As a result, we choose simply return the actual size on allocation. But why have a separate "over-allocate" interface, when we could simply return the real size on the normal allocate? Primarily, the resource may need to tell whether the size passed to deallocation is the "real size" or the "requested size". Also, the resource may choose to allocate differently, when over-allocations are requested. 

# C++ interface
**ByteSpan**
```c++
using byte = kab::byte;
using size_t = decltype(sizeof(0));
// byte_span can be anything that supports non-member data and size which returns a contiguous byte range
struct byte_span {
  ...
};

byte* data(byte_span);
size_t size(byte_span);
```

**MemoryResource**
```
using align_t = kab::align_t;
struct memory_resource {
  ByteSpan allocate(size_t n, align_t a);
  ByteSpan over_allocate(size_t n, align_t a); // optional
  void deallocate(ByteSpan s, align_t a);
  void over_deallocate(ByteSpan s, align_t a); // optional, even if over_allocate is provided
};
```

Containers may impose extra requirements on memory resources. 

# Container design
TODO :^)
