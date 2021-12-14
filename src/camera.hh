#pragma once

#include <GL/glut.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace mygl
{
    constexpr float CAMERA_SPEED = 0.1f;
    constexpr float CAMERA_SENSITIVITY = 0.1f;
    constexpr glm::vec3 CAMERA_DEFAULT_POS = glm::vec3({0.0f, 0.0f, 1.f});
    constexpr glm::vec3 CAMERA_DEFAULT_FRONT = glm::vec3({0.0f, 0.0f, -1.0f});
    constexpr glm::vec3 CAMERA_UP = glm::vec3({0.0f, 1.0f, 0.0f});

    class Camera
    {
    protected:
        Camera()
        {
        }

        static Camera* singleton_;

    public:
        static Camera* get_instance();

        glm::mat4 get_view_matrix() const
        {
            return glm::lookAt(camera_pos, camera_pos + camera_front, CAMERA_UP);
        }

        glm::mat4 get_projection_matrix() const
        {
            return glm::perspective(glm::radians(fov), 1920.f / 1080.f, 0.1f, 100.0f);
        }

        glm::vec3 get_position() const
        {
            return camera_pos;
        }

        glm::mat4 get_world_to_cam_matrix(bool turn_around = false) const
        {
            glm::mat4 projection = get_projection_matrix();
            glm::mat4 view = get_view_matrix();

            if (turn_around)
            {
                const float radius = 15.0f;
                const float time = 0.0005 * glutGet(GLUT_ELAPSED_TIME);
                float cam_x = sin(time) * radius;
                float cam_z = cos(time) * radius;
                view = glm::lookAt(glm::vec3(cam_x, 0.0, cam_z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            }

            return projection * view;
        }

        void key_callback(int key, int, int)
        {
            if (key == GLUT_KEY_UP)
                camera_pos = camera_pos + CAMERA_SPEED * camera_front;
            else if (key == GLUT_KEY_DOWN)
                camera_pos = camera_pos - CAMERA_SPEED * camera_front;
            else if (key == GLUT_KEY_LEFT)
                camera_pos = camera_pos - normalize(cross(camera_front, CAMERA_UP)) * CAMERA_SPEED;
            else if (key == GLUT_KEY_RIGHT)
                camera_pos = camera_pos + normalize(cross(camera_front, CAMERA_UP)) * CAMERA_SPEED;
        }

        void mouse_motion_callback(int x, int y)
        {
            float x_offset = (x - last_x) * CAMERA_SENSITIVITY;
            float y_offset = -(y - last_y) * CAMERA_SENSITIVITY;

            last_x = x;
            last_y = y;

            yaw += x_offset;
            pitch += y_offset;
            pitch = std::clamp(pitch, -90.0f, 90.0f);

            glm::vec3 direction = {cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch))};
            camera_front = normalize(direction);
        }

        void mouse_callback(int button, int state, int x, int y)
        {
            if (button == 3)
                fov -= 1;
            else if (button == 4)
                fov += 1;
            else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
            {
                last_x = x;
                last_y = y;
            }

            fov = std::clamp(fov, 1.0f, 90.0f);
        }

    private:
        glm::vec3 camera_pos = CAMERA_DEFAULT_POS;
        glm::vec3 camera_front = CAMERA_DEFAULT_FRONT;

        float yaw = -90.0f;
        float pitch = 0.0f;
        float last_x = 1920.f / 2.f;
        float last_y = 1080.f / 2.f;
        float fov = 90.0f;
    };

    Camera* Camera::singleton_ = nullptr;

    Camera* Camera::get_instance()
    {
        if (singleton_ == nullptr)
            singleton_ = new Camera();

        return singleton_;
    }
} // namespace mygl
