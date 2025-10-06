//
// Created by ethan on 05/10/2025.
//

#ifndef SOULJA_TEXTURE_H
#define SOULJA_TEXTURE_H

#include <glad/glad.h>
#include <iostream>

class Texture {
public:
    unsigned int ID;

    Texture(int* objs, const char* imagepath);
    void Bind();
};

#endif //SOULJA_T EXTURE_H