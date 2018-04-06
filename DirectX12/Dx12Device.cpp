#include "Direct3D12Core.hpp"
#include "SafeRelease.hpp"
#include "ThrowExceptionIfFailed.hpp"
#include <array>

static inline std::string GetErrorMessage(const unsigned long ErrorCode) {
	char* lpMessageBuffer = nullptr;
	const DWORD length = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ErrorCode, LANG_USER_DEFAULT, (LPSTR)&lpMessageBuffer, 0, NULL);
	if (length == 0) throw std::runtime_error("エラーメッセージ取得時にエラーが発生しました。\nエラーコード : " + std::to_string(GetLastError()));
	DWORD i = length - 3;
	for (; '\r' != lpMessageBuffer[i] && '\n' != lpMessageBuffer[i] && '\0' != lpMessageBuffer[i]; i++);//改行文字削除
	lpMessageBuffer[i] = '\0';
	std::string s = "エラーコード : " + std::to_string(ErrorCode) + "　" + lpMessageBuffer;
	LocalFree(lpMessageBuffer);
	return s;
}

static std::string WStringToString(const std::wstring oWString) {
	const int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, (char *)NULL, 0, NULL, NULL);
	if (0 == iBufferSize) {
		const DWORD ErrorCode = GetLastError();
		throw std::runtime_error(GetErrorMessage(ErrorCode));
	}
	char* cpMultiByte = new char[iBufferSize];
	WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL);
	std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
	delete[] cpMultiByte;
	return oRet;
}

#ifdef DIRECT3D12_DLL_CREATE
static inline std:shared_ptr<std::basic_string<TCHAR>> ToTCHAR(const wchar_t* str) {
#ifdef UNICODE
	return std::make_shared<std::wstring>(std::wstring(str));
#else
	return std::make_shared<std::string>(WStringToString(str));
#endif
}
#else
static inline std::basic_string<TCHAR> ToTCHAR(const wchar_t* str) {
#ifdef UNICODE
	return std::wstring(str);
#else
	return WStringToString(str);
#endif
}
#endif

