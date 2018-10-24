#include <iterator>
#include <mutex>
#include "d3d12descriptorheapmanager.h"
#include "renderer.h"
#include "logger.h"

namespace {
	static std::mutex mtxHeap;
}

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		D3D12DescriptorHeapManager::D3D12DescriptorHeapManager()
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;++i)
			{
				sIncrementSizes[i] = device->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
			}
		}

		D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager()
		{
			LOG_INFO("Descriptor heap manager destruct!");
		}

		DescriptorHeap* D3D12DescriptorHeapManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool frameTransient)
		{
			if (frameTransient)
			{
				return AllocateFrameHeap(type, shaderVisible, count);
			}
			else
			{
				return AllocatePersistentHeap(type, shaderVisible, count);
			}
		}

		DescriptorHeap* D3D12DescriptorHeapManager::AllocateFrameHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			auto& frameHeap = *mFrameHeaps;
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto i = shaderVisible ? 1 : 0;
			auto createNewHeap{ false };
			if (frameHeap.heaps[resourceIndex][type][i].empty())
				createNewHeap = true;

			if (!createNewHeap)
			{
				auto& last = frameHeap.heaps[resourceIndex][type][i].back();
				if (frameHeap.next[resourceIndex][type][i] + count >= last->freeDescriptors)
					createNewHeap = true;
			}

			if (createNewHeap)
			{
				auto res = CreateHeapStore(type, shaderVisible,
					count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE);
				frameHeap.heaps[resourceIndex][type][i].push_back(std::get<1>(res));
				frameHeap.next[resourceIndex][type][i] = 0;
			}

			auto& last = frameHeap.heaps[resourceIndex][type][i].back();
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(last->cpuHandle);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(last->gpuHandle);
			cpuHandle.Offset(frameHeap.next[resourceIndex][type][i] * last->incrementSize);
			gpuHandle.Offset(frameHeap.next[resourceIndex][type][i] * last->incrementSize);
			auto heap = new DescriptorHeapEx;
			heap->cpuHandle = cpuHandle;
			heap->gpuHandle = gpuHandle;
			heap->incrementSize = last->incrementSize;
			heap->pStore = last;

			frameHeap.next[resourceIndex][type][i] += count;
			frameHeap.allocations[resourceIndex][type][i].push_back(heap);
			return heap;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::AllocatePersistentHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			std::lock_guard<std::mutex> lock(mtxHeap);
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

		container::tuple<bool, D3D12DescriptorHeapManager::DescriptorHeapStore*> D3D12DescriptorHeapManager::CreateHeapStore(
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
			heapStore->cpuHandle = heapStore->heap->GetCPUDescriptorHandleForHeapStart();
			heapStore->gpuHandle = heapStore->heap->GetGPUDescriptorHandleForHeapStart();
			heapStore->incrementSize = GetIncrementSize(type);
			
			std::get<0>(res) =  true;
			return res;
		}

		container::tuple<bool, DescriptorHeap*> D3D12DescriptorHeapManager::TryAllocatePersistentHeap(container::vector<DescriptorHeapStore*>& heapList, UINT count)
		{
			//loop over existing heap list reversely and try to allocate from it
			for (int i = heapList.size() - 1; i >= 0;i--)
			{
				auto res = TryAllocatePersistentHeap(heapList[i], count);
				if (std::get<0>(res))
					return res;
			}
			return std::make_tuple<bool, DescriptorHeap*>(false, nullptr);
		}

		container::tuple<bool, DescriptorHeap*> D3D12DescriptorHeapManager::TryAllocatePersistentHeap(DescriptorHeapStore* heapStore, UINT count)
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
					container::tuple<UINT, UINT> interval;
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
					auto pHeapEx = new DescriptorHeapEx;
					std::get<1>(res) = pHeapEx;
					CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(heapStore->cpuHandle);
					CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(heapStore->gpuHandle);
					cpuHandle.Offset(left * heapStore->incrementSize);
					gpuHandle.Offset(left * heapStore->incrementSize);
					pHeapEx->cpuHandle = cpuHandle;
					pHeapEx->gpuHandle = gpuHandle;
					pHeapEx->incrementSize = heapStore->incrementSize;
					//pHeapEx->offsetInDescriptors = std::get<0>(interval);
					pHeapEx->interval = interval;
					pHeapEx->pStore = heapStore;
					break;
				}
			}
			return res;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeap* pHeap)
		{
			assert(pHeap != nullptr && "pHeap must be a valid heap pointer!");
			Deallocate(static_cast<DescriptorHeapEx*>(pHeap));
			delete pHeap;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeapEx *pHeapEx)
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
			for (auto i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				EraseTransientAllocation(i, 0);
			}
			for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;++i)
			{
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
			auto pHeapEx = static_cast<DescriptorHeapEx*>(pHeap);
			return pHeapEx->pStore->heap;
		}

		void D3D12DescriptorHeapManager::EraseTransientAllocation(std::size_t frameIndex, std::size_t reservedSize)
		{
			for (auto it = mFrameHeaps.begin(); it != mFrameHeaps.end(); ++it)
			{
				for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
				{
					for (auto j = 0; j < 2; ++j)
					{
						for (auto k = reservedSize;k < it->heaps[frameIndex][i][j].size();++k)
						{
							delete it->heaps[frameIndex][i][j][k];
						}
						for (auto mem : it->allocations[frameIndex][i][j])
						{
							delete mem;
						}
						it->allocations[frameIndex][i][j].clear();
						if(it->heaps[frameIndex][i][j].size() > reservedSize)
							it->heaps[frameIndex][i][j].resize(reservedSize);
						it->next[frameIndex][i][j] = 0;
					}
				}
			}
		}
	}
}