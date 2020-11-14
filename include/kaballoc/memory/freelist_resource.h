#pragma once

#include "kaballoc/memory/resource.h"
#include "kaballoc/memory/byte_span.h"

#include <type_traits>
#include <utility>

namespace kab
{
	/**
	* 'freelist_resource' is a memory resource which keeps a "list" of fixed-size deallocated blocks.
	* The freelist has an inner resource which provides the actual memory resources, and the freelist manages the freed blocks to avoid actually freeing memory.
	* Smaller allocations are upgraded to the bucket size, and bigger allocations are directly delegated to the inner resource
	*/
	template<typename InnerResource, size_t BlockSize, align_t Alignment = static_cast<align_t>(BlockSize)>
	class freelist_resource : InnerResource 
	{
		static_assert(BlockSize >= sizeof(void*), "Come on, give me at least something to work with");
		static_assert(is_power_of_two(BlockSize), "Powers of two only :)");

		[[nodiscard]] InnerResource& access_inner() & noexcept { return static_cast<InnerResource&>(*this); }
		[[nodiscard]] InnerResource&& access_inner() && noexcept { return static_cast<InnerResource&&>(*this); }

		struct node 
		{
			node* next;
		};

		node* free_head = nullptr;

		static constexpr align_t get_target_alignment()
		{
			return Alignment;
		}

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

		/**
		 * allocate
		 *
		 * If 'byte_size' is smaller or equal to BlockSize, a block from the freelist is returned to the caller if there's any. 
		 * Otherwise, a block of BlockSize bytes is allocated from the inner resource.
		 * If 'byte_size' is bigger than BlockSize, a block is always allocated from the inner resource.
		 * If the freelist cannot fulfill the alignment requirement with the freelist, it may also allocate from the inner resource.
		 *
		 * On allocation failure, the behavior depends on the inner resource. This resource has basic exception guarantee.
		 */
		[[nodiscard]] byte_span allocate(size_t byte_size, align_t alignment)
		{
			byte_span s = over_allocate(byte_size, alignment);
			s.size = byte_size;
			return s;
		}

		/**
		 * over_allocate
		 *
		 * Like allocate, but the returned size can be bigger than the requested size
		 */
		[[nodiscard]] byte_span over_allocate(size_t byte_size, align_t requested_alignment)
		{
			const bool over_aligned = requested_alignment > get_target_alignment();

			if (byte_size > BlockSize)
			{
				return access_inner().allocate(byte_size, over_aligned ? requested_alignment : get_target_alignment());
			}

			if (over_aligned) // we can't fulfill over-aligned requests from the freelist
			{
				return access_inner().allocate(BlockSize, requested_alignment);
			}

			if (free_head == nullptr) // we don't have anything in the freelist, allocate some new stuff
			{
				return access_inner().allocate(BlockSize, get_target_alignment());
			}

			// Just return the head of the freelist
			node* const head = free_head;
			free_head = head->next;
			return { reinterpret_cast<byte*>(head), BlockSize };
		}

		/**
		 * deallocate
		 *
		 * To be called with the span returned from the allocation function, and the requested alignment
		 */
		void deallocate(byte_span bytes, align_t alignment) noexcept
		{
			// If the memory was over-sized or over-aligned, we can't put the block on the freelist
			// This is because when the freelist is cleared, we assume that the allocation was made with "BlockSize" and target alignment as the parameters
			// If the allocation was made with other values, we can't deallocate it reliably on freelist clear.
			// Therefore, deallocate it here
			if (bytes.size > BlockSize || alignment > get_target_alignment())
			{
				return access_inner().deallocate(bytes, alignment);
			}

			// Push this block on top of the freelist
			node* const current_head = free_head;
			free_head = new(bytes.data) node{ current_head };
		}

		// Frees the entire freelist
		// Blocks acquired from an allocation function still need to be passed to 'deallocate' - this won't magically collect all the garbage
		void clear() noexcept
		{
			while (free_head != nullptr)
			{
				node* const head = free_head;
				free_head = head->next;
				access_inner().deallocate({ reinterpret_cast<byte*>(head), BlockSize }, get_target_alignment());
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