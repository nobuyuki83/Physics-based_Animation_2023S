//
// Created by Nobuyuki Umetani on 2023/05/16.
//

#ifndef PBA_FLOOR_DRAWER_H_
#define PBA_FLOOR_DRAWER_H_

namespace pba {

class FloorDrawer {
 public:
  FloorDrawer(double wfloor, double yfloor): wfloor(wfloor), yfloor(yfloor) {}

  // floor
  void draw_geometry() const {
    ::glDisable(GL_LIGHTING);
    ::glBegin(GL_QUADS);
    ::glNormal3d(0, 1, 0);
    ::glVertex3d(-wfloor, yfloor, -wfloor);
    ::glVertex3d(+wfloor, yfloor, -wfloor);
    ::glVertex3d(+wfloor, yfloor, +wfloor);
    ::glVertex3d(-wfloor, yfloor, +wfloor);
    ::glEnd();
  }
  static void ShadowMatrix(float m[16], const float plane[4], float lpos[3]) {
    float dot = plane[0] * lpos[0] + plane[1] * lpos[1] + plane[2] * lpos[2] + plane[3];
    for (int j = 0; j < 4; ++j) {
      for (int i = 0; i < 4; ++i) {
        m[j * 4 + i] = -plane[j] * lpos[i];
        if (i == j) { m[j * 4 + i] += dot; }
      }
    }
  }
  void draw_checkerboard() const {
    constexpr int ndiv = 25;
    const double len = wfloor * 2.0 / ndiv;
    ::glDisable(GL_LIGHTING);
    ::glBegin(GL_QUADS);
    ::glNormal3d(0, 1, 0);
    for (unsigned int idiv = 0; idiv < ndiv; ++idiv) {
      for (unsigned int jdiv = 0; jdiv < ndiv; ++jdiv) {
        if ((idiv + jdiv) % 2 == 0) {
          ::glColor4f(0.6f, 0.6f, 0.5f, 1.0f);
        } else {
          ::glColor4f(0.55f, 0.55f, 0.45f, 1.0f);
        }
        const double x0 = -wfloor + len * (idiv + 0);
        const double x1 = -wfloor + len * (idiv + 1);
        const double z0 = -wfloor + len * (jdiv + 0);
        const double z1 = -wfloor + len * (jdiv + 1);
        ::glVertex3d(x0, yfloor, z0);
        ::glVertex3d(x1, yfloor, z0);
        ::glVertex3d(x1, yfloor, z1);
        ::glVertex3d(x0, yfloor, z1);
      }
    }
    ::glEnd();
  }
  void draw_shadow_pre() {
    this->is_lighting = ::glIsEnabled(GL_LIGHTING);
    this->is_blend = ::glIsEnabled(GL_BLEND);
    // draw floor
    ::glClearStencil(0);
    // draw floor (stencil 1)
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, static_cast<GLuint>(~0));
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    this->draw_checkerboard();
    // draw stensil
    glColorMask(0, 0, 0, 0);
    glDepthMask(0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1, static_cast<GLuint>(~0));
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glPushMatrix();
    {
      float plane[4] = {0, 1, 0, -float(this->yfloor) - 0.001f};
      float lpos[4] = {0, 100, 0, 1};
      float m_shadow[16];
      ShadowMatrix(m_shadow, plane, lpos);
      glMultMatrixf(m_shadow);
    }
  }
  void draw_shadow_pos() {
    glPopMatrix();
    glColorMask(1, 1, 1, 1);
    glDepthMask(1);
    // draw shadow
    glStencilFunc(GL_EQUAL, 2, static_cast<GLuint>(~0));
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ::glDisable(GL_DEPTH_TEST);
    ::glDisable(GL_LIGHTING);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    this->draw_geometry();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    if (is_lighting) { ::glEnable(GL_LIGHTING); }
    if (is_blend) { ::glEnable(GL_BLEND); }
  }
 public:
  double wfloor, yfloor;
  GLboolean is_lighting = false;
  GLboolean is_blend =  false;
};

}

#endif //PBA_FLOOR_DRAWER_H_
