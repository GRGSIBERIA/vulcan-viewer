#include "RichIO.h"
using namespace vlc::rio;

vlc::rio::FileInfo::FileInfo(const void* binary, const uint64_t offset, const uint64_t size)
	: offset(offset), size(size), binary(binary)
{
}

vlc::rio::FileInfo::FileInfo()
	: offset(0), size(0), binary(nullptr)
{
}

FileInfo & vlc::rio::FileInfo::operator=(FileInfo & info)
{
	binary = info.binary;
	offset = info.offset;
	size = info.size;
}

FileInfo vlc::rio::FileInfo::ToWrite(const void * binary, const uint64_t size)
{
	return FileInfo(binary, 0, size);
}

FileInfo vlc::rio::FileInfo::ToRead(const uint64_t offset, const uint64_t size)
{
	return FileInfo(nullptr, offset, size);
}

FILE * vlc::rio::RichIO::FileOpen(const uint64_t size, const char * mode)
{
	// setvbufにNULLを指定すると自動的に容量を確保する
	// その代わりファイルを開くたびにmallocされるが
	// 大量に呼び出すことはないと思うのでそこまでオーバーヘッドにはならないと思う
	FILE* fp = fopen(filePath.c_str(), mode);
	setvbuf(fp, NULL, _IOFBF, size);	
	return fp;
}

// ファイルを取得する関数
FILE* vlc::rio::RichIO::GetFileOne(const FileInfo& info, const char* mode)
{
	// ファイルを開いてバッファリング領域を取得する
	return FileOpen(info.size, "rb");
}

// 複数のファイル情報を読み込んで一気にsetvbufする関数
FILE* vlc::rio::RichIO::GetFileMany(const FileInfoArray & infos, const char * mode)
{
	uint64_t total = 0;
	for (int i = 0; i < infos.size(); ++i)
		total += infos[i].size;

	return FileOpen(total, "rb");
}

void vlc::rio::RichIO::CheckOverSize(FILE* fp)
{
	// 内部サイズがファイルサイズを超えた場合、ファイルサイズを増やす
	filesize = _fseeki64_nolock(fp, 0, SEEK_END);
	while (realsize > filesize)
	{
		const auto temp = filesize;
		filesize = filesize * 1.5;

		const auto allocsize = filesize - temp;
		void* allocation = malloc(allocsize);
		memset(allocation, 0, allocsize);
		_fwrite_nolock(allocation, allocsize, 1, fp);
		_fflush_nolock(fp);
		free(allocation);
	}
}

vlc::rio::RichIO::RichIO(const std::string path)
	: filePath(path)
{
	// 先頭8バイト読み込んで、実際に使用している容量を取得する
	FILE* fp = fopen(filePath.c_str(), "ab");

	// ファイルサイズが0に等しかったら内部容量を追記する
	auto size = _fseeki64_nolock(fp, 0, SEEK_END);
	if (size <= 0)
	{
		realsize = sizeof(uint64_t);
		WriteRealSize(fp);
	}

	_fseek_nolock(fp, 0, SEEK_SET);
	_fread_nolock_s(&realsize, sizeof(uint64_t), sizeof(uint64_t), 1, fp);	// 先頭8バイトでファイルの実際のサイズがわかる
	fclose(fp);
}

void vlc::rio::RichIO::WriteBinary(FileInfo & info, FILE * fp)
{
	info.offset = realsize;
	realsize += info.size;

	CheckOverSize(fp);	// 内部サイズがファイルサイズを超えたらファイルの容量を増やす

	_fseeki64_nolock(fp, realsize, SEEK_SET);
	_fwrite_nolock(info.binary, info.size, 1, fp);
}

// ファイル先頭の内部容量に書き込む
void vlc::rio::RichIO::WriteRealSize(FILE * fp)
{
	_fseeki64_nolock(fp, 0, SEEK_SET);;
	_fwrite_nolock(&realsize, sizeof(uint64_t), 1, fp);
}

void vlc::rio::RichIO::Write(FileInfo & info)
{
	FILE* fp = GetFileOne(info, "ab");
	{
		WriteBinary(info, fp);
		_fflush_nolock(fp);
	}
	WriteRealSize(fp);

	fclose(fp);
}

void vlc::rio::RichIO::Write(FileInfoArray & infos)
{
	FILE* fp = GetFileMany(infos, "ab");
	{
		for (auto& info : infos)
			WriteBinary(info, fp);
		_fflush_nolock(fp);
	}
	WriteRealSize(fp);	// 書き込み終わったら急なシャットダウンでデストラクタが呼ばれなくともどうにかなる対策を施す

	fclose(fp);
}

void vlc::rio::RichIO::Read(FileInfo & info)
{
	FILE* fp = GetFileOne(info, "rb");

	fclose(fp);
}

void vlc::rio::RichIO::Read(FileInfoArray & infos)
{
	FILE* fp = GetFileMany(infos, "rb");

	fclose(fp);
}
