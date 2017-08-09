#pragma once
#include <iostream>
namespace HL
{
	namespace System
	{
		class Console
		{
			static Threading::Mutex& ConsoleMutex() {
				static Threading::Mutex console_enter;
				return console_enter;
			}
		public:
			static void Write(const wchar_t*Message) {
				ConsoleMutex().Lock();
				wprintf_s(Message);
				ConsoleMutex().UnLock();
			}
			template<class...Args>
			static void Write(const wchar_t*Format, Args const...args) {
				String temp = String::Format(Format, args...);
				Write(temp);
			}
			static void Write(String const&Message) {
				ConsoleMutex().Lock();
				std::wcout << Message;
				ConsoleMutex().UnLock();
			}
			template<class...Args>
			static void WriteLine(const wchar_t*Format, Args const...args) {
				String temp = String::Format(Format, args...);
				WriteLine(temp);
			}
			static void WriteLine(const wchar_t*Message) {
				ConsoleMutex().Lock();
				std::wcout << Message << std::endl;
				ConsoleMutex().UnLock();
			}

			static void WriteLine(String const&Message) {
				ConsoleMutex().Lock();
				std::wcout << Message << std::endl;
				ConsoleMutex().UnLock();
			}
			static void WriteLine(const char*Message) {
				ConsoleMutex().Lock();
				std::cout << Message << std::endl;
				ConsoleMutex().UnLock();
			}
			//数值类型
			static void WriteLine(int number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%d \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(unsigned number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%u \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%ld \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(unsigned long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%u \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(long long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%lld \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(unsigned long long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%llu \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(float number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%f \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(double number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%lf \n", number);
				ConsoleMutex().UnLock();
			}
			static void WriteLine(short number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%hd \n", number);
				ConsoleMutex().UnLock();
			}

			static void Write(int number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%d", number);
				ConsoleMutex().UnLock();
			}
			static void Write(unsigned number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%u", number);
				ConsoleMutex().UnLock();
			}
			static void Write(long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%ld", number);
				ConsoleMutex().UnLock();
			}
			static void Write(unsigned long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%lu", number);
				ConsoleMutex().UnLock();
			}
			static void Write(long long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%lld", number);
				ConsoleMutex().UnLock();
			}
			static void Write(unsigned long long number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%llu", number);
				ConsoleMutex().UnLock();
			}
			static void Write(float number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%f", number);
				ConsoleMutex().UnLock();
			}
			static void Write(double number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%lf", number);
				ConsoleMutex().UnLock();
			}
			static void Write(short number) {
				ConsoleMutex().Lock();
				wprintf_s(L"%hd", number);
				ConsoleMutex().UnLock();
			}

			static void Write(bool boolean) {
				ConsoleMutex().Lock();
				wprintf_s(boolean ? L"True" : L"False");
				ConsoleMutex().UnLock();
			}
			static void WriteLine(bool boolean) {
				ConsoleMutex().Lock();
				wprintf_s(boolean ? L"True" : L"False");
				wprintf_s(L"\n");
				ConsoleMutex().UnLock();
			}
			static void Pause() {
				system("pause");
			}
		};
	}
}