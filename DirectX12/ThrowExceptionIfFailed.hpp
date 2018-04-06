#ifndef __THROWEXCEPTIONIFFAILED_HPP__
#define __THROWEXCEPTIONIFFAILED_HPP__
#include <Windows.h>
#include <string>

inline void ThrowExceptionIfFailed(const HRESULT hr, const std::string MainErrorMessage) {
	if (FAILED(hr)) throw std::runtime_error(MainErrorMessage + "\nErrorCode : " + std::to_string(hr));
}
#endif
