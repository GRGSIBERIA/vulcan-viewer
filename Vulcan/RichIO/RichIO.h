#pragma once
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h>

namespace vlc
{
	namespace rio
	{
		struct FileInfo;

		typedef std::vector<FileInfo> FileInfoArray;
		typedef std::vector<void*> BinrayArray;
		typedef std::vector<FILE*> FileArray;
		typedef long long DataInt;
		
		/**
		* ファイル情報を扱うための構造体
		* 64ビットを想定している
		*/
		struct FileInfo
		{
			void* buffer;	// バイナリへのポインタ
			DataInt offset;	// オフセット
			DataInt size;		// サイズ

			FileInfo(void* buffer, const DataInt offset, const DataInt size);

			FileInfo();
			~FileInfo();

			FileInfo& operator=(FileInfo& info);

			static FileInfo ToWrite(void* buffer, const DataInt size);

			static FileInfo ToRead(const DataInt offset, const DataInt size);
		};

		

		/**
		* 贅沢な読み書きを提供する
		*/
		class RichIO
		{
			const std::string filePath;

			DataInt realsize;	// 内部的なファイルサイズ
			DataInt filesize;	// 実ファイルのサイズ

		private:
			void WriteBinary(FileInfo& info, FILE* fp);
			void WriteRealSize(FILE* fp);
			void ReadBinary(FileInfo& info, FILE* fp);

			FILE* FileOpen(const DataInt size, const char* mode);
			FILE* GetFileOne(const FileInfo& info, const char* mode);
			FILE* GetFileMany(const FileInfoArray& infos, const char* mode);
			void CheckOverSize(FILE* fp);

		public:
			RichIO(const std::string path);

			void Write(FileInfo& info);

			void Write(FileInfoArray& infos);

			void Read(FileInfo& info);

			void Read(FileInfoArray& infos);
		};
	}
}