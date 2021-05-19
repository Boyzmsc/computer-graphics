#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
  enum Mode { kOrtho, kPerspective };

public:
  Camera()
    : position_(0.0f, 0.0f, 0.5f), 
      front_dir_(0.0f, 0.0f,-1.0f), 
      up_dir_(0.0f, 1.0f, 0.0f), 
      right_dir_(1.0f, 0.0f, 0.0f),
      fovy_(45.0f),
      yaw_(0.0f),
      pitch_(0.0f),
      mode_(kOrtho) { }

  Camera(const glm::vec3& _position, const glm::vec3& _front_dir, const glm::vec3& _up_dir, float _fovy, float _yaw, float _pitch)
    : position_(_position), front_dir_(_front_dir), up_dir_(_up_dir), fovy_(_fovy), yaw_(_yaw), pitch_(_pitch)
  {
    right_dir_ = glm::cross(front_dir_, up_dir_);
  }
  
  void move_forward(float delta);
  void move_backward(float delta);
  void move_left(float delta);
  void move_right(float delta);
  void move_up(float delta);
  void move_down(float delta);
	
  const glm::vec3  position() const          { return  position_; }
  const glm::vec3  front_direction() const   { return  front_dir_; } 
  const glm::vec3  up_direction() const      { return  up_dir_; }
  const glm::vec3  right_direction() const   { return  right_dir_; }	
	const glm::vec3  center_position() const;
  const glm::mat4  get_view_matrix();

	const float				fovy() const							{ return fovy_; }
	void							set_fovy(float _fovy)			{ fovy_ = _fovy; }

  Camera::Mode      mode() const { return mode_; }
  void              set_mode(Camera::Mode _mode) { mode_ = _mode; }

  const float       yaw() const { return yaw_; }
  const float       pitch() const { return pitch_; }

  void              update_front_direction(glm::vec3 dir);

private:
	glm::vec3  position_;    // position of the camera  
  glm::vec3  front_dir_;   // front direction of the camera    (it should be a unit vector whose length is 1)
  glm::vec3  up_dir_;      // up direction of the camera       (it should be a unit vector whose length is 1)
  glm::vec3  right_dir_;   // right direction of the camera    (it should be a unit vector whose length is 1)

	float fovy_;

  Mode  mode_;

  float pitch_;
  float yaw_;
};