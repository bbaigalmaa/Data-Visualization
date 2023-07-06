//
//  Camera.hpp
//  OpenGLTest
//
//  Created by Baigalmaa on 2023.06.27.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "Camera.hpp"
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

class Camera{
private:
    void UpdateUV(float du, float dv); //updating spherical coordinates u and v
    float cam_speed; //traversal speed of camera
    
    glm::mat4 viewMatrix; // view matrix of camera
    glm::mat4 matViewprojection; //matrix view projection
    
    bool slow;
    
    glm::vec3 eye; //camera position
    glm::vec3 up; //vector pointing up direction
    glm::vec3 look_at; //camera look at point
    
    //spherical coordinate (u,v) - denoting current viewing direction from camera position (eye)
    float u;
    float v;
    
    //Distance of look at point from camera
    float dist;
    
    //Unit vector pointing towards the viewing direction
    glm::vec3 unitVector;
    glm::vec3 unitVectorRight;//unit vector pointing to the right
    
    glm::mat4 matProj;
    float goForward;
    float goRight;
    
    
public:
    Camera(void);
    Camera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
    ~Camera(void);
    
    glm::mat4 getViewMatrix();
    
    void Update(float _deltaTime);
    void setView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
    void setProj(float _angle, float _aspect, float _zn, float _zf);
    void lookAt(glm::vec3 _at);
    
    void setSpeed(float _value);
    
    glm::vec3 getEye(){
        return eye;
    }
    
    glm::vec3 getAt(){
        return look_at;
    }
    
    glm::vec3 getUp(){
        return up;
    }
    
    glm::mat4 getProj(){
        return matProj;
    }
    
    glm::mat4 getViewProj(){
        return matViewprojection;
    }
    
    void resize(int _w, int _h);
    
    void KeyboardDown(SDL_KeyboardEvent& key);
    void KeyboardUp(SDL_KeyboardEvent& key);
    void MouseMove(SDL_MouseMotionEvent& mouse);
};

#endif /* Camera_hpp */
