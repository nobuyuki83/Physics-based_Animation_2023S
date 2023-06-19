//
// Created by Nobuyuki Umetani on 2023/05/16.
//

#ifndef PBA_EIGEN_GL_H_
#define PBA_EIGEN_GL_H_

#include "pba_util_eigen.h"

namespace pba {

void draw_triangle_3d_mesh(
    const Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> &tri2vtx,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz) {
  ::glBegin(GL_TRIANGLES);
  for (unsigned int itri = 0; itri < tri2vtx.size() / 3; ++itri) {
    int i0 = tri2vtx(itri, 0);
    int i1 = tri2vtx(itri, 1);
    int i2 = tri2vtx(itri, 2);
    auto n = pba::unit_normal_of_triangle(vtx2xyz.row(i0), vtx2xyz.row(i1), vtx2xyz.row(i2));
    ::glNormal3fv(n.data());
    ::glVertex3fv(vtx2xyz.row(i0).data());
    ::glVertex3fv(vtx2xyz.row(i1).data());
    ::glVertex3fv(vtx2xyz.row(i2).data());
  }
  ::glEnd();
}

void draw_line_3d_mesh(
    const Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajor> &line2vtx,
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> &vtx2xyz) {
  ::glBegin(GL_LINES);
  for (unsigned int il = 0; il < line2vtx.rows(); ++il) {
    int i0 = line2vtx(il, 0);
    int i1 = line2vtx(il, 1);
    ::glVertex3fv(vtx2xyz.row(i0).data());
    ::glVertex3fv(vtx2xyz.row(i1).data());
  }
  ::glEnd();
}

}

#endif //PBA_EIGEN_GL_H_
