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

void set_some_lighting() {
  glEnable(GL_LIGHTING);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
  { // initialize light parameter
    glEnable(GL_LIGHT0);
    GLfloat light0_Kd[] = {0.9f, 0.3f, 0.3f, 1.0f};
    GLfloat light0_Pos[4] = {+0.5f, -0.5f, +1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_Kd);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_Pos);
  }
  {
    glEnable(GL_LIGHT1);
    GLfloat light1_Kd[] = {0.3f, 0.3f, 0.9f, 1.0f};
    GLfloat light1_Pos[4] = {-0.5f, +0.5f, +1.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_Kd);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_Pos);
  }
}

void colormap_hot(float v, float scale) {
  constexpr float map[6][3] = {
      {0, 0, 0}, // 0
      {0.5, 0, 0}, // 0.2
      {1, 0, 0}, // 0.4
      {1, 0.5, 0}, // 0.6
      {1, 1, 0}, // 0.8
      {1, 1, 1}, // 1.0
  };
  v *= scale;
  int ic = int(v * 6.f);
  float r = v*6.f - static_cast<float>(ic);
  if( ic >= 5 ){ ic = 4; r = 1.f; }
  if( ic < 0 ){ ic = 0; r = 0.f; }
  float color[3] = {
      (1 - r) * map[ic][0] + r * map[ic + 1][0],
      (1 - r) * map[ic][1] + r * map[ic + 1][1],
      (1 - r) * map[ic][2] + r * map[ic + 1][2] };
  ::glColor3fv(color);
}

void draw_sphere(
    int nla, int nlo) {
  if (nla <= 1 || nlo <= 2) { return; }
  const double pi = 3.1415926535;
  double dla = 2.0 * pi / nla;
  double dlo = pi / nlo;
  ::glBegin(GL_QUADS);
  for (int ila = 0; ila < nla; ila++) {
    for (int ilo = 0; ilo < nlo; ilo++) {
      double rla0 = (ila + 0) * dla;
      double rla1 = (ila + 1) * dla;
      double rlo0 = (ilo + 0) * dlo;
      double rlo1 = (ilo + 1) * dlo;
      double p0[3] = {cos(rla0) * cos(rlo0), cos(rla0) * sin(rlo0), sin(rla0)};
      double p1[3] = {cos(rla0) * cos(rlo1), cos(rla0) * sin(rlo1), sin(rla0)};
      double p2[3] = {cos(rla1) * cos(rlo1), cos(rla1) * sin(rlo1), sin(rla1)};
      double p3[3] = {cos(rla1) * cos(rlo0), cos(rla1) * sin(rlo0), sin(rla1)};
      ::glVertex3dv(p0);
      ::glVertex3dv(p1);
      ::glVertex3dv(p2);
      ::glVertex3dv(p3);
    }
  }
  ::glEnd();
}

void draw_sphere_at(
    int nla, int nlo, double rad,
    double x, double y, double z) {
  ::glMatrixMode(GL_MODELVIEW);
  ::glPushMatrix();
  ::glTranslated(+x, +y, +z);
  ::glScaled(rad, rad, rad);
  draw_sphere(nla, nlo);
  ::glPopMatrix();
}

} // namespace pbd

#endif //PBA_GL_UTIL_H_
