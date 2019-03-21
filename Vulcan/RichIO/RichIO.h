#pragma once
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h>

namespace vlc
{
	namespace rio
	{
		class FileInfo;
		class RichIO;

		typedef std::vector<FileInfo> FileInfoArray;
		typedef std::vector<void*> BinrayArray;
		typedef std::vector<FILE*> FileArray;
		typedef long long DataInt;

		/**
		* 読み込んだときにoffsetとsizeの間で内部容量との不整合を起こした
		*/
		class ReadingOverloadException : std::exception 
		{
		public:
			ReadingOverloadException(const FileInfo& info, const DataInt total);
		};
		
		/**
		* ファイル情報を扱うための構造体
		* ファイルのオフセットやサイズは64ビットを想定している
		* 読み込み時にメモリ確保を行い、デストラクタで解放する
		*/
		class FileInfo
		{
			friend RichIO;
			
			DataInt id;			// 配列上のID
			bool sortMode;		// ソートするときにtrueはID順, falseはオフセット順

		public:
			void* buffer;		// バイナリへのポインタ
			DataInt offset;		// オフセット
			DataInt size;		// サイズ
			

			FileInfo(void* buffer, const DataInt offset, const DataInt size);

			FileInfo();
			~FileInfo();

			FileInfo& operator=(FileInfo& info);
			bool operator<(const FileInfo& info) const;

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