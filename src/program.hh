#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>

namespace mygl
{
    class Programs
    {
    protected:
        static std::shared_ptr<Programs> singleton_;

    public:
        static std::shared_ptr<Programs> get_instance()
        {
            if (singleton_ == nullptr)
                singleton_ = std::make_shared<Programs>();

            return singleton_;
        }

        Programs() = default;

        ~Programs()
        {
            for (const auto& p : programs_)
                glDeleteProgram(p);
        }

        inline void add_program(const std::string& vertex_shader_src, const std::string& fragment_shader_src)
        {
            // Code adapted from https://www.khronos.org/opengl/wiki/Shader_Compilation

            size_t nb_programs = programs_.size();

            singleton_->programs_.push_back(-1);

            GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

            const auto* source = (const GLchar*)vertex_shader_src.c_str();
            glShaderSource(vertex_shader, 1, &source, 0);

            glCompileShader(vertex_shader);

            GLint is_compiled = 0;
            glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
            if (is_compiled == GL_FALSE)
            {
                GLint max_length = 0;
                glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

                std::vector<GLchar> info_log(max_length);
                glGetShaderInfoLog(vertex_shader, max_length, &max_length, &info_log[0]);

                glDeleteShader(vertex_shader);

                singleton_->last_log = &info_log[0];
            }

            GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

            source = (const GLchar*)fragment_shader_src.c_str();
            glShaderSource(fragment_shader, 1, &source, 0);

            glCompileShader(fragment_shader);

            glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
            if (is_compiled == GL_FALSE)
            {
                GLint max_length = 0;
                glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

                std::vector<GLchar> info_log(max_length);
                glGetShaderInfoLog(fragment_shader, max_length, &max_length, &info_log[0]);

                glDeleteShader(fragment_shader);
                glDeleteShader(vertex_shader);

                singleton_->last_log = &info_log[0];
            }

            GLuint program = glCreateProgram();

            singleton_->programs_[nb_programs] = program;

            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);
            glLinkProgram(program);

            GLint is_linked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, (int*)&is_linked);
            if (is_linked == GL_FALSE)
            {
                GLint max_length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

                std::vector<GLchar> info_log(max_length);
                glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

                glDeleteProgram(program);
                glDeleteShader(vertex_shader);
                glDeleteShader(fragment_shader);

                singleton_->last_log = &info_log[0];
            }

            glDetachShader(program, vertex_shader);
            glDetachShader(program, fragment_shader);

            singleton_->is_last_ready_ = true;
        };

        inline std::string get_last_log()
        {
            return last_log;
        };

        [[nodiscard]] inline bool is_last_ready() const { return is_last_ready_; };

        inline void use(size_t pos) const
        {
            glUseProgram(programs_[pos]);
        };

        inline GLuint get_id(size_t pos) const { return programs_[pos]; }

    private:
        std::vector<GLuint> programs_;

        std::string last_log;

        bool is_last_ready_;
    };

    std::shared_ptr<Programs> Programs::singleton_ = nullptr;
} // namespace mygl