//
// Created by Nobuyuki Umetani on 2023/05/15.
//

#ifndef PBA_UTIL_EIGEN_H_
#define PBA_UTIL_EIGEN_H_

#include <set>
#include <cassert>
#include <Eigen/Dense>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace pba {

auto generate_mesh_annulus3(
    float r_small,
    float r_large,
    int ndiv_radius,
    int ndiv_theta) {
  Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> tri2vtx(ndiv_radius * ndiv_theta * 2, 3);
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> vtx2xyz((ndiv_radius + 1) * ndiv_theta, 3);
  constexpr float pi = 3.1415926535;
  { // make coordinates
    float dr = (r_large - r_small) / static_cast<float>(ndiv_radius);
    float dth = 2.f * pi / static_cast<float>(ndiv_theta);
    for (int ir = 0; ir <= ndiv_radius; ir++) {
      for (int ith = 0; ith < ndiv_theta; ith++) {
        float rad = dr * static_cast<float>(ir) + r_small;
        float theta = static_cast<float>(ith +  (ir%2)*0.5 ) * dth;
        vtx2xyz.row(ir * ndiv_theta + ith) = Eigen::Vector3f(rad * cos(theta), 0.f, rad * sin(theta));
      }
    }
  }
  for (int ir = 0; ir < ndiv_radius; ir++) {
    for (int ith = 0; ith < ndiv_theta; ith++) {
      int i1 = (ir + 0) * ndiv_theta + (ith + 0) % ndiv_theta;
      int i2 = (ir + 0) * ndiv_theta + (ith + 1) % ndiv_theta;
      int i3 = (ir + 1) * ndiv_theta + (ith + 1) % ndiv_theta;
      int i4 = (ir + 1) * ndiv_theta + (ith + 0) % ndiv_theta;
      if (ir % 2 == 1) {
        tri2vtx.row((ir * ndiv_theta + ith) * 2 + 0) = Eigen::Vector3i(i3, i1, i2);
        tri2vtx.row((ir * ndiv_theta + ith) * 2 + 1) = Eigen::Vector3i(i4, i1, i3);
      }
      else {
        tri2vtx.row((ir * ndiv_theta + ith) * 2 + 0) = Eigen::Vector3i(i4, i2, i3);
        tri2vtx.row((ir * ndiv_theta + ith) * 2 + 1) = Eigen::Vector3i(i4, i1, i2);
      }
    }

  }
  return std::make_pair(tri2vtx, vtx2xyz);
}

auto vertex_to_elem(
    const Eigen::MatrixXi &elem2vtx,
    size_t num_vtx) {
  std::vector<unsigned int> vtx2idx, idx2elem;
  vtx2idx.assign(num_vtx + 1, 0);
  for (int i_elem = 0; i_elem < elem2vtx.rows(); i_elem++) {
    for (int i_node = 0; i_node < elem2vtx.cols(); i_node++) {
      const int i_vtx = elem2vtx(i_elem, i_node);
      vtx2idx[i_vtx + 1] += 1;
    }
  }
  for (unsigned int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    vtx2idx[i_vtx + 1] += vtx2idx[i_vtx];
  }
  unsigned int num_idx = vtx2idx[num_vtx];
  idx2elem.resize(num_idx);
  for (int i_elem = 0; i_elem < elem2vtx.rows(); i_elem++) {
    for (int i_node = 0; i_node < elem2vtx.cols(); i_node++) {
      const int i_vtx = elem2vtx(i_elem, i_node);
      const unsigned int ind1 = vtx2idx[i_vtx];
      idx2elem[ind1] = i_elem;
      vtx2idx[i_vtx] += 1;
    }
  }
  for (int ivtx = static_cast<int>(num_vtx); ivtx >= 1; --ivtx) {
    vtx2idx[ivtx] = vtx2idx[ivtx - 1];
  }
  vtx2idx[0] = 0;
  return std::make_pair(vtx2idx, idx2elem);
}


