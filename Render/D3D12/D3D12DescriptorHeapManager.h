#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <tuple>
#include <functional>
#include <wrl\client.h>
#include <atomic>
#include "Singleton.h"
#include "D3D12Device.h"
#include "ThreadLocalObject.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		struct DescriptorHeap
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
			UINT incrementSize;
			//offset from heap start.Measured in number of descriptors offset,not byte
			//UINT offsetInDescriptors;
		};

		class D3D12DescriptorHeapManager : public Foundation::Singleton<D3D12DescriptorHeapManager>
		{
		public:
			D3D12DescriptorHeapManager();
			~D3D12DescriptorHeapManager();
			DescriptorHeap* Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool transient);
			ComPtr<ID3D12DescriptorHeap> GetHeap(DescriptorHeap* pHeap)const;
			void Deallocate(DescriptorHeap* pHeap);
			//Thread safe
			static UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);
			//Thread unsafe
			void Clear();
		private:
			static constexpr int HEAP_DESCRIPTOR_ALLOC_SIZE = 100;
			struct DescriptorHeapStore : DescriptorHeap
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc;
				ComPtr<ID3D12DescriptorHeap> heap;
				std::list<std::tuple<UINT, UINT>> freeIntervals;
				std::size_t freeDescriptors;
			};
			struct DescriptorHeapAllocation : DescriptorHeap
			{
				DescriptorHeapStore *pStore;
				std::tuple<UINT, UINT> interval;
			};

			//represents heaps allocated in one frame
			struct TransientHeap
			{
				UINT allocCount;
				UINT descriptorCount;
				DescriptorHeapStore* heapStore;
				DescriptorHeapAllocation* handles;
				std::uint64_t frameCount;
			};
			using TransientHeapList = std::vector<TransientHeap>;
			TransientHeap ReserveTransientDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, std::uint64_t frameCount);
			DescriptorHeap* AllocatePersistentHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			DescriptorHeap* AllocateTransientHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			DescriptorHeap* TryAllocatePersistentHeap(DescriptorHeapStore* heapInfo, UINT count);
			DescriptorHeap* TryAllocatePersistentHeap(std::vector<DescriptorHeapStore*>& heapList, UINT count);
			void Deallocate(DescriptorHeapAllocation* handle);
			static DescriptorHeapStore* CreateHeapStore(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount);

			//persistent heaps are heaps persist longer than one frame.Examples are RTV and DSV heaps
			std::vector<DescriptorHeapStore*> mPersistentHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];
			//transient heaps are heaps that only needs validity in one frame.After a frame finishes,the content of heaps doesn't matter
			Foundation::ThreadLocalObject<TransientHeapList> mTransientHeaps[RENDER_FRAME_COUNT][D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];

			static UINT sIncrementSizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
			static constexpr UINT TransientHeapAllocationUnit = 100;
		};
	}
}