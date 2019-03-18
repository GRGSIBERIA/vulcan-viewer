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

vlc::rio::RichIO::RichIO(const std::string path)
	: filePath(path)
{
}

void vlc::rio::RichIO::Read(FileInfo & info)
{
	
}
