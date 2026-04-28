#include <utils/Texture.hpp>
#include <tiny_gltf.h>

class TextureManager {
public:
    utils::Texture* make(int* objs, const char* imagePath);

    std::vector<utils::Texture> makeglTF(tinygltf::Model model);
};
