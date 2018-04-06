#include "Direct3D12Core.hpp"
#include "ThrowExceptionIfFailed.hpp"
#include "SafeRelease.hpp"

namespace Direct3D12 {
	SwapChain::SwapChain(const Factory DxgiFactory, const CommandQueue Dx12CommandQueue, const HWND WindowHandle, const UINT WindowWidth, const UINT WindowHeight) {
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.BufferCount = DefaultBufferCount;
		desc.Width = WindowWidth;
		desc.Height = WindowHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Flags = 0;
		SwapChain(DxgiFactory, Dx12CommandQueue, WindowHandle, desc);
	}

	SwapChain::SwapChain(const Factory DxgiFactory, const CommandQueue Dx12CommandQueue, const HWND WindowHandle, const DXGI_SWAP_CHAIN_DESC1 SwapChainDesc) {
		IDXGISwapChain1* SwapChain1;
		ThrowExceptionIfFailed(DxgiFactory->CreateSwapChainForHwnd(*Dx12CommandQueue, WindowHandle, &SwapChainDesc, nullptr, nullptr, &SwapChain1), "Failed to create swap chain.");
		ThrowExceptionIfFailed(DxgiFactory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER), "Failed to make window association.");
		ThrowExceptionIfFailed(SwapChain1->QueryInterface(__uuidof(IDXGISwapChain3), reinterpret_cast<void**>(SwapChain1)), "Failed to convert SwapChain1 to SwapChain3.");
		SAFERELEASE(SwapChain1);
	}

	SwapChain::~SwapChain() {
		SAFERELEASE(this->DxgiSwapChain);
	}
}
