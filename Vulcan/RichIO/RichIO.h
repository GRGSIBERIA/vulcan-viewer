#pragma once
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h>

namespace vlc
{
	namespace rio
	{
		typedef std::vector<FileInfo> FileInfoArray;
		typedef std::vector<void*> BinrayArray;
		
		/**
		* ファイル情報を扱うための構造体
		* 64ビットを想定している
		*/
		struct FileInfo
		{
			const void* binary;	// バイナリへのポインタ
			uint64_t offset;	// オフセット
			uint64_t size;		// サイズ

			FileInfo(const void* binary, const uint64_t offset, const uint64_t size);

			FileInfo();

			FileInfo& operator=(FileInfo& info);

			static FileInfo ToWrite(const void* binary, const uint64_t size);

			static FileInfo ToRead(const uint64_t offset, const uint64_t size);
		};

		

		/**
		* 贅沢な読み書きを提供する
		*/
		class RichIO
		{
			const std::string filePath;

		public:
			RichIO(const std::string path);

			void Write(FileInfo& info);

			void Write(FileInfoArray& infos);

			void Read(FileInfo& info);

			void Read(FileInfoArray& infos)
		};
	}
}