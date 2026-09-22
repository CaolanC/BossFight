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
	ofs.write(reinterpret_cast<const char*>(&file_table), sizeof(FileTable));
};

// Note to caolan: None of this will work untile we store the guids inside of the resource, or pass it in with the resource itself.

void PakFile::add_mesh(CPUMesh cpu_mesh) {
	Entry entry;
	entry.type = Type::Mesh;
	entry.guid = cpu_mesh.guid;
	entry.offset = static_cast<uint64_t>(ofs.tellp()); 

	DrawMetadata draw_metadata;
	draw_metadata.index_count = cpu_mesh.index_count;
	draw_metadata.vertex_count = cpu_mesh.vertex_count;
	draw_metadata.index_type = cpu_mesh.index_type;
	draw_metadata.draw_mode = cpu_mesh.draw_mode;

	uint64_t entry_size = sizeof(DrawMetadata);

	uint64_t indices_size = sizeof(uint8_t) * cpu_mesh.indices.size();
	ofs.write(reinterpret_cast<const char*>(&draw_metadata), sizeof(DrawMetadata));
	ofs.write(reinterpret_cast<const char*>(cpu_mesh.indices.data()), indices_size);

	entry_size += indices_size;

	for(auto& [attr_type, vbo_attr_pair] : cpu_mesh.data) {
		AttributeData attribute_data;
		attribute_data.attribute_type = attr_type;
		attribute_data.vbo_type = vbo_attr_pair.type;
		attribute_data.vertex_attribute = vbo_attr_pair.attribute;
		attribute_data.vbo_data_size = vbo_attr_pair.vbo.size() * sizeof(uint8_t);
		entry_size += sizeof(AttributeData) + attribute_data.vbo_data_size;
		ofs.write(reinterpret_cast<const char*>(&attribute_data), sizeof(AttributeData));
		ofs.write(reinterpret_cast<const char*>(vbo_attr_pair.vbo.data()), attribute_data.vbo_data_size);
		
	};

	entry.size = entry_size;
	
	file_table.entries[entry_count] = entry;
	entry_count++;
};

void PakFile::add_texture(rendering::CPUTexture cpu_texture) {
	Entry entry;
	entry.type = Type::Texture;
	entry.guid = cpu_texture.guid; // Might be better to pass it in alongside it, we might never know :D.
	entry.offset = static_cast<uint64_t>(ofs.tellp());

	TextureMetadata metadata;
	metadata.internal_format = cpu_texture.internal_format;
	metadata.format = cpu_texture.format;
	metadata.width = cpu_texture.width;
	metadata.height = cpu_texture.height;
	metadata.nr_channels = cpu_texture.nrChannels;

	ofs.write(reinterpret_cast<const char*>(&metadata), sizeof(TextureMetadata));
	uint64_t entry_size = sizeof(TextureMetadata);

	uint64_t data_size = static_cast<uint64_t>(cpu_texture.width) * cpu_texture.height * cpu_texture.nrChannels;
	ofs.write(reinterpret_cast<const char*>(cpu_texture.data), data_size);
	entry_size += data_size;

	entry.size = static_cast<uint64_t>(entry_size);

	file_table.entries[entry_count] = entry;
	entry_count++;
};

void PakFile::add_material(rendering::MaterialAsset material_asset) {
	Entry entry;
	entry.type = Type::Material;
	entry.guid = material_asset.handle;
	entry.offset = static_cast<uint64_t>(ofs.tellp());

	MaterialMetadata metadata;
	metadata.material_asset_handle = material_asset.handle;
	metadata.shader_program_handle = material_asset.shader_program_handle;

	if (material_asset.base_color_texture_handle.has_value()) {
		metadata.color_texture_asset_handle = material_asset.base_color_texture_handle.value();
		metadata.has_color_texture = true;
	};

	uint64_t entry_size = sizeof(MaterialMetadata);
	ofs.write(reinterpret_cast<const char*>(&metadata), entry_size);

	entry.size = static_cast<uint64_t>(entry_size);

	file_table.entries[entry_count] = entry;
	entry_count++;
};

void PakFile::add_shader_program(
	rendering::ShaderProgramAsset shader_program_asset,
	rendering::ShaderSource vshader_source,
	rendering::ShaderSource fshader_source
	)
{
	//shader_source.text;
};

void PakFile::close() {
	ofs.seekp(sizeof(Header), std::ios::beg);
	ofs.write(reinterpret_cast<const char*>(&file_table), sizeof(FileTable));
	ofs.close();
};

};
