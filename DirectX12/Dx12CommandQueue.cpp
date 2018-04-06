#include "Direct3D12Core.hpp"
#include "ThrowExceptionIfFailed.hpp"
#include "SafeRelease.hpp"

namespace Direct3D12 {
	CommandQueue::CommandQueue(const Device Dx12Device) {
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CommandQueue(desc, Dx12Device);
	}

	CommandQueue::CommandQueue(const D3D12_COMMAND_QUEUE_DESC QueueDesc, const Device Dx12Device) {
		ThrowExceptionIfFailed((*Dx12Device)->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&this->Dx12CommandQueue)), "Failed to create command queue.");
	}

	CommandQueue::~CommandQueue() {
		SAFERELEASE(this->Dx12CommandQueue);
	}
}
