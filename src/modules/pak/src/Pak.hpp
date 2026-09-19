#pragma once

#include <rendering/NewMesh.hpp>

namespace pak {

class PakFile {
	public:
	PakFile();
	void add_mesh();
	void add_textuure();
	void add_material();

	private:


	#pragma pack(push, 1)
	struct Header {
		char magic[4];
	};

	struct FileTable {
		
	};

	#pragma pack(pop)
};

};
