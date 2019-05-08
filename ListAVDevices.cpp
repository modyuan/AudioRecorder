
#include "ListAVDevices.h"

#ifdef WINDOWS

static std::string unicode2utf8(const WCHAR* uni) {
	static char temp[500];// max length of friendly name by UTF-16 is 128, so 500 in enough by utf-8
	memset(temp, 0, 500);
	WideCharToMultiByte(CP_UTF8, 0, uni, -1, temp, 500, NULL, NULL);
	return std::string(temp);
}


string GbkToUtf8(const char *src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

string Utf8ToGbk(const char *src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

HRESULT DS_GetAudioVideoInputDevices(std::vector<std::string> &vectorDevices, const std::string deviceType)
{
	GUID guidValue;
	if (deviceType == "v") {
		guidValue = CLSID_VideoInputDeviceCategory;
	}
	else if (deviceType == "a") {
		guidValue = CLSID_AudioInputDeviceCategory;
	}
	else {
		throw std::invalid_argument("param deviceType must be 'a' or 'v'.");
	}

	WCHAR FriendlyName[MAX_FRIENDLY_NAME_LENGTH];
	HRESULT hr;

	// 初始化
	vectorDevices.clear();

	// 初始化COM
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		return hr;
	}

	// 创建系统设备枚举器实例
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		CoUninitialize();
		return hr;
	}

	// 获取设备类枚举器
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(guidValue, &pEnumCat, 0);
	if (hr == S_OK)
	{
		// 枚举设备名称
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// 获取设备友好名
				VARIANT varName;
				VariantInit(&varName);

				hr = pPropBag->Read(L"FriendlyName", &varName, NULL);
				if (SUCCEEDED(hr))
				{
					StringCchCopy(FriendlyName, MAX_FRIENDLY_NAME_LENGTH, varName.bstrVal);
					vectorDevices.push_back(unicode2utf8(FriendlyName));
				}

				VariantClear(&varName);
				pPropBag->Release();
			}

			pMoniker->Release();
		} // End for While

		pEnumCat->Release();
	}

	pSysDevEnum->Release();
	CoUninitialize();

	return hr;
}

std::string DS_GetDefaultDevice(std::string type) {
	if (type == "a") {
		std::vector<string> v;
		int ret = DS_GetAudioVideoInputDevices(v, "a");
		if (ret >= 0 && !v.empty()) {
			return v[0];
		}
		else {
			return "";
		}
	}
	else if (type == "v") {
		std::vector<string> v;
		int ret = DS_GetAudioVideoInputDevices(v, "v");
		if (ret >= 0 && !v.empty()) {
			return v[0];
		}
		else {
			return "";
		}
	}
	else {
		throw std::invalid_argument("param type must be 'a' or 'v'.");
	}
}

#endif