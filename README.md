# allocator
Storage is owned by memory resource objects, and containers use those resources when dynamic storage is required. This library also comes with some containers supporting the MemoryResource concept.

The library itself is mostly about the interface, the protocol. We don't necessarily have a huge collection of complex allocation schemes, but you can easily import one from somewhere else, wrap it in this interface, and use the containers written against this.

# Usage
```c++
new_resource r; // resource that allocates with 'new', the most basic one

byte_span s = r.allocate(sizeof(int), align_v<int>); // align storage of proper size and alignment for 'int'
int* p = new(data(s)) int; // construct object on the allocated storage
*p = 0; // object is now valid
r.deallocate(s, align_v<int>); // the storage of 's' is no longer valid, and therefore neither is the 'int' object
```

# What's new?
This MemoryResource removes more than it adds, compared to the standard allocator model. There's less customization points for non-memory operations (ex: construct), and some features can be supported from the start rather than be opted-into through workarounds, such as the allocator propagation customization point (ie: POCCA, POCMA, ...). Some features are not supported at all, such as fancy pointers (they have their use, just not here). Some features aren't needed anymore - we don't do implicit container copies here. Overall, memory resource propagation follows normal C++ propagation rules (copyable memory resources use a copy constructor and a copy assignment operation, moveable memory resources use move operations, etc). Oh, and we deal in bytes only here.

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
TODO :^) Maybe we should talk about relocation.

# Container design
TODO :^)
