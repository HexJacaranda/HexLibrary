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
				std::wcout << Message;
			}
			static void Write(const char&Message) {
				std::cout << Message;
			}
			static void Write(const wchar_t&Message) {
				std::wcout << Message;
			}
			template<class...Args>
			static void WriteLine(const wchar_t*Format, Args const&...args) {
				WriteLine(String::Format(Format, args...));
			}
			static void WriteLine(const wchar_t*Message) {
				std::wcout << Message << std::endl;
			}

			static void WriteLine(String const&Message) {
				std::wcout << Message << std::endl;
			}
			static void WriteLine(const char*Message) {
				std::cout << Message << std::endl;
			}
			static void WriteLine(const char&Message) {
				std::cout << Message << std::endl;
			}
			static void WriteLine(const wchar_t&Message) {
				std::wcout << Message << std::endl;
			}
			//数值类型
			static void WriteLine(int number) {
				ConsoleMutex().Lock();
				std::wcout << number << std::endl;
				ConsoleMutex().UnLock();
			}
			static void WriteLine(unsigned number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(long number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(unsigned long number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(long long number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(unsigned long long number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(float number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(double number) {
				std::wcout << number << std::endl;
			}
			static void WriteLine(short number) {
				std::wcout << number << std::endl;
			}

			static void Write(int number) {
				std::wcout << number;
			}
			static void Write(unsigned number) {
				std::wcout << number;
			}
			static void Write(long number) {
				std::wcout << number;
			}
			static void Write(unsigned long number) {
				std::wcout << number;
			}
			static void Write(long long number) {
				std::wcout << number;
			}
			static void Write(unsigned long long number) {
				std::wcout << number;
			}
			static void Write(float number) {
				std::wcout << number;
			}
			static void Write(double number) {
				std::wcout << number;
			}
			static void Write(short number) {
				std::wcout << number;
			}
			static void Write(bool boolean) {
				std::wcout << (boolean ? L"True" : L"False");
			}
			static void WriteLine(bool boolean) {
				std::wcout << (boolean ? L"True" : L"False") << std::endl;
			}
			static void Pause() {
				system("pause");
			}
		};
	}
}