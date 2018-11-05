#pragma once
namespace HL
{
	namespace System
	{
		class File {
		public:
			enum WriteMode {
				Append,
				Cover
			};
			static String Read(String const& path) {
				return Read(path.GetData());
			}
			static String Read(const wchar_t*path) {
				FILE*fh = nullptr;
				_wfopen_s(&fh, path, L"r");
				if (!fh)
					return String((size_t)0);
				long len = FileSizeOf(path) + 1;
				wchar_t*buf = (wchar_t*)malloc(sizeof(wchar_t)*len);
				if (buf == nullptr)
					return String((size_t)0);
				wchar_t*write = buf;
				while (!feof(fh))
				{
					*write = fgetwc(fh);
					write++;
				}
				fclose(fh);
				return String::ManageFrom(buf, len, len);
			}
			static void Write(String const& path, String const& content, WriteMode mode) {
				Write(path.GetData(), content.GetData(), mode);
			}
			static void Write(const wchar_t*path, const wchar_t*content, WriteMode mode) {
				FILE*fh = nullptr;
				if (mode == Append)
					_wfopen_s(&fh, path, L"a");
				else
					_wfopen_s(&fh, path, L"w");
				if (fh == nullptr)
					return;
				fputws(content, fh);
				fclose(fh);
			}
			static UPointer::uptr<Threading::Task<String>> ReadAsync(String const&path)
			{
				return ReadAsync(path.GetData());
			}
			static UPointer::uptr<Threading::Task<String>> ReadAsync(const wchar_t*path) {
				return Threading::Task<void>::Run([](const wchar_t*in) { return Forward(File::Read(in)); }, path);
			}
			static long FileSizeOf(String const&path) {
				return FileSizeOf(path.GetData());
			}
			static long FileSizeOf(const wchar_t*path) {
				FILE*fh = nullptr;
				_wfopen_s(&fh, path, L"r");
				if (fh == nullptr)
					return 0;
				fseek(fh, 0, SEEK_END);
				long r = ftell(fh);
				fclose(fh);
				return r;
			}
		};
	}
}