//
// Created by Nobuyuki Umetani on 2023/04/21.
//

#ifndef PBA_GL_UTIL_H_
#define PBA_GL_UTIL_H_

#include <cmath>

#if defined(_WIN32) // windows
#define NOMINMAX   // to remove min,max macro
#include <windows.h>
#endif

#if defined(__APPLE__) && defined(__MACH__) // mac
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace pba {

void draw_circle_solid(
    float x,
    float y,
    float rad = 1.f,
    unsigned int ndiv = 32) {
  constexpr float PI2 = 3.14159265358979323846264338327950288419716939 * 2;
  float r = PI2 / static_cast<float>(ndiv);
  ::glBegin(GL_TRIANGLES);
  for (unsigned int idiv = 0; idiv < ndiv; ++idiv) {
    const unsigned int jdiv = (idiv + 1) % ndiv;
    const auto idivf = static_cast<float>(idiv);
    const auto jdivf = static_cast<float>(jdiv);
    ::glVertex2f(x, y);
    ::glVertex2f(x + rad * std::cos(idivf * r), y + rad * std::sin(idivf * r));
    ::glVertex2f(x + rad * std::cos(jdivf * r), y + rad * std::sin(jdivf * r));
  }
  ::glEnd();
}

void draw_circle_wireframe(
    float x,
    float y,
    float rad = 1.f,
    unsigned int ndiv = 32) {
  constexpr float PI2 = 3.14159265358979323846264338327950288419716939 * 2;
  float r = PI2 / static_cast<float>(ndiv);
  ::glBegin(GL_LINE_LOOP);
  for (unsigned int idiv = 0; idiv < ndiv; ++idiv) {
    const auto idivf = static_cast<float>(idiv);
    ::glVertex2f(x + rad * std::cos(idivf * r), y + rad * std::sin(idivf * r));
  }
  ::glEnd();
}

void draw_box_wireframe(
  float cx,
  float cy,
  float w,
  float h) {
  ::glBegin(GL_LINE_LOOP);
  ::glVertex2f(cx-w*0.5f, cy-h*0.5f);
  ::glVertex2f(cx+w*0.5f, cy-h*0.5f);
  ::glVertex2f(cx+w*0.5f, cy+h*0.5f);
  ::glVertex2f(cx-w*0.5f, cy+h*0.5f);
  ::glEnd();
}

void draw_polyline(const std::vector<Eigen::Vector2f> &ps) {
  ::glBegin(GL_LINE_STRIP);
  for (auto p: ps) {
    ::glVertex2f(p.x(), p.y());
  }
  ::glEnd();
}


} // namespace pbd

#endif //PBA_GL_UTIL_H_
