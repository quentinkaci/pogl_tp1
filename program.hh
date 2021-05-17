#pragma once

#include <GL/glew.h>
#include <memory>

namespace mygl
{
    class program
    {
    public:
        program() = default;

        ~program() { glDeleteProgram(program_); }

        inline static std::shared_ptr<program> make_program(const std::string& vertex_shader_src, const std::string& fragment_shader_src)
        {
            auto res = std::make_shared<program>();

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

                res->last_log = &infoLog[0];

                return res;
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

                res->last_log = &infoLog[0];

                return res;
            }

            // Vertex and fragment shaders are successfully compiled.
            // Now time to link them together into a program.
            // Get a program object.
            GLuint program = glCreateProgram();

            res->program_ = program;

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

                res->last_log = &infoLog[0];

                return res;
            }

            // Always detach shaders after a successful link.
            glDetachShader(program, vertexShader);
            glDetachShader(program, fragmentShader);

            res->is_ready_ = true;

            return res;
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
} // namespace mygl