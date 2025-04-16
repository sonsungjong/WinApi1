#include "pch.h"
#include "LZR920_Func.h"

std::string getTime()
{
	time_t now = time(nullptr);
	struct tm t;
	localtime_s(&t, &now);

	char buffer[20]{ 0 };  // "YYYY-MM-DD HH:mm:SS\0" (총 19글자 + null terminator)
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &t);  // 시간까지 포함
	return std::string(buffer);
}

std::string getTimeMS()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm local_tm;
	// gmtime_s -> UTC 시간
	// localtime_s -> 로컬 시간
	localtime_s(&local_tm, &in_time_t);

	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;

	char buffer[100]{};
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);

	char result[128]{};
	sprintf_s(result, sizeof(result), "%s.%03d", buffer, static_cast<int>(millis.count()));

	return std::string(result);
}

std::wstring getTime_wstr()
{
	time_t now = time(nullptr);
	struct tm t;
	localtime_s(&t, &now);

	wchar_t buffer[20]{ 0 };  // "YYYY-MM-DD HH:mm:SS\0" (총 19글자 + null terminator)
	wcsftime(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%Y-%m-%d %H:%M:%S", &t);  // 시간까지 포함
	return std::wstring(buffer);
}

std::string getTimeMS_FileName()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm local_tm;
	// gmtime_s -> UTC 시간
	// localtime_s -> 로컬 시간
	localtime_s(&local_tm, &in_time_t);

	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;

	char buffer[100]{};
	strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", &local_tm);

	char result[128]{};
	sprintf_s(result, sizeof(result), "%s_%03d", buffer, static_cast<int>(millis.count()));

	return std::string(result);
}

std::string convertUTF16toUTF8(const std::wstring& utf16)
{
	std::string strUTF8 = "";

	// 빈문자가 아닐때
	if (!utf16.empty()) {
		// UTF-8 변환에 필요한 버퍼 크기 계산
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
		if (bufferSize > 0) {
			strUTF8.assign(static_cast<size_t>(bufferSize) - 1, '\0');
			WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &strUTF8[0], bufferSize, NULL, NULL);
		}
	}

	return strUTF8;
}

std::string convertUTF16toASCII(const std::wstring& utf16)
{
	std::string strASCII = "";

	if (!utf16.empty()) {
		// ASCII 변환에 필요한 버퍼 크기 계산
		int bufferSize = WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
		if (bufferSize > 0) {
			strASCII.assign(static_cast<size_t>(bufferSize) - 1, '\0');  // 널 문자를 제외한 크기로 할당
			WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), -1, &strASCII[0], bufferSize, NULL, NULL);
		}
	}

	return strASCII;
}

std::wstring convertUTF8toUTF16(const std::string& utf8)
{
	std::wstring strUTF16 = L"";

	if (!utf8.empty()) {
		int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
		if (bufferSize > 0) {
			strUTF16.assign(static_cast<size_t>(bufferSize) - 1, L'\0');
			MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &strUTF16[0], bufferSize);
		}
	}

	return strUTF16;
}

std::string convertUTF8toASCII(const std::string& utf8)
{
	std::string strASCII = "";

	if (!utf8.empty()) {
		int bufferSize = WideCharToMultiByte(CP_ACP, 0, convertUTF8toUTF16(utf8).c_str(), -1, NULL, 0, NULL, NULL);
		if (bufferSize > 0) {
			strASCII.assign(static_cast<size_t>(bufferSize) - 1, '\0');
			WideCharToMultiByte(CP_ACP, 0, convertUTF8toUTF16(utf8).c_str(), -1, &strASCII[0], bufferSize, NULL, NULL);
		}
	}

	return strASCII;
}

std::string convertASCIItoUTF8(const std::string& ascii)
{
	std::string strUTF8 = "";

	if (!ascii.empty()) {
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, convertASCIItoUTF16(ascii).c_str(), -1, NULL, 0, NULL, NULL);
		if (bufferSize > 0) {
			strUTF8.assign(static_cast<size_t>(bufferSize) - 1, '\0');
			WideCharToMultiByte(CP_UTF8, 0, convertASCIItoUTF16(ascii).c_str(), -1, &strUTF8[0], bufferSize, NULL, NULL);
		}
	}

	return strUTF8;
}

std::wstring convertASCIItoUTF16(const std::string& ascii)
{
	std::wstring strUTF16 = L"";

	if (!ascii.empty()) {
		int bufferSize = MultiByteToWideChar(CP_ACP, 0, ascii.c_str(), -1, NULL, 0);
		if (bufferSize > 0) {
			strUTF16.assign(static_cast<size_t>(bufferSize) - 1, L'\0');
			MultiByteToWideChar(CP_ACP, 0, ascii.c_str(), -1, &strUTF16[0], bufferSize);
		}
	}

	return strUTF16;
}

std::string convertUTF16toCP949(const std::wstring& utf16)
{
	std::string cp949;
	if (!utf16.empty())
	{
		// CP949는 코드페이지 949입니다.
		int bufferSize = WideCharToMultiByte(949, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
		if (bufferSize > 0)
		{
			cp949.resize(bufferSize - 1);
			WideCharToMultiByte(949, 0, utf16.c_str(), -1, &cp949[0], bufferSize, NULL, NULL);
		}
	}
	return cp949;
}
