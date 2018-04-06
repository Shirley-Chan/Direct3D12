#include "Direct3D12Core.hpp"
#include "SafeRelease.hpp"
#include "ThrowExceptionIfFailed.hpp"

namespace Direct3D12 {
	RenderTargetView::RenderTargetView(const Device Dx12Device, const SwapChain DxgiSwapChain, const RenderTargetViewDescriptorHeap Dx12RTVDescHeap, const unsigned int BufferCount) {
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MipSlice = 0;
		Desc.Texture2D.PlaneSlice = 0;
		RenderTargetView(Dx12Device, DxgiSwapChain, Dx12RTVDescHeap, Desc, BufferCount);
	}

	RenderTargetView::RenderTargetView(const Device Dx12Device, const SwapChain DxgiSwapChain, const RenderTargetViewDescriptorHeap Dx12RTVDescHeap, const D3D12_RENDER_TARGET_VIEW_DESC RTVDesc, const unsigned int BufferCount) {
		this->BufferCount = BufferCount;
		CD3DX12_CPU_DESCRIPTOR_HANDLE DescHandle(Dx12RTVDescHeap->GetCPUDescriptorHandleForHeapStart());
		this->Dx12RenderTargetView = new ID3D12Resource*[BufferCount];
		const unsigned int DescriptorSize = Dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (unsigned int i = 0; i < BufferCount; i++) {
			ThrowExceptionIfFailed(DxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&this->Dx12RenderTargetView[i])), "Failed to get buffer.");
			Dx12Device->CreateRenderTargetView(this->Dx12RenderTargetView[i], &RTVDesc, DescHandle);
			DescHandle.Offset(i, DescriptorSize);
		}
	}

	RenderTargetView::~RenderTargetView() {
		for (unsigned int i = 0; i < BufferCount; i++) SAFERELEASE(this->Dx12RenderTargetView[i]);
	}

	ID3D12Resource* RenderTargetView::Get(const unsigned int Count) {
		if (Count > this->BufferCount) throw std::out_of_range("The specified number exceeds the buffer count.");
		return this->Dx12RenderTargetView[Count];
	}

	ID3D12Resource* RenderTargetView::Get(const unsigned int Count) const {
		if (Count > this->BufferCount) throw std::out_of_range("The specified number exceeds the buffer count.");
		return this->Dx12RenderTargetView[Count];
	}
}
