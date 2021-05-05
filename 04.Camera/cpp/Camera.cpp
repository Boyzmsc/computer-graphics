#include "Camera.h"

const glm::vec3 Camera::center_position() const
{
  return glm::vec3(position_[0] + front_dir_[0],
                   position_[1] + front_dir_[1],
                   position_[2] + front_dir_[2]);
}

// TODO: fill up the following functions properly
const glm::mat4 Camera::get_view_matrix()
{
  return glm::lookAt(position_, center_position(), up_dir_);
}

void Camera::move_forward(float delta)
{
  position_ += delta * front_dir_;
}

void Camera::move_backward(float delta)
{
  position_ -= delta * front_dir_;
}

void Camera::move_left(float delta)
{
  position_ -= delta * right_dir_;
}

void Camera::move_right(float delta)
{
  position_ += delta * right_dir_;
}

void Camera::move_up(float delta)
{
  position_ += delta * up_dir_;
}

void Camera::move_down(float delta)
{
  position_ -= delta * up_dir_;
}

void Camera::update_front_direction(glm::vec3 dir)
{
  front_dir_ = dir;
}