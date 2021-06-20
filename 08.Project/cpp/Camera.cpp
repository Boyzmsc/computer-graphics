#include "Camera.h"

const glm::vec3 Camera::center_position() const
{
  return glm::vec3(position_[0] + front_dir_[0],
                   position_[1] + front_dir_[1],
                   position_[2] + front_dir_[2]);
}

const glm::mat4 Camera::get_view_matrix()
{
  return glm::lookAt(position_, center_position(), up_dir_);
}

// TODO: fill up the following functions properly
void Camera::move_forward(float delta)
{
  position_ += delta * front_dir_;
}

void Camera::move_backward(float delta)
{
  move_forward(-delta);
}

void Camera::move_left(float delta)
{
  position_ -= delta * right_dir_;
}

void Camera::move_right(float delta)
{
  move_left(-delta);
}

void Camera::move_up(float delta)
{
  position_ += delta * up_dir_;
}

void Camera::move_down(float delta)
{
  move_up(-delta);
}

// Camera Pitch
void Camera::rotate_x(float pitch)
{
  glm::mat4 cam_rot(glm::mat4(1.0f));

  cam_rot = glm::rotate(cam_rot, glm::radians(pitch), right_dir_);
  front_dir_ = glm::mat3(cam_rot) * front_dir_;

  up_dir_ = glm::normalize(glm::cross(right_dir_, front_dir_));
}

// Camera Yaw
void Camera::rotate_y(float yaw)
{
  glm::mat4 cam_rot(glm::mat4(1.0f));

  cam_rot = glm::rotate(cam_rot, glm::radians(yaw), up_dir_);
  right_dir_ = glm::mat3(cam_rot) * right_dir_;

  front_dir_ = glm::normalize(glm::cross(up_dir_, right_dir_));
}

// Camera Roll
void Camera::rotate_z(float roll)
{
  glm::mat4 cam_rot(glm::mat4(1.0f));

  cam_rot = glm::rotate(cam_rot, glm::radians(roll), front_dir_);
  up_dir_ = glm::mat3(cam_rot) * up_dir_;

  right_dir_ = glm::normalize(glm::cross(front_dir_, up_dir_));
}

void Camera::update_front_direction(glm::vec3 dir)
{
  front_dir_ = glm::normalize(dir);

  right_dir_ = glm::normalize(glm::cross(front_dir_, glm::vec3(0.0, 1.0f, 0.0f)));
  up_dir_ = glm::normalize(glm::cross(right_dir_, front_dir_));
}