auto vertex_to_vertex(
    const Eigen::MatrixXi &elem2vtx,
    size_t num_vtx) {
  const auto[vtx2idx, idx2elem] = vertex_to_elem(elem2vtx, num_vtx);
  std::vector<unsigned int> vtx2jdx, jdx2vtx;
  vtx2jdx.push_back(0);
  for (unsigned int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    std::set<int> set_connected_points;
    for (unsigned int idx = vtx2idx[i_vtx]; idx < vtx2idx[i_vtx + 1]; ++idx) {
      const unsigned int i_elem = idx2elem[idx];
      for (int i_node = 0; i_node < elem2vtx.cols(); ++i_node) {
        const int j_vtx = elem2vtx(i_elem, i_node);
        set_connected_points.insert(j_vtx);
      }
    }
    vtx2jdx.push_back(vtx2jdx[vtx2jdx.size()-1]+set_connected_points.size());
    jdx2vtx.insert(jdx2vtx.end(), set_connected_points.begin(), set_connected_points.end());
  }
  assert(vtx2jdx.size() == num_vtx+1);
  assert(vtx2jdx[num_vtx] == jdx2vtx.size());
  return std::make_pair(vtx2jdx, jdx2vtx);
}

auto lines_of_mesh(
    const Eigen::MatrixXi &elem2vtx,
    int num_vtx) {
  const auto[vtx2idx, idx2elem] = vertex_to_elem(elem2vtx, num_vtx);
  std::vector<int> _line2vtx;
  for (int i_vtx = 0; i_vtx < num_vtx; ++i_vtx) {
    std::set<int> set_connected_points;
    for (unsigned int idx = vtx2idx[i_vtx]; idx < vtx2idx[i_vtx + 1]; ++idx) {
      const unsigned int ielem0 = idx2elem[idx];
      for (int inode = 0; inode < elem2vtx.cols(); ++inode) {
        const int j_vtx = elem2vtx(ielem0, inode);
        if (j_vtx <= i_vtx) continue;
        set_connected_points.insert(j_vtx);
      }
    }
    for (int j_vtx: set_connected_points) {
      _line2vtx.push_back(i_vtx);
      _line2vtx.push_back(j_vtx);
    }
  }
  auto map = Eigen::Map<Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajor> >(
      _line2vtx.data(), static_cast<int>(_line2vtx.size() / 2), 2);
  return Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajor>(map);
}

Eigen::Vector3f unit_normal_of_triangle(
    const Eigen::Vector3f& v1,
    const Eigen::Vector3f& v2,
    const Eigen::Vector3f& v3){
  return (v2-v1).cross(v3-v1).normalized();
}

auto load_wavefront_obj(
    const std::filesystem::path &file_path) {
  std::ifstream fin;
  fin.open(file_path);
  if (fin.fail()) {
    std::cout << "File Read Fail" << std::endl;
    exit(0);
  }
  std::vector<float> vtx2xyz;
  vtx2xyz.reserve(256 * 16);
  std::vector<int> tri2vtx;
  tri2vtx.reserve(256 * 16);
  const int BUFF_SIZE = 256;
  char buff[BUFF_SIZE];
  while (fin.getline(buff, BUFF_SIZE)) {
    if (buff[0] == '#') { continue; }
    if (buff[0] == 'v' && buff[1] == ' ') {
      char str[256];
      double x, y, z;
      {
        std::istringstream is(buff);
        is >> str >> x >> y >> z;
      }
      vtx2xyz.push_back(x);
      vtx2xyz.push_back(y);
      vtx2xyz.push_back(z);
    }
    if (buff[0] == 'f') {
      char str[256];
      int i0, i1, i2;
      {
        std::istringstream is(buff);
        is >> str >> i0 >> i1 >> i2;
      }
      tri2vtx.push_back(i0 - 1);
      tri2vtx.push_back(i1 - 1);
      tri2vtx.push_back(i2 - 1);
    }
  }
  const auto map_tri2vtx = Eigen::Map<Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> >(
      tri2vtx.data(), static_cast<int>(tri2vtx.size() / 3), 3);
  const auto map_vtx2xyz = Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> >(
      vtx2xyz.data(), static_cast<int>(vtx2xyz.size() / 3), 3);
  return std::make_pair(
      Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>(map_tri2vtx),
      Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>(map_vtx2xyz));
}


}

#endif //PBA_UTIL_EIGEN_H_
