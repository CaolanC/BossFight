#pragma once

#include <rendering/NewMesh.hpp>

#include <string>
#include <fstream>

#include <crossguid/guid.hpp>

namespace pak {

#define MAX_ENTRIES 256

enum Type {
	Mesh,
	Material,
	Texture,
	EndEntry
};

class PakFile {
	public:
	PakFile(std::string file_name);
	void add_mesh(CPUMesh cpu_mesh); // Copy for now, see if it's handy and safe to pass as a reference later
	void add_texture(rendering::CPUTexture cpu_texture);
	void add_material();

	void close();

	private:

	uint16_t entry_count = 0;


	#pragma pack(push, 1)

	std::ofstream ofs;

	struct Entry {
		Type type = Type::EndEntry;
		xg::Guid guid;
		uint32_t offset;
		uint32_t size;
	};

	struct FileTable {
		Entry entries[MAX_ENTRIES];
	};

	struct DrawMetadata {
		uint32_t vertex_count;
		uint32_t index_count;
		GLenum index_type;
		GLenum draw_mode;
	};

	// The index data goes between the draw metadata and the attribute Data, using the index_count for its width.

	struct AtrributeData {
		AttributeType attribute_type;
		VBO_Type vbo_type;
		VertexAttribute vertex_attribute;
		uint64_t vbo_data_size;
	};

	// Then we write the VBO_DATA itself, and do this for each attribute. So it's AttributeData, vbo_data, AttributeData, vbo_data, and so on and so forth.

	struct Header {
		char magic[4] = {'P', 'A', 'C', 'K'};
		uint32_t size = sizeof(Entry) * MAX_ENTRIES; // Size of the file table. Could've probabaly just done sizeof FileTable
		uint32_t offset = sizeof(Header); // Offset to beginning of file table
	};

	#pragma pack(pop)

	FileTable file_table;
};

};
