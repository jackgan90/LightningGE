#include <iterator>
#include "tbb/mutex.h"
#include "D3D12DescriptorHeapManager.h"
#include "Renderer.h"
#include "Logger.h"

namespace {
	using Mutex = tbb::mutex;
	using MutexLock = Mutex::scoped_lock;
	static Mutex mtxHeap;
}

namespace Lightning
{
	namespace Render
	{
		D3D12DescriptorHeapManager::D3D12DescriptorHeapManager()
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;++i)
			{
				sIncrementSizes[i] = device->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
				for (auto j = 0;j < RENDER_FRAME_COUNT;++j)
				{
					for (auto k = 0;k < 2;++k)
					{
						mTransientHeaps[j][i][k].heapStore = nullptr;
						mTransientHeaps[j][i][k].handles = nullptr;
					}
				}
			}
		}

		D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager()
		{

		}

		void D3D12DescriptorHeapManager::ReserveTransientDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			assert(count > 0 && "descriptor count must be positive!");
			auto frameResourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto i = shaderVisible ? 1 : 0;
			auto& transientHeap = mTransientHeaps[frameResourceIndex][type][i];
			if (transientHeap.heapStore && transientHeap.descriptorCount >= count)
			{
				transientHeap.allocCount = 0;
				transientHeap.offset = 0;
				return;
			}
			delete transientHeap.heapStore;
			delete[] transientHeap.handles;
			auto res = CreateHeapStore(type, shaderVisible, count);
			transientHeap.heapStore = std::get<1>(res);
			transientHeap.handles = new DescriptorHeapAllocation[count];
			transientHeap.descriptorCount = count;
			transientHeap.allocCount = 0;
			transientHeap.offset = 0;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool transient)
		{
			if (transient)
			{
				return AllocateTransientHeap(type, shaderVisible, count);
			}
			else
			{
				return AllocatePersistentHeap(type, shaderVisible, count);
			}
		}

		DescriptorHeap* D3D12DescriptorHeapManager::AllocateTransientHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			assert(count > 0);
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto i = shaderVisible ? 1 : 0;
			auto& transientHeap = mTransientHeaps[resourceIndex][type][i];
			CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle(transientHeap.heapStore->CPUHandle);
			CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle(transientHeap.heapStore->GPUHandle);
			auto offset = transientHeap.offset.fetch_add(count, std::memory_order_relaxed);
			CPUHandle.Offset(offset * transientHeap.heapStore->incrementSize);
			GPUHandle.Offset(offset * transientHeap.heapStore->incrementSize);
			
			auto handleIndex = transientHeap.allocCount.fetch_add(1, std::memory_order_relaxed);
			auto heap = &transientHeap.handles[handleIndex];
			heap->CPUHandle = CPUHandle;
			heap->GPUHandle = GPUHandle;
			heap->incrementSize = transientHeap.heapStore->incrementSize;
			heap->pStore = transientHeap.heapStore;
			return heap;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::AllocatePersistentHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			MutexLock lock(mtxHeap);
			auto i = shaderVisible ? 1 : 0;
			if (mPersistentHeaps[type][i].empty())
			{
				auto store = CreateHeapStore(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE);
				mPersistentHeaps[type][i].push_back(std::get<1>(store));
			}
			auto res = TryAllocatePersistentHeap(mPersistentHeaps[type][i], count);
			if (std::get<0>(res))
				return std::get<1>(res);
			//if reach here,the existing heaps can not allocate more descriptors, so just allocate a new heap
			auto store = CreateHeapStore(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE);
			mPersistentHeaps[type][i].push_back(std::get<1>(store));
			res = TryAllocatePersistentHeap(mPersistentHeaps[type][i], count);
			return std::get<1>(res);
		}

		std::tuple<bool, D3D12DescriptorHeapManager::DescriptorHeapStore*> D3D12DescriptorHeapManager::CreateHeapStore(
			D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount)
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			auto res = std::make_tuple(false, new DescriptorHeapStore);
			auto& heapStore = std::get<1>(res);
			heapStore->desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapStore->desc.NodeMask = 0;
			heapStore->desc.NumDescriptors = descriptorCount;
			heapStore->desc.Type = type;
			heapStore->heap = device->CreateDescriptorHeap(&heapStore->desc);
			if (!heapStore->heap)
			{
				LOG_ERROR("Failed to create d3d12 descriptor heap!type:%d, flags:%d, number of descriptors:%d", 
					heapStore->desc.Type, heapStore->desc.Flags, heapStore->desc.NumDescriptors);
				return res;
			}
			heapStore->freeDescriptors = descriptorCount;
			heapStore->freeIntervals.emplace_back(std::make_tuple(0, descriptorCount));
			heapStore->CPUHandle = heapStore->heap->GetCPUDescriptorHandleForHeapStart();
			heapStore->GPUHandle = heapStore->heap->GetGPUDescriptorHandleForHeapStart();
			heapStore->incrementSize = GetIncrementSize(type);
			
			std::get<0>(res) =  true;
			return res;
		}

		std::tuple<bool, DescriptorHeap*> D3D12DescriptorHeapManager::TryAllocatePersistentHeap(std::vector<DescriptorHeapStore*>& heapList, UINT count)
		{
			//loop over existing heap list reversely and try to allocate from it
			for (long long i = heapList.size() - 1; i >= 0;i--)
			{
				auto res = TryAllocatePersistentHeap(heapList[i], count);
				if (std::get<0>(res))
					return res;
			}
			return std::make_tuple<bool, DescriptorHeap*>(false, nullptr);
		}

		std::tuple<bool, DescriptorHeap*> D3D12DescriptorHeapManager::TryAllocatePersistentHeap(DescriptorHeapStore* heapStore, UINT count)
		{
			auto res = std::make_tuple<bool, DescriptorHeap*>(false, nullptr);
			if(count > heapStore->freeDescriptors)
				return res;
			for (auto it = heapStore->freeIntervals.begin(); it != heapStore->freeIntervals.end();++it)
			{
				auto left = std::get<0>(*it);
				auto right = std::get<1>(*it);
				auto descriptorCount = right - left;
				if (descriptorCount >= count)
				{
					std::tuple<UINT, UINT> interval;
					//split this interval into 2 intervals and mark left interval as used
					if (descriptorCount == count)
					{
						//no other space for a descriptor, just remove this interval
						heapStore->freeIntervals.erase(it);
						std::get<0>(interval) = left;
						std::get<1>(interval) = right;
					}
					else
					{
						//there's still some space for at least a descriptor,change the interval end points
						*it = std::make_tuple(left + count, right);
						std::get<0>(interval) = left;
						std::get<1>(interval) = left + count;
					}
					heapStore->freeDescriptors -= count;
					std::get<0>(res) = true;
					auto heapAllocation = new DescriptorHeapAllocation;
					std::get<1>(res) = heapAllocation;
					CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle(heapStore->CPUHandle);
					CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle(heapStore->GPUHandle);
					CPUHandle.Offset(left * heapStore->incrementSize);
					GPUHandle.Offset(left * heapStore->incrementSize);
					heapAllocation->CPUHandle = CPUHandle;
					heapAllocation->GPUHandle = GPUHandle;
					heapAllocation->incrementSize = heapStore->incrementSize;
					heapAllocation->interval = interval;
					heapAllocation->pStore = heapStore;
					break;
				}
			}
			return res;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeap* pHeap)
		{
			assert(pHeap != nullptr && "pHeap must be a valid heap pointer!");
			Deallocate(static_cast<DescriptorHeapAllocation*>(pHeap));
			delete pHeap;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeapAllocation *pHeapEx)
		{
			auto pHeapStore = pHeapEx->pStore;
			pHeapStore->freeDescriptors += std::get<1>(pHeapEx->interval) - std::get<0>(pHeapEx->interval);
			if (pHeapStore->freeIntervals.empty())
				pHeapStore->freeIntervals.push_back(pHeapEx->interval);
			else
			{
				//insert new interval to appropriate position.If adjacent intervals can be joined,then join them
				auto prevIt = pHeapStore->freeIntervals.end();
				auto nextIt = pHeapStore->freeIntervals.end();
				//first loop intervals and find a gap that can be fit the new interval
				for (auto currIt = pHeapStore->freeIntervals.begin();currIt != pHeapStore->freeIntervals.end();++currIt)
				{
					if(std::get<0>(*currIt) >= std::get<1>(pHeapEx->interval))
					{
						nextIt = currIt;
						break;
					}
					prevIt = currIt;
				}
				if (nextIt == pHeapStore->freeIntervals.end())//no nextIt means append to back
				{
					if (prevIt == pHeapStore->freeIntervals.end())//prevIt also end, which means there's no free interval
					{
						pHeapStore->freeIntervals.push_back(pHeapEx->interval);
					}
					else
					{
						//try to join previous interval with the new interval
						if (std::get<1>(*prevIt) == std::get<0>(pHeapEx->interval))
						{
							std::get<1>(*prevIt) = std::get<1>(pHeapEx->interval);	//joinable
						}
						else
						{
							auto backIt = std::back_inserter(pHeapStore->freeIntervals);
							*backIt = pHeapEx->interval;
						}
					}
				}
				else	//insert between two adjacent intervals.Try to merge if possible
				{
					auto currIt = pHeapStore->freeIntervals.insert(nextIt, pHeapEx->interval);
					if (prevIt != pHeapStore->freeIntervals.end() && std::get<1>(*prevIt) == std::get<0>(pHeapEx->interval))
					{
						std::get<1>(*prevIt) = std::get<1>(pHeapEx->interval);
						pHeapStore->freeIntervals.erase(currIt);
						currIt = prevIt;
					}
					//here must recalculate nextIt,because it may be invalidated by previous code
					auto oldCurrIt = currIt;
					auto nextIt = ++currIt;
					if (std::get<1>(*oldCurrIt) == std::get<0>(*nextIt))
					{
						std::get<1>(*oldCurrIt) = std::get<1>(*nextIt);
						pHeapStore->freeIntervals.erase(nextIt);
					}
				}
			}
		}

		UINT D3D12DescriptorHeapManager::GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
		{
			return sIncrementSizes[type];
		}

		void D3D12DescriptorHeapManager::Clear()
		{
				//TODO : clear frame transient heap
			for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
			{
				for (auto j = 0;j < RENDER_FRAME_COUNT;++j)
				{
					for (auto k = 0; k < 2; ++k)
					{
						if (mTransientHeaps[j][i][k].heapStore)
						{
							delete mTransientHeaps[j][i][k].heapStore;
						}

						if (mTransientHeaps[j][i][k].handles)
						{
							delete[] mTransientHeaps[j][i][k].handles;
						}
						
					}
				}
				for (auto j = 0;j < 2;++j)
				{
					for (auto p : mPersistentHeaps[i][j])
					{
						delete p;
					}
					mPersistentHeaps[i][j].clear();
				}
			}
		}

		ComPtr<ID3D12DescriptorHeap> D3D12DescriptorHeapManager::GetHeap(DescriptorHeap* pHeap)const
		{
			auto pHeapEx = static_cast<DescriptorHeapAllocation*>(pHeap);
			return pHeapEx->pStore->heap;
		}
	}
}