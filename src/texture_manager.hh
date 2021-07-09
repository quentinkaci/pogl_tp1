#pragma once

#include "utils/image.hh"

#include <utility>

class TextureManager
{
public:
    TextureManager(std::shared_ptr<mygl::program> program) : program_(std::move(program)) {}

    void add_texture(const std::string& filename, const std::string& shader_name)
    {
        auto texture = PNGImage::load(filename);
        GLint texture_loc = glGetUniformLocation(program_->get_id(), shader_name.c_str());
        if (texture_loc == -1)
        {
            std::cerr << shader_name + " is not defined in shaders !" << std::endl;
            return;
        }

        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glActiveTexture(GL_TEXTURE0 + nb_texture_);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.pixels.data());

        glUniform1i(texture_loc, nb_texture_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        ++nb_texture_;
    }

private:
    int nb_texture_ = 0;

    std::shared_ptr<mygl::program> program_ = nullptr;
};