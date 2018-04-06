#ifndef __DIRECT3D12CORE_HPP__
#define __DIRECT3D12CORE_HPP__
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <string>
#include <vector>
#include <limits>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#ifdef DIRECT3D12_DLL_CREATE
#include <memory>
#endif
constexpr unsigned int DefaultBufferCount = 2;

namespace Direct3D12 {
	enum class D3DFeatureLevel : unsigned int {
		Level12_1 = 4,
		Level12_0 = 3,
		Level11_1 = 2,
		Level11_0 = 1,
		Unknown = 0
	};

	class CreateDeviceConfig {
	private:
		CreateDeviceConfig(const D3DFeatureLevel FeatureLevel, const unsigned int DeviceID)
			: FeatureLevel(FeatureLevel), UseDeviceID(DeviceID) {}
	public:
		CreateDeviceConfig() = default;
		static CreateDeviceConfig UseWarpDeviceMode() { return CreateDeviceConfig(D3DFeatureLevel::Level11_0, WarpDevice); }
		static CreateDeviceConfig UseHardwareAdapterMode(const D3DFeatureLevel FeatureLevel = D3DFeatureLevel::Unknown, const unsigned int DeviceID = DeviceSearchMode) {
			return CreateDeviceConfig(FeatureLevel, DeviceID);
		}
		D3DFeatureLevel FeatureLevel;
		unsigned int UseDeviceID;
		static constexpr unsigned int WarpDevice = std::numeric_limits<unsigned int>::max();
		static constexpr unsigned int DeviceSearchMode = WarpDevice - 1;
	};

	class Factory {
	private:
		void EnableDebugLayer(unsigned int &Flag);
	protected:
		IDXGIFactory4* DxgiFactory;
	public:
		Factory();
		~Factory();
		IDXGIFactory4* operator -> () { return this->DxgiFactory; }
		IDXGIFactory4* operator -> () const { return this->DxgiFactory; }
		IDXGIFactory4* operator * () { return this->DxgiFactory; }
		IDXGIFactory4* operator * () const { return this->DxgiFactory; }
	};

	struct GPUData {
		GPUData() = default;
		GPUData(const unsigned int ID, const std::basic_string<TCHAR> GPUName)
			: DeviceID(ID), GPUName(GPUName) {}
		unsigned int DeviceID;
#ifdef DIRECT3D12_DLL_CREATE
		std::shared_ptr<std::basic_string<TCHAR>> GPUName;
#else
		std::basic_string<TCHAR> GPUName;
#endif
	};

	class Device {
	private:
		static HRESULT CreateDeviceUseHardwareAdapterTest(IDXGIAdapter1* Adapter, const D3DFeatureLevel FeatureLevel);
		HRESULT CreateDeviceUseWarpDevice(IDXGIAdapter* Adapter, const D3DFeatureLevel FeatureLevel);
		HRESULT CreateDeviceUseHardwareAdapter(IDXGIAdapter1* Adapter, const D3DFeatureLevel FeatureLevel);
		void CreateDeviceUseWarpDevice(const Factory factory);
		HRESULT CreateDeviceUseHardwareAdapter(const Factory factory, const D3DFeatureLevel FeatureLevel, const unsigned int DeviceID);
		void CreateDeviceUseHardwareAdapter(const Factory factory, const D3DFeatureLevel FeatureLevel);
		void CreateDeviceUseHardwareAdapter(const Factory factory, const unsigned int DeviceID);
		void CreateDeviceUseHardwareAdapter(const Factory factory);
	protected:
		ID3D12Device* Dx12Device;
	public:
		Device() = default;
		Device(const Factory DxgiFactory, const CreateDeviceConfig Conf);
		~Device();
		ID3D12Device* operator -> () { return this->Dx12Device; }
		ID3D12Device* operator -> () const { return this->Dx12Device; }
		ID3D12Device* operator * () { return this->Dx12Device; }
		ID3D12Device* operator * () const { return this->Dx12Device; }
		static std::vector<GPUData> GetGPUList(const Factory factory, const D3DFeatureLevel FeatureLevel);
	};

	class CommandQueue {
	protected:
		ID3D12CommandQueue* Dx12CommandQueue;
	public:
		CommandQueue() = default;
		CommandQueue(const Device Dx12Device);
		CommandQueue(const D3D12_COMMAND_QUEUE_DESC QueueDesc, const Device Dx12Device);
		~CommandQueue();
		ID3D12CommandQueue* operator -> () { return this->Dx12CommandQueue; }
		ID3D12CommandQueue* operator -> () const { return this->Dx12CommandQueue; }
		ID3D12CommandQueue* operator * () { return this->Dx12CommandQueue; }
		ID3D12CommandQueue* operator * () const { return this->Dx12CommandQueue; }
	};

