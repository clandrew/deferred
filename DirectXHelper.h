#pragma once

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw std::exception();
		}
	}

	inline std::vector<byte> ReadData(const std::wstring& filename)
	{
		std::vector<byte> result;

		FILE* file;
		_wfopen_s(&file, filename.c_str(), L"rb");

		fseek(file, 0, SEEK_END);
		long length = ftell(file);

		result.resize(length);

		fseek(file, 0, SEEK_SET);
		fread_s(result.data(), result.size(), 1, result.size(), file);

		return result;
	}

	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}

	// Assign a name to the object to aid with debugging.
#if defined(_DEBUG)
	inline void SetName(ID3D12Object* pObject, LPCWSTR name)
	{
		pObject->SetName(name);
	}
#else
	inline void SetName(ID3D12Object*, LPCWSTR)
	{
	}
#endif
}

// Naming helper function for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
#define NAME_D3D12_OBJECT(x) DX::SetName(x.Get(), L#x)
