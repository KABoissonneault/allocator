#pragma once

#include <iterator>

#include "kaballoc/memory/resource.h"
#include "kaballoc/core/atomic_op.h"

namespace kab
{
	template<typename ElementT, typename ResourceT>
	auto array_value<ElementT, ResourceT>::new_control(ResourceT& r, size_t size) -> control*
	{
		const auto alloc_size = sizeof(control) + (size - 1) * sizeof(ElementT);
		byte_span const s = r.allocate(alloc_size, align_v<control>);
		auto const c = new(s.data) control;
		c->count = 1;
		c->size = size;
		
		// don't construct the elements here
		
		return c;
	}

	template<typename ElementT, typename ResourceT>
	auto array_value<ElementT, ResourceT>::acquire_control(control* c) noexcept -> control*
	{
		if (c != nullptr)
		{
			KAB_ATOMIC_FETCH_INC_SIZE_T_RELAXED(c->count);
		}
		return c;
	}

	template<typename ElementT, typename ResourceT>
	void array_value<ElementT, ResourceT>::release_control(ResourceT& r, control* c)
	{
		if (c == nullptr)
		{
			return;
		}

		if (KAB_ATOMIC_FETCH_DEC_SIZE_T_RELEASE(c->count) == 1)
		{
			KAB_ATOMIC_FENCE_ACQUIRE();
			
			// destroy the elements
			if constexpr (!std::is_trivially_destructible_v<ElementT>)
			{
				std::destroy_n(c->fam, c->size);
			}
			// the rest of the control is trivially destructible, so nothing to do there
	
			// deallocate the memory
			auto const alloc_size = sizeof(control) + (c->size - 1) * sizeof(ElementT);
			r.deallocate({ reinterpret_cast<byte*>(c), alloc_size, }, align_v<control>);
		}
	}

	template<typename ElementT, typename ResourceT>
	array_value<ElementT, ResourceT>::array_value(ResourceT r) noexcept
		: ResourceT(r)
	{

	}

	template<typename ElementT, typename ResourceT>
	array_value<ElementT, ResourceT>::array_value(array_value const& rhs) noexcept
		: ResourceT(rhs.access_resource())
		, m_control(acquire_control(rhs.m_control))
		, m_data(rhs.m_data)
		, m_end(rhs.m_end)
	{
		
	}

	template<typename ElementT, typename ResourceT>
	array_value<ElementT, ResourceT>::array_value(array_value && rhs) noexcept
		: ResourceT(std::move(rhs).access_resource())
		, m_control(std::exchange(rhs.m_control, nullptr))
		, m_data(std::exchange(rhs.m_data, nullptr))
		, m_end(std::exchange(rhs.m_end, nullptr))
	{

	}

	template<typename ElementT, typename ResourceT>
	auto array_value<ElementT, ResourceT>::operator=(array_value const& rhs) noexcept -> array_value&
	{
		if (this != &rhs)
		{
			release_control(access_resource(), m_control);
			
			access_resource() = rhs.access_resource();
			m_control = acquire_control(rhs.m_control);
			m_data = rhs.m_data;
			m_end = rhs.m_end;
		}
		return *this;
	}

	template<typename ElementT, typename ResourceT>
	auto array_value<ElementT, ResourceT>::operator=(array_value && rhs) noexcept -> array_value&
	{
		if (this != &rhs)
		{
			release_control(access_resource(), m_control);

			access_resource() = std::move(rhs).access_resource();
			m_control = std::exchange(rhs.m_control, nullptr);
			m_data = std::exchange(rhs.m_data, nullptr);
			m_end = std::exchange(rhs.m_end, nullptr);
		}
		return *this;
	}

	template<typename ElementT, typename ResourceT>
	array_value<ElementT, ResourceT>::~array_value()
	{
		release_control(access_resource(), m_control);
	}

	template<typename ElementT, typename ResourceT>
	void array_value<ElementT, ResourceT>::swap(array_value & rhs) noexcept
	{
		using std::swap;
		swap(access_resource(), rhs.access_resource());
		swap(m_control, rhs.m_control);
		swap(m_data, rhs.m_data);
		swap(m_end, rhs.m_end);
	}
	
	template<typename ElementT, typename ResourceT>
	template<typename SizedRangeT>
	auto array_value<ElementT, ResourceT>::assign(SizedRangeT && r) -> array_value&
	{
		using std::size;
		using std::begin;
		using std::end;

		release_control(access_resource(), m_control);

		auto const range_size = size(r);
		if (range_size == 0)
		{
			return *this;
		}

		m_control = new_control(access_resource(), range_size);
		m_data = m_control->fam;
		m_end = m_data + range_size;

		std::uninitialized_copy(begin(r), end(r), m_data);

		return *this;
	}
}