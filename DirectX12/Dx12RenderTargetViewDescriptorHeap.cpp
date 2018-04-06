#include "Direct3D12Core.hpp"
#include "SafeRelease.hpp"
#include "ThrowExceptionIfFailed.hpp"

namespace Direct3D12 {
	RenderTargetViewDescriptorHeap::RenderTargetViewDescriptorHeap(const Device Dx12Device, const unsigned int BufferCount) {
		D3D12_DESCRIPTOR_HEAP_DESC Desc{};
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.NumDescriptors = BufferCount;
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		RenderTargetViewDescriptorHeap(Dx12Device, Desc, BufferCount);
	}

	RenderTargetViewDescriptorHeap::RenderTargetViewDescriptorHeap(const Device Dx12Device, const D3D12_DESCRIPTOR_HEAP_DESC Dx12DescHeapDesc, const unsigned int BufferCount) {
		ThrowExceptionIfFailed(Dx12Device->CreateDescriptorHeap(&Dx12DescHeapDesc, IID_PPV_ARGS(&this->Dx12RTVDescriptorHeap)), "Failed to create RTV descriptor heap.");
	}

	RenderTargetViewDescriptorHeap::~RenderTargetViewDescriptorHeap() {
		SAFERELEASE(this->Dx12RTVDescriptorHeap);
	}
}