	class SwapChain {
	protected:
		IDXGISwapChain3* DxgiSwapChain;
	public:
		SwapChain() = default;
		SwapChain(const Factory DxgiFactory, const CommandQueue Dx12CommandQueue, const HWND WindowHandle, const UINT WindowWidth, const UINT WindowHeight);
		SwapChain(const Factory DxgiFactory, const CommandQueue Dx12CommandQueue, const HWND WindowHandle, const DXGI_SWAP_CHAIN_DESC1 SwapChainDesc);
		~SwapChain();
		IDXGISwapChain3* operator -> () { return this->DxgiSwapChain; }
		IDXGISwapChain3* operator -> () const { return this->DxgiSwapChain; }
		IDXGISwapChain3* operator * () { return this->DxgiSwapChain; }
		IDXGISwapChain3* operator * () const { return this->DxgiSwapChain; }
	};

	class RenderTargetViewDescriptorHeap {
	protected:
		ID3D12DescriptorHeap* Dx12RTVDescriptorHeap;
	public:
		RenderTargetViewDescriptorHeap() = default;
		RenderTargetViewDescriptorHeap(const Device Dx12Device, const unsigned int BufferCount = DefaultBufferCount);
		RenderTargetViewDescriptorHeap(const Device Dx12Device, const D3D12_DESCRIPTOR_HEAP_DESC Dx12DescHeapDesc, const unsigned int BufferCount = DefaultBufferCount);
		~RenderTargetViewDescriptorHeap();
		ID3D12DescriptorHeap* operator -> () { return this->Dx12RTVDescriptorHeap; }
		ID3D12DescriptorHeap* operator -> () const { return this->Dx12RTVDescriptorHeap; }
		ID3D12DescriptorHeap* operator * () { return this->Dx12RTVDescriptorHeap; }
		ID3D12DescriptorHeap* operator * () const { return this->Dx12RTVDescriptorHeap; }
	};

	class RenderTargetView {
	private:
		unsigned int BufferCount;
	protected:
		ID3D12Resource** Dx12RenderTargetView;
	public:
		RenderTargetView() = default;
		RenderTargetView(const Device Dx12Device, const SwapChain DxgiSwapChain, const RenderTargetViewDescriptorHeap Dx12RTVDescHeap, const unsigned int BufferCount = DefaultBufferCount);
		RenderTargetView(const Device Dx12Device, const SwapChain DxgiSwapChain, const RenderTargetViewDescriptorHeap Dx12RTVDescHeap, const D3D12_RENDER_TARGET_VIEW_DESC Dx12RTVDesc, const unsigned int BufferCount = DefaultBufferCount);
		~RenderTargetView();
		ID3D12Resource* Get(const unsigned int Count) const;
		ID3D12Resource* Get(const unsigned int Count);
		ID3D12Resource** operator -> () { return this->Dx12RenderTargetView; }
		ID3D12Resource** operator -> () const { return this->Dx12RenderTargetView; }
		ID3D12Resource** operator * () { return this->Dx12RenderTargetView; }
		ID3D12Resource** operator * () const { return this->Dx12RenderTargetView; }
		ID3D12Resource* operator [] (const unsigned int Count) { return this->Dx12RenderTargetView[Count]; }
		ID3D12Resource* operator [] (const unsigned int Count) const { return this->Dx12RenderTargetView[Count]; }
	};

	template<unsigned int BufCount = DefaultBufferCount> class RenderTargetView2 : public RenderTargetView {
	public:
		RenderTargetView2() = default;
		RenderTargetView2(const Device Dx12Device, const SwapChain DxgiSwapChain, const RenderTargetViewDescriptorHeap Dx12RTVDescHeap)
			: RenderTargetView(Dx12Device, DxgiSwapChain, Dx12RTVDescHeap, BufCount) {}
		RenderTargetView2(const Device Dx12Device, const SwapChain DxgiSwapChain, const RenderTargetViewDescriptorHeap Dx12RTVDescHeap, const D3D12_RENDER_TARGET_VIEW_DESC Dx12RTVDesc)
			: RenderTargetView(Dx12Device, DxgiSwapChain, Dx12RTVDescHeap, Dx12RTVDesc, BufCount) {}
		~RenderTargetView2() { RenderTargetView::~RenderTargetView(); }
	};

	/*
		開発うろ覚えメモ
		RenderTargetViewが引数の関数に対してRenderTargetView2は渡せるけど
		RenderTargetView2が引数の関数にRenderTargetViewは渡せない
	*/

	class CommandAllocator {
	protected:
		ID3D12CommandAllocator* Dx12CommandAllocator;
	public:
		CommandAllocator() = default;
		CommandAllocator(const Device Dx12Device);
		~CommandAllocator();
		ID3D12CommandAllocator* operator -> () { return this->Dx12CommandAllocator; }
		ID3D12CommandAllocator* operator -> () const { return this->Dx12CommandAllocator; }
		ID3D12CommandAllocator* operator * () { return this->Dx12CommandAllocator; }
		ID3D12CommandAllocator* operator * () const { return this->Dx12CommandAllocator; }
	};

	class PipelineState {
	protected:
		ID3D12PipelineState* Dx12PipelineState;
	public:

	};

	class CommandList {
	protected:
		ID3D12CommandList* Dx12CommandList;
	public:
		CommandList() = default;
		CommandList(const Device Dx12Device, const CommandAllocator Dx12CommandAllocator);
		CommandList(const Device Dx12Device, const CommandAllocator Dx12CommandAllocator, const PipelineState Dx12PipelineState);
	};
 }
#endif
