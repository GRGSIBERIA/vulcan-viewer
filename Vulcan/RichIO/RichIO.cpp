#include "RichIO.h"
using namespace vlc::rio;

vlc::rio::FileInfo::FileInfo(void* buffer, const DataInt offset, const DataInt size)
	: offset(offset), size(size), buffer(buffer)
{
}

vlc::rio::FileInfo::FileInfo()
	: offset(0), size(0), buffer(nullptr)
{
}

vlc::rio::FileInfo::~FileInfo()
{
	if (buffer != nullptr)
		free(buffer);
}

FileInfo & vlc::rio::FileInfo::operator=(FileInfo & info)
{
	buffer = info.buffer;
	offset = info.offset;
	size = info.size;
}

FileInfo vlc::rio::FileInfo::ToWrite(void * buffer, const DataInt size)
{
	return FileInfo(buffer, 0, size);
}

FileInfo vlc::rio::FileInfo::ToRead(const DataInt offset, const DataInt size)
{
	return FileInfo(nullptr, offset, size);
}

FILE * vlc::rio::RichIO::FileOpen(const DataInt size, const char * mode)
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
	DataInt total = 0;
	for (int i = 0; i < infos.size(); ++i)
		total += infos[i].size;

	return FileOpen(total, "rb");
}

void vlc::rio::RichIO::CheckOverSize(FILE* fp)
{
	// 内部サイズがファイルサイズを超えたらファイルサイズを増やす
	filesize = _fseeki64_nolock(fp, 0, SEEK_END);
	while (realsize > filesize)
	{
		const auto temp = filesize;
		filesize = filesize * 1.5;

		// 適当に空のバッファを用意してファイルに書き込む
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
		realsize = sizeof(DataInt);
		WriteRealSize(fp);
	}

	_fseek_nolock(fp, 0, SEEK_SET);
	_fread_nolock_s(&realsize, sizeof(DataInt), sizeof(DataInt), 1, fp);	// 先頭8バイトでファイルの実際のサイズがわかる
	fclose(fp);
}

void vlc::rio::RichIO::WriteBinary(FileInfo & info, FILE * fp)
{
	info.offset = realsize;
	realsize += info.size;

	CheckOverSize(fp);	// 内部サイズがファイルサイズを超えたらファイルの容量を増やす

	_fseeki64_nolock(fp, realsize, SEEK_SET);
	_fwrite_nolock(info.buffer, info.size, 1, fp);
}

// ファイル先頭の内部容量に書き込む
void vlc::rio::RichIO::WriteRealSize(FILE * fp)
{
	_fseeki64_nolock(fp, 0, SEEK_SET);;
	_fwrite_nolock(&realsize, sizeof(DataInt), 1, fp);
}

// ファイルから適当なバイナリをbufferへ読み込む
void vlc::rio::RichIO::ReadBinary(FileInfo & info, FILE * fp)
{
	info.buffer = malloc(info.size);
	_fseeki64_nolock(fp, info.offset, SEEK_SET);
	_fread_nolock_s(info.buffer, info.size, info.size, 1, fp);
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

	ReadBinary(info, fp);	// bufferにメモリが確保されるので注意

	fclose(fp);
}

void vlc::rio::RichIO::Read(FileInfoArray & infos)
{
	FILE* fp = GetFileMany(infos, "rb");

	for (auto& info : infos)	// bufferにメモリが確保される
		ReadBinary(info, fp);

	fclose(fp);
}
