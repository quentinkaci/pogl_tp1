#pragma once

#include "utils/image.hh"

#include <utility>

const std::string SHADER_NAME = "texture_sampler";

class TextureManager
{
public:
    TextureManager() = default;

    void set_program(GLuint program)
    {
        program_ = program;
    }

    void add_texture(const std::string& filename)
    {
        if (program_ == -1)
            return;

        auto nb_textures = ids_.size();

        auto texture = PNGImage::load(filename);
        GLint texture_loc = glGetUniformLocation(program_, SHADER_NAME.c_str());
        if (texture_loc == -1)
        {
            std::cerr << SHADER_NAME + " is not defined in shaders !" << std::endl;
            return;
        }
        texture_loc_ = texture_loc;

        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glActiveTexture(GL_TEXTURE0 + nb_textures);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.pixels.data());

        glUniform1i(texture_loc, nb_textures);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        ids_.push_back(texture_id);
    }

    void bind_texture(size_t pos)
    {
        if (program_ == -1)
            return;

        glActiveTexture(GL_TEXTURE0 + pos);
        glBindTexture(GL_TEXTURE_2D, ids_[pos]);
        glUniform1i(texture_loc_, pos);
    }

private:
    std::vector<GLuint> ids_;

    GLint texture_loc_;

    GLuint program_ = -1;
};