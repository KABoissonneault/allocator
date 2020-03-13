#pragma once

#include "memory/byte_span.h"

namespace kab
{
	namespace detail
	{
		template<size_t Alignment>
		inline byte_span aligned_alloc(std::byte* current, size_t alloc_size, size_t max_alloc)
		{
			size_t const remainder = current % Alignment;
			if (remainder == 0)
			{
				return current;
			}
			size_t const advance = Alignment - remainder;
			if (advance + alloc_size > max_alloc)
			{
				return {}; // allocation failure
			}

			return { current + advance, alloc_size };
		}

		template<typename Alignment=alignof(std::max_align_t)>
		struct default_align_helper
		{
			byte_span operator()(std::byte* current, size_t alloc_size, size_t max_alloc)
			{
				if (alloc_size >= Alignment)
				{
					return aligned_alloc<Alignment>(current, alloc_size, max_alloc);
				}
				else
				{
					return default_align_helper<Alignment / 2>{}(current, alloc_size, max_alloc);
				}
			}
		};

		template<>
		struct default_align_helper<1>
		{
			byte_span operator()(std::byte* current, size_t alloc_size, size_t max_alloc)
			{
				return { current, alloc_size };
			}
		};
	}

	template<size_t Size>
	class array_resource
	{
		std::byte m_buffer[Size];
		std::byte* m_head = m_buffer;
				
		static std::byte* align_up(std::byte* current, size_t alloc_size, size_t max_alloc)
		{
			if (alloc_size >= alignof(std::max_align_t))
			{
				return align_up<alignof(std::max_align_t)>(current, max_alloc);
			}
		}
	public:
		array_resource() = default;
		array_resource(array_resource const&) = delete;
		array_resource& operator=(array_resource const&) = delete;

		/**
		 * Returns the maximum allocation currently possible. Allocation may still fail due to alignment requirements
		 */
		[[nodiscard]] size_t max_alloc() const noexcept
		{
			return std::distance(m_head, std::end(m_buffer));
		}

		[[nodiscard]] byte_span allocate(size_t n)
		{
			std::byte* const current = m_buffer + m_allocated;
			size_t const max_alloc = max_alloc();
			
			byte_span const s = default_align_helper<>(current, n, max_alloc);
			if (s.data == nullptr)
			{
				return nullptr;
			}
			
			m_head = s.data + s.size;
			return s;
		}

		void deallocate(byte_span s) noexcept
		{
			// only deallocate if the top block was freed
			if (s.data + s.size == m_head)
			{
				m_head -= s.size;
			}
		}

		// Deallocates all memory, returning to the initial state
		void release() noexcept
		{
			m_head = m_buffer;
		}
	};
}