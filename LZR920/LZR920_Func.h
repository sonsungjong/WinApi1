#pragma once



std::string getTime();
std::string getTimeMS();
std::wstring getTime_wstr();
std::string getTimeMS_FileName();
std::string convertUTF16toUTF8(const std::wstring& utf16);
std::string convertUTF16toASCII(const std::wstring& utf16);
std::wstring convertUTF8toUTF16(const std::string& utf8);
std::string convertUTF8toASCII(const std::string& utf8);
std::string convertASCIItoUTF8(const std::string& ascii);
std::wstring convertASCIItoUTF16(const std::string& ascii);
std::string convertUTF16toCP949(const std::wstring& utf16);