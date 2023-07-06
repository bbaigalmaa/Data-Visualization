//
//  Camera.cpp
//  OpenGLTest
//
//  Created by Baigalmaa on 2023.06.27.
//

#include "Camera.hpp"
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(void) : eye(0.0f, 0.0f, -5.0f), look_at(0.0f), up(0.0f, 1.0f, 0.0f), cam_speed(16.0f), goForward(0), goRight(0), slow(false)
{
    setView(glm::vec3(0, 0, -5), glm::vec3(0,0,0), glm::vec3(0,1,0));
    dist = glm::length(look_at - eye);
    setProj(glm::radians(45.0f), 640/480.0f, 0.01f, 1000.0f);
    
}

Camera::Camera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up) : cam_speed(16.0f), goForward(0), goRight(0), slow(false)
{
    setView(_eye, _at, _up);
}

Camera::~Camera(void){
    
}

void Camera::setView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up){
    eye = _eye;
    look_at = _at;
    up = _up;
    
    unitVector = glm::normalize(look_at - eye);
    unitVectorRight = glm::normalize(glm::cross(unitVector, up));
    
    dist = glm::length(look_at - eye);
    
    u = atan2f(unitVector.z, unitVector.x);
    v = acosf(unitVector.y);
    
}

void Camera::setProj(float _angle, float _aspect, float _zn, float _zf){
    matProj = glm::perspective(_angle, _aspect, _zn, _zf);
    matViewprojection = matProj * viewMatrix;
}

glm::mat4 Camera::getViewMatrix(){
    return viewMatrix;
}

void Camera::Update(float _deltatime){
    eye += (goForward*unitVector + goRight*unitVectorRight)*cam_speed*_deltatime;
    look_at += (goForward*unitVector + goRight*unitVectorRight)*cam_speed*_deltatime;
    
    viewMatrix = glm::lookAt(eye, look_at, up);
    matViewprojection = matProj * viewMatrix;
}

void Camera::UpdateUV(float du, float dv){
    u += du;
    v = glm::clamp(v + dv, 0.1f, 3.1f);
    
    look_at = eye + dist * glm::vec3(cosf(u)*sinf(v), cosf(v), sinf(u)*sinf(v));
    
    unitVector = glm::normalize(look_at - eye);
    unitVectorRight = glm::normalize(glm::cross(unitVector, up));
    
}

void Camera::setSpeed(float _value){
    cam_speed = _value;
}

void Camera::resize(int _w, int _h){
    setProj(glm::radians(60.0f), _w/(float)_h, 0.1f, 1000.0f);
}

void Camera::KeyboardDown(SDL_KeyboardEvent &key){
    switch (key.keysym.sym) {
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if(!slow){
                slow = true;
                cam_speed /= 2.0f;
            }
            break;
        case SDLK_w:
            goForward = 0.1f;
            break;
        case SDLK_s:
            goForward = -0.1f;
            break;
            
        case SDLK_a:
            goRight = -0.1f;
            break;
        case SDLK_d:
            goRight = 0.1f;
            break;
    }
}

void Camera::KeyboardUp(SDL_KeyboardEvent &key){
    float current_speed = cam_speed;
    switch (key.keysym.sym) {
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if(slow){
                slow = false;
                cam_speed *= 2.0f;
            }
            break;
        case SDLK_w:
        case SDLK_s:
            goForward = 0;
            break;
            
        case SDLK_a:
        case SDLK_d:
            goRight = 0;
            break;
    }
}

void Camera::MouseMove(SDL_MouseMotionEvent &mouse){
    if(mouse.state & SDL_BUTTON_LMASK){
        UpdateUV(mouse.xrel/100.0f,mouse.yrel/100.0f);
    }
}

void Camera::lookAt(glm::vec3 _at){
    setView(eye, _at, up);
}