namespace Direct3D12 {
	namespace {
		const std::array<D3D_FEATURE_LEVEL, 4> LevelBuf = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_12_1
		};
		D3D_FEATURE_LEVEL ToFeatureLevelImpl(const unsigned int Level) {
			if (Level < 1 || Level > 4) throw std::out_of_range("invalid feature level");
			return LevelBuf[Level - 1];
		}
		D3D_FEATURE_LEVEL ToFeatureLevel(const D3DFeatureLevel Level) {
			return ToFeatureLevelImpl(static_cast<unsigned int>(Level));
		}
	}

	Device::Device(const Factory DxgiFactory, const CreateDeviceConfig Conf) {
		if (Conf.UseDeviceID == CreateDeviceConfig::WarpDevice) this->CreateDeviceUseWarpDevice(DxgiFactory);
		else if (Conf.FeatureLevel == D3DFeatureLevel::Unknown) 
			Conf.UseDeviceID == CreateDeviceConfig::DeviceSearchMode
				? this->CreateDeviceUseHardwareAdapter(DxgiFactory) 
				: this->CreateDeviceUseHardwareAdapter(DxgiFactory, Conf.UseDeviceID);
		else 
			Conf.UseDeviceID == CreateDeviceConfig::DeviceSearchMode
				? this->CreateDeviceUseHardwareAdapter(DxgiFactory, Conf.FeatureLevel)
				: ThrowExceptionIfFailed(this->CreateDeviceUseHardwareAdapter(DxgiFactory, Conf.FeatureLevel, Conf.UseDeviceID), "Failed to create device.");
	}

	Device::~Device() {
		SAFERELEASE(this->Dx12Device);
	}

	std::vector<GPUData> Device::GetGPUList(const Factory factory, const D3DFeatureLevel FeatureLevel) {
		IDXGIAdapter1* HardwareAdapter;
		std::vector<GPUData> Arr;
		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &HardwareAdapter); i++) {
			DXGI_ADAPTER_DESC1 desc;
			HardwareAdapter->GetDesc1(&desc);
			if (SUCCEEDED(CreateDeviceUseHardwareAdapterTest(HardwareAdapter, FeatureLevel))) Arr.emplace_back(i, ToTCHAR(desc.Description));
		}
		SAFERELEASE(HardwareAdapter);
		return Arr;
	}

	HRESULT Device::CreateDeviceUseWarpDevice(IDXGIAdapter* Adapter, const D3DFeatureLevel FeatureLevel) {
		return D3D12CreateDevice(Adapter, ToFeatureLevel(FeatureLevel), IID_PPV_ARGS(&this->Dx12Device));
	}

	HRESULT Device::CreateDeviceUseHardwareAdapter(IDXGIAdapter1* Adapter, const D3DFeatureLevel FeatureLevel) {
		return D3D12CreateDevice(Adapter, ToFeatureLevel(FeatureLevel), IID_PPV_ARGS(&this->Dx12Device));
	}

	HRESULT Device::CreateDeviceUseHardwareAdapterTest(IDXGIAdapter1* Adapter, const D3DFeatureLevel FeatureLevel) {
		return D3D12CreateDevice(Adapter, ToFeatureLevel(FeatureLevel), __uuidof(ID3D12Device), nullptr);
	}

	// Microsoft Basic Render Driverを使用してのデバイス作成
	void Device::CreateDeviceUseWarpDevice(const Factory factory) {
		IDXGIAdapter* WarpAdapter;
		ThrowExceptionIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapter)), "Error is occured in EnumWarpAdapter.");
		ThrowExceptionIfFailed(this->CreateDeviceUseWarpDevice(WarpAdapter, D3DFeatureLevel::Level11_0), "Error is occured in CreateDevice in CreateDeviceUseWarpDevice.");
	}

	// 全ての情報が分かっており、作成するだけの関数
	HRESULT Device::CreateDeviceUseHardwareAdapter(const Factory factory, const D3DFeatureLevel FeatureLevel, const unsigned int DeviceID) {
		IDXGIAdapter1* Adapter;
		if (DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(DeviceID, &Adapter))
			throw std::runtime_error("DeviceID : " + std::to_string(DeviceID) + " : not found.");
		const HRESULT TestResult = CreateDeviceUseHardwareAdapterTest(Adapter, FeatureLevel);
		if (FAILED(TestResult)) return TestResult;
		return this->CreateDeviceUseHardwareAdapter(Adapter, FeatureLevel);
	}

	// デバイスが指定されていない場合の関数
	void Device::CreateDeviceUseHardwareAdapter(const Factory factory, const D3DFeatureLevel FeatureLevel) {
		const std::vector<GPUData> Arr = GetGPUList(factory, FeatureLevel);
		if (Arr.empty()) throw std::runtime_error("Reserved device can't create Direct3D 12 device using reserved feature level.");
		ThrowExceptionIfFailed(this->CreateDeviceUseHardwareAdapter(factory, FeatureLevel, Arr[0].DeviceID), "Failed to create device.");
	}

	// 機能レベルが指定されていない場合の関数
	void Device::CreateDeviceUseHardwareAdapter(const Factory factory, const unsigned int DeviceID) {
		static const std::array<D3DFeatureLevel, 4> Buf = {
			D3DFeatureLevel::Level12_1, D3DFeatureLevel::Level12_0, D3DFeatureLevel::Level11_1, D3DFeatureLevel::Level11_0
		};
		for (auto& i : Buf) if (SUCCEEDED(this->CreateDeviceUseHardwareAdapter(factory, i, DeviceID))) return;
		throw std::runtime_error("Reserved device can't create Direct3D 12 device using hardware adapter rendering mode.");
	}

	// 機能レベルもデバイスも指定されていない場合の関数
	void Device::CreateDeviceUseHardwareAdapter(const Factory factory) {
		static const std::array<D3DFeatureLevel, 4> Buf = {
			D3DFeatureLevel::Level12_1, D3DFeatureLevel::Level12_0, D3DFeatureLevel::Level11_1, D3DFeatureLevel::Level11_0
		};
		std::vector<GPUData> Arr;
		D3DFeatureLevel UseFeatureLevel = D3DFeatureLevel::Unknown;
		for (auto& i : Buf) {
			Arr = GetGPUList(factory, i);
			if (!Arr.empty()) {
				UseFeatureLevel = i;
				break;
			}
		}
		if (Arr.empty()) throw std::runtime_error("This computer can't create Direct3D 12 device using hardware adapter rendering mode.");
		ThrowExceptionIfFailed(this->CreateDeviceUseHardwareAdapter(factory, UseFeatureLevel, Arr[0].DeviceID), "Failed to create device.");
	}
}
