#include "helper.h"
#include <string>
#include <base/hook/iat.h>
#include <base/hook/fp_call.h>
#include <base/util/unicode.h>
#include <base/hook/inline.h>
#include <stdio.h>
#include <filesystem>
#include <map> 
#include "storm_mpq.h"
#include <fstream>

namespace fs = std::filesystem;

uint32_t real_fopen;
uint32_t real_CreateDirectoryA;
uint32_t real_CreateFileA;

uint32_t real_saveArchive;

uint32_t real_addFile;



std::map<std::string, std::string> file_map;
std::string temp_save_path;



uintptr_t getAddress(uintptr_t addr)
{
	const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
	return addr - 0x00400000 + base;
}


std::string covert_local(std::string filename) {

	auto pos = filename.find(temp_save_path);

	if (pos != filename.npos) {
		std::string local = temp_save_path + base::u2a(filename.substr(pos + temp_save_path.length()));
		return local;
	}
	return filename;
}


BOOL __stdcall fake_CreateDirectoryA(const char* filename, void* attribute) {
	std::string name = covert_local(filename).c_str();
	
	base::std_call<BOOL>(real_CreateDirectoryA, name.c_str(), attribute);
	return 1;
}


FILE* __cdecl fake_fopen(const char* filename, const char* mode) {

	std::string name = covert_local(filename).c_str();
	return base::c_call<FILE*>(real_fopen, name.c_str(), mode);

}

int __fastcall fake_saveAchive(void* editdata, uint32_t unknow, const char* path, uint32_t num) {
	fs::path temppath = fs::path(path).parent_path();
	temp_save_path = temppath.string();
	file_map.clear();

	if (base::fast_call<int>(real_saveArchive, editdata, unknow, path, num)) {
		storm_mpq mpq;

		if (mpq.Open(path)) {
			std::string str = "";
			for (auto&& [k, v] : file_map) {
				int r = mpq.AddFile(k.c_str(), v.c_str());
				str += v + "\r\n";
			}

			std::ofstream file(temppath / "(listfile)", std::ios::binary);
			file << str;
			file.close();
			mpq.AddFile((temppath / "(listfile)").string().c_str(), "(listfile)");
			mpq.Compact();
			mpq.Close();
		}

	
		return 1;
	}

	return 0;
}

int __fastcall fake_addFile(uint32_t unknow1, uint32_t unknow2, uint32_t object1, const char* filename, const char* archive_file_path, uint32_t object2) {

	std::string name = covert_local(filename).c_str();

	if (name.length() != strlen(filename))
	{
		file_map[name] = archive_file_path;
	}
	else {
		return base::fast_call<int>(real_addFile, unknow1, unknow2, object1, name.c_str(), archive_file_path, object2);
	}

	return 1;
}

Helper::Helper() {
	m_attach = false;
}

Helper::~Helper() {

}

void Helper::attach() {
	if (m_attach) return;


	char buffer[MAX_PATH];

	GetModuleFileNameA(nullptr, buffer, MAX_PATH);

	std::string name = fs::path(buffer).filename().string();
	if (std::string::npos == name.find("worldedit")
		&& std::string::npos == name.find("WorldEdit"))
		return;


	m_attach = true;

	real_CreateDirectoryA = base::hook::iat(GetModuleHandle(NULL), "kernel32.dll", "CreateDirectoryA", (uintptr_t)&fake_CreateDirectoryA);

	real_fopen = base::hook::iat(GetModuleHandle(NULL), "msvcrt.dll", "fopen", (uintptr_t)&fake_fopen);

	real_saveArchive = getAddress(0x0055D720);

	base::hook::inline_install(&real_saveArchive, (uintptr_t)&fake_saveAchive);

	real_addFile = getAddress(0x004038A0);
	base::hook::inline_install(&real_addFile, (uintptr_t)&fake_addFile);

}

void Helper::detach() {
	if (!m_attach) return;

	m_attach = false;

	base::hook::iat(GetModuleHandle(NULL), "kernel32.dll", "CreateDirectoryA", real_CreateDirectoryA);
	base::hook::iat(GetModuleHandle(NULL), "msvcrt.dll", "fopen", real_fopen);

	base::hook::inline_uninstall(&real_saveArchive, (uintptr_t)&fake_saveAchive);
	base::hook::inline_uninstall(&real_addFile, (uintptr_t)&fake_addFile);
}



Helper& get_helper() {
	static Helper instance;
	return instance;
}