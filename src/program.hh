#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>

namespace mygl
{
    class Program
    {
    protected:
        static std::shared_ptr<Program> singleton_;

    public:
        static std::shared_ptr<Program> get_instance()
        {
            return singleton_;
        }

        Program()
        {
        }

        ~Program() { glDeleteProgram(program_); }

        inline static std::shared_ptr<Program> make_program(const std::string& vertex_shader_src, const std::string& fragment_shader_src)
        {
            if (singleton_ != nullptr)
                return singleton_;

            singleton_ = std::make_shared<Program>();

            // Create an empty vertex shader handle
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

            // Send the vertex shader source code to GL
            const auto* source = (const GLchar*)vertex_shader_src.c_str();
            glShaderSource(vertexShader, 1, &source, 0);

            // Compile the vertex shader
            glCompileShader(vertexShader);

            GLint isCompiled = 0;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

                // We don't need the shader anymore.
                glDeleteShader(vertexShader);

                singleton_->last_log = &infoLog[0];

                return singleton_;
            }

            // Create an empty fragment shader handle
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

            // Send the fragment shader source code to GL
            source = (const GLchar*)fragment_shader_src.c_str();
            glShaderSource(fragmentShader, 1, &source, 0);

            // Compile the fragment shader
            glCompileShader(fragmentShader);

            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

                // We don't need the shader anymore.
                glDeleteShader(fragmentShader);
                // Either of them. Don't leak shaders.
                glDeleteShader(vertexShader);

                singleton_->last_log = &infoLog[0];

                return singleton_;
            }

            // Vertex and fragment shaders are successfully compiled.
            // Now time to link them together into a program.
            // Get a program object.
            GLuint program = glCreateProgram();

            singleton_->program_ = program;

            // Attach our shaders to our program
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);

            // Link our program
            glLinkProgram(program);

            // Note the different functions here: glGetProgram* instead of glGetShader*.
            GLint isLinked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

                // We don't need the program anymore.
                glDeleteProgram(program);
                // Don't leak shaders either.
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);

                singleton_->last_log = &infoLog[0];

                return singleton_;
            }

            // Always detach shaders after a successful link.
            glDetachShader(program, vertexShader);
            glDetachShader(program, fragmentShader);

            singleton_->is_ready_ = true;

            return singleton_;
        };

        inline std::string get_log()
        {
            return last_log;
        };

        [[nodiscard]] inline bool is_ready() const { return is_ready_; };

        inline void use() const
        {
            glUseProgram(program_);
        };

        inline GLuint get_id() const { return program_; }

    private:
        GLuint program_{};

        std::string last_log;

        bool is_ready_ = false;
    };

    std::shared_ptr<Program> Program::singleton_ = nullptr;
} // namespace mygl