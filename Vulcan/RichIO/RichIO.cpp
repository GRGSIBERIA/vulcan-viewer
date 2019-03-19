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

void vlc::rio::RichIO::AllocCheck(const uint64_t infosize)
{
	if (bufsize < infosize)
	{
		free(buffer);
		buffer = malloc(infosize);
	}
}

FILE * vlc::rio::RichIO::FileOpen(const uint64_t size, const char * mode)
{
	FILE* fp = fopen(filePath.c_str(), mode);
	setvbuf(fp, (char*)buffer, _IOFBF, size);
	return fp;
}

// ファイルを取得する関数
FILE* vlc::rio::RichIO::GetFileOne(const FileInfo& info, const char* mode)
{
	AllocCheck(info.size);	// ファイルバッファの容量を確保

	// ファイルを開いてバッファリング領域を取得する
	return FileOpen(info.size, "rb");
}

// 複数のファイル情報を読み込んで一気にsetvbufする関数
FILE* vlc::rio::RichIO::GetFileMany(const FileInfoArray & infos, const char * mode)
{
	uint64_t total = 0;
	for (int i = 0; i < infos.size(); ++i)
		total += infos[i].size;

	AllocCheck(total);

	return FileOpen(total, "rb");
}



vlc::rio::RichIO::RichIO(const std::string path)
	: filePath(path), bufsize(1)
{
	buffer = malloc(bufsize);

	FILE* fp = fopen(filePath.c_str(), "ab");
	_fseek_nolock(fp, 0, SEEK_SET);
	_fread_nolock_s(&realsize, sizeof(realsize), sizeof(realsize), 1, fp);	// 先頭8バイトでファイルの実際のサイズがわかる
	fclose(fp);
}

void vlc::rio::RichIO::Write(FileInfo & info)
{
	FILE* fp = GetFileOne(info, "ab");

	info.offset = realsize;
	realsize += info.size;

	filesize = _fseeki64_nolock(fp, 0, SEEK_END);
	if (realsize > filesize)
	{
		// 内部サイズがファイルサイズを超えた場合、ファイルサイズを2倍にする
		
	}
	
	_fseeki64_nolock(fp, realsize, SEEK_SET);
	_fwrite_nolock(info.binary, info.size, 1, fp);

	fclose(fp);
}


void vlc::rio::RichIO::Write(FileInfoArray & infos)
{
	FILE* fp = GetFileMany(infos, "ab");

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
