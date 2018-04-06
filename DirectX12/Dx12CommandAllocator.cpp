#include "Direct3D12Core.hpp"
#include "SafeRelease.hpp"
#include "ThrowExceptionIfFailed.hpp"

namespace Direct3D12 {
	CommandAllocator::CommandAllocator(const Device Dx12Device) {
		ThrowExceptionIfFailed(Dx12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->Dx12CommandAllocator)), "Failed to create command allocator.");
	}

	CommandAllocator::~CommandAllocator() {
		SAFERELEASE(this->Dx12CommandAllocator);
	}
}
