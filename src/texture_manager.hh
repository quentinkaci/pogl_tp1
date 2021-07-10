#pragma once

#include "utils/image.hh"

#include <utility>

const std::string SHADER_NAME = "texture_sampler";

class TextureManager
{
public:
    TextureManager() = default;

    void add_cube_map_texture(const std::vector<std::string>& faces_filename)
    {
        GLuint cube_map_id;

        glGenTextures(1, &cube_map_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_id);

        for (unsigned int i = 0; i < faces_filename.size(); i++)
        {
            auto image = PNGImage::load(faces_filename[i]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.pixels.data());
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        ids_.push_back(cube_map_id);
    }

    void set_program(GLuint program)
    {
        program_ = program;
    }

    void add_texture(const std::string& filename)
    {
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
        glActiveTexture(GL_TEXTURE0 + pos);
        glBindTexture(GL_TEXTURE_2D, ids_[pos]);
        glUniform1i(texture_loc_, pos);
    }

    void bind_cube_map_texture(size_t pos)
    {
        glActiveTexture(GL_TEXTURE0 + pos);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ids_[pos]);
    }

private:
    std::vector<GLuint> ids_;

    GLint texture_loc_;

    GLuint program_ = -1;
};