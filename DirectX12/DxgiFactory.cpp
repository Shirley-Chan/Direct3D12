#include "SafeRelease.hpp"
#include "Direct3D12Core.hpp"
#include "ThrowExceptionIfFailed.hpp"
#include <array>
#include <stdexcept>

namespace Direct3D12 {
	Factory::Factory() {
		unsigned int Flag{};
#ifdef _DEBUG
		this->EnableDebugLayer(Flag);
#endif
		ThrowExceptionIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&this->DxgiFactory)), "Failed to create factory.");
	}

	Factory::~Factory() { SAFERELEASE(this->DxgiFactory); }

	void Factory::EnableDebugLayer(unsigned int& Flag) {
		ID3D12Debug* Debug;
		ThrowExceptionIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&Debug)), "Failed to get debug interface.");
		Debug->EnableDebugLayer();
		SAFERELEASE(Debug);
		Flag |= DXGI_CREATE_FACTORY_DEBUG;
	}
}
