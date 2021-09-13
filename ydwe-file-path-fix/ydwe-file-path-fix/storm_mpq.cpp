#include <stormlib.h>
#include "storm_mpq.h"
#include <string>

#define print(s) /**/

storm_mpq::storm_mpq()
{
	m_hMpq = NULL;
	m_strPath = "";
}
storm_mpq::storm_mpq(const char* szPath)
{
	if (!SFileOpenArchive(szPath, 0, 0, &m_hMpq))
	{
		print((std::string(szPath) + "-打开mpq失败" + std::to_string(GetLastError())).c_str());
	}
	m_strPath = szPath;
}
storm_mpq::~storm_mpq()
{
	if (!m_hMpq)
	{
		return;
	}
	if (!SFileCloseArchive(m_hMpq))
	{
		print((m_strPath + "-mpq关闭失败").c_str());
	}
}

int storm_mpq::Open(const char* szPath)
{
	if (m_hMpq != NULL)
	{
		if (!Close())
		{
			return 0;
		}
	}
	if (!SFileOpenArchive(szPath, 0, 0, &m_hMpq))
	{
		print((std::string(szPath) + "-打开mpq失败" + std::to_string(GetLastError())).c_str());
		return 0;
	}
	m_strPath = szPath;
	return 1;
}
int storm_mpq::Close()
{
	if (!m_hMpq)
	{
		print((std::string(m_strPath) + "-关闭mpq失败-mpq无效").c_str());
		return 0;
	}
	if (!SFileCloseArchive(m_hMpq))
	{
		print((m_strPath + "-mpq关闭失败").c_str());
		return 0;
	}
	m_hMpq = NULL;
	return 1;
}

int storm_mpq::HasFile(const char* szFileName)
{
	if (!m_hMpq)
	{
		print("文件不存在-mpq无效");
		return 0;
	}
	return SFileHasFile(m_hMpq, szFileName);
}

int storm_mpq::LoadFile(const char* szFileName, const char** pszBuffer, DWORD* pdwSize)
{
	HANDLE hFile;
	if (!m_hMpq)
	{
		print("读取文件失败-mpq无效");
		return 0;
	}
	if (!SFileHasFile(m_hMpq, szFileName))
	{
		print((std::string(szFileName) + "不存在").c_str());
		return 0;
	}
	if (!SFileOpenFileEx(m_hMpq, szFileName, 0, &hFile))
	{
		print(("打开文件失败-" + std::string(szFileName)).c_str());
		return 0;
	}
	DWORD dwSize = SFileGetFileSize(hFile, 0);
	if (!dwSize)
	{
		print("文件大小为0");
		return 0;
	}
	char* szFileData = new char[dwSize + 1];
	DWORD dwRead;
	if (!SFileReadFile(hFile, szFileData, dwSize, &dwRead, NULL))
	{
		print(("读取文件-" + std::string(szFileName) + "内存失败").c_str());
		return 0;
	}
	if (!SFileCloseFile(hFile))
	{
		print(("关闭文件-" + std::string(szFileName) + "失败").c_str());
	}
	szFileData[dwSize] = 0;
	*pdwSize = dwSize;
	*pszBuffer = szFileData;
	return 1;
}

int storm_mpq::UnLoadFile(const char* szFileData)
{
	if (NULL == szFileData)
		return 0;
	if (NULL == *szFileData)
		return 0;
	delete[] szFileData;
	return 1;
}
int storm_mpq::AddFile(const char* szFilePath)
{
	return AddFile(szFilePath, szFilePath);
}
int storm_mpq::AddFile(const char* szFilePath, const char* szArchivePath)
{
	if (!m_hMpq)
	{
		print("添加文件失败-mpq无效");
		return 0;
	}
	if (SFileHasFile(m_hMpq, szArchivePath)) //判断路径已存在文件
	{
		if (!RemoveFile(szArchivePath)) //删除已存在的文件
		{
			return 0;
		}
	}
	if (!SFileAddFile(m_hMpq, szFilePath, szArchivePath, 0x00000200 | 0x80000000))
	{
		print((std::string(szArchivePath) + "添加文件失败").c_str());
		return 0;
	}
	return 1;
}
int storm_mpq::RemoveFile(const char* szFileName)
{
	if (!m_hMpq)
	{
		print("删除文件失败-mpq无效");
		return 0;
	}
	if (!SFileRemoveFile(m_hMpq, szFileName, 0))
	{
		print(("删除文件-" + std::string(szFileName) + "失败").c_str());
		return 0;
	}
	return 1;
}

