#pragma once

#include "memory/byte_span.h"

#include <type_traits>

namespace kab
{
	/**
	* 'freelist_resource' is a memory resource which keeps a "list" of fixed-size deallocated blocks.
	* The freelist has an inner resource which provides the actual memory resources, and the freelist manages the freed blocks to avoid actually freeing memory.
	* Smaller allocations are upgraded to the bucket size, and bigger allocations are directly delegated to the inner resource
	*/
	template<typename InnerResource, size_t BlockSize>
	class freelist_resource : InnerResource 
	{
		static_assert(BlockSize >= sizeof(void*), "Come on, give me at least something to work with");

		[[nodiscard]] InnerResource& access_inner() & noexcept { return static_cast<InnerResource&>(*this); }
		[[nodiscard]] InnerResource&& access_inner() && noexcept { return static_cast<InnerResource&&>(*this); }

		struct node 
		{
			node* next;
		};

		node* free_head = nullptr;

	public:
		constexpr freelist_resource() = default;
		freelist_resource(InnerResource r)
			: InnerResource(std::move(r))
		{

		}
		freelist_resource(freelist_resource && rhs) noexcept
			: InnerResource(std::move(rhs).access_inner())
			, free_head(std::exchange(rhs.free_head, nullptr))
		{

		}
		freelist_resource& operator=(freelist_resource && rhs) noexcept
		{
			if (this != &rhs)
			{
				clear();
				access_inner() = std::move(rhs).access_inner();
				free_head = std::exchange(rhs.free_head, nullptr);
			}
			return *this;
		}
		~freelist_resource()
		{
			clear();
		}

		// If the memory fails to allocate, the behavior depends on the inner resource
		[[nodiscard]] byte_span allocate(size_t byte_size)
		{
			if (byte_size <= BlockSize)
			{
				if (free_head == nullptr) // we don't have anything in the freelist, allocate some new stuff
				{
					byte_span block = access_inner().allocate(BlockSize);
					block.size = byte_size;
					return block;
				}
				else
				{
					// Just return the head of the freelist
					node* const head = free_head;
					free_head = head->next;
					return { reinterpret_cast<byte*>(head), byte_size };
				}
			}
			else // we don't handle blocks over our block size
			{
				return access_inner().allocate(byte_size);
			}
		}

		[[nodiscard]] byte_span over_allocate(size_t byte_size)
		{
			if (byte_size <= BlockSize)
			{
				if (free_head == nullptr)
				{
					return access_inner().allocate(BlockSize);
				}
				else
				{
					// Just return the head of the freelist
					node* const head = free_head;
					free_head = head->next;
					return { reinterpret_cast<byte*>(head), BlockSize };
				}
			}
			else
			{
				return access_inner().allocate(byte_size);
			}
		}

		void deallocate(byte_span bytes) noexcept
		{
			// Append the bytes to this freelist
			if (bytes.size <= BlockSize)
			{
				node* const current_head = free_head;
				free_head = new(bytes.data) node{ current_head };
			}
			else
			{
				access_inner().deallocate(bytes);
			}
		}

		// Frees the entire freelist
		// Blocks acquired from an allocation function still need to be passed to 'deallocate' - this won't magically collect all the garbage
		void clear() noexcept
		{
			while (free_head != nullptr)
			{
				node* const head = free_head;
				free_head = head->next;
				access_inner().deallocate({ reinterpret_cast<byte*>(head), BlockSize });
			}
		}

		[[nodiscard]] constexpr bool operator==(freelist_resource const& rhs) const noexcept
		{
			if constexpr (std::is_empty_v<InnerResource>)
			{
				return true;
			}
			else
			{
				return access_inner() == rhs.access_inner();
			}
		}
	};
}