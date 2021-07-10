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
            size_t nb_programs = programs_.size();

            singleton_->programs_.push_back(-1);

            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

            const auto* source = (const GLchar*)vertex_shader_src.c_str();
            glShaderSource(vertexShader, 1, &source, 0);

            glCompileShader(vertexShader);

            GLint isCompiled = 0;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(vertexShader);

                singleton_->last_log = &infoLog[0];
            }

            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

            source = (const GLchar*)fragment_shader_src.c_str();
            glShaderSource(fragmentShader, 1, &source, 0);

            glCompileShader(fragmentShader);

            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(fragmentShader);
                glDeleteShader(vertexShader);

                singleton_->last_log = &infoLog[0];
            }

            GLuint program = glCreateProgram();

            singleton_->programs_[nb_programs] = program;

            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);

            GLint isLinked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

                glDeleteProgram(program);
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);

                singleton_->last_log = &infoLog[0];
            }

            glDetachShader(program, vertexShader);
            glDetachShader(program, fragmentShader);

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