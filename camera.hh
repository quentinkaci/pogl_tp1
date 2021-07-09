#pragma once

#include <GL/glut.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace mygl
{
    class camera
    {
    protected:
        camera()
        {
        }

        static camera* singleton_;

    public:
        static camera* get_instance();

        glm::mat4 get_world_to_cam_matrix() const
        {
            glm::mat4 projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
            glm::mat4 world_to_cam_matrix = projection * view;
            return world_to_cam_matrix;
        }

        void key_callback(int key, int, int)
        {
            if (key == GLUT_KEY_UP)
                camera_pos = camera_pos + camera_speed * camera_front;
            else if (key == GLUT_KEY_DOWN)
                camera_pos = camera_pos - camera_speed * camera_front;
            else if (key == GLUT_KEY_LEFT)
                camera_pos = camera_pos - normalize(cross(camera_front, camera_up)) * camera_speed;
            else if (key == GLUT_KEY_RIGHT)
                camera_pos = camera_pos + normalize(cross(camera_front, camera_up)) * camera_speed;

            glutPostRedisplay();
        }

        void mouse_motion_callback(int x, int y)
        {
            float xoffset = x - lastX;
            float yoffset = lastY - y;
            lastX = x;
            lastY = y;

            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            pitch = std::clamp(pitch, -89.0f, 89.0f);

            glm::vec3 direction;
            direction[0] = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction[1] = sin(glm::radians(pitch));
            direction[2] = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            camera_front = normalize(direction);

            glutPostRedisplay();
        }

        void mouse_callback(int button, int state, int x, int y)
        {
            if (button == 3)
                fov -= (float)1;
            else if (button == 4)
                fov += (float)1;
            else if (button == GLUT_LEFT_BUTTON)
            {
                if (state == GLUT_DOWN)
                {
                    lastX = x;
                    lastY = y;
                    firstMouse = false;
                }
                else if (state == GLUT_UP)
                    firstMouse = true;
            }

            fov = std::clamp(fov, 1.0f, 90.0f);

            glutPostRedisplay();
        }

    private:
        const float camera_speed = 0.05f;

        glm::vec3 camera_pos = glm::vec3({0.0f, 0.0f, 1.f});
        glm::vec3 camera_front = glm::vec3({0.0f, 0.0f, -1.0f});
        glm::vec3 camera_up = glm::vec3({0.0f, 1.0f, 0.0f});

        bool firstMouse = true;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float lastX = 512.0f;
        float lastY = 512.0f;
        float fov = 90.0f;
    };

    camera* camera::singleton_ = nullptr;

    camera* camera::get_instance()
    {
        if (singleton_ == nullptr)
            singleton_ = new camera();

        return singleton_;
    }
} // namespace mygl