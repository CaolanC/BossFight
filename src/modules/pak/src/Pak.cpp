#include <Pak.hpp>
#include <rendering/NewMesh.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace pak {

namespace fs = std::filesystem;

PakFile::PakFile(std::string file_name) {
	fs::path output_path = fs::path(SLJA_ASSETS_DIR) / "pakfiles" / file_name;
	ofs.open(output_path, std::ios::binary | std::ios::trunc | std::ios::out);
	Header header;
	ofs.write(reinterpret_cast<const char*>(&header), sizeof(Header));
};

void PakFile::add_mesh(CPUMesh cpu_mesh) {
	Entry entry;

	
	
	//entry.offset = sizeof(Header) + sizeof(FileTable) + sizeof(Entry) * entry_count; // This doesn't work.
	
	//file_table[entry_count] = entry;
	entry_count++;
};

void PakFile::close() {
	ofs.close();
};

};
