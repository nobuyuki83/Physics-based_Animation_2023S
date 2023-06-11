/**
 * @file task04: Accelerated Nearest Search using Kd-Tree
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#include <climits>
#include <cassert>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "../src/pba_util_glfw.h"
#include "../src/pba_util_gl.h"

class Node {
 public:
  Eigen::Vector2f pos;
  unsigned int idx_node_left = UINT_MAX;
  unsigned int idx_node_right = UINT_MAX;
};

/**
 * construct Kd-tree recursively
 * @param [in,out] nodes
 * @param [in] idx_node
 * @param [in,out] points
 * @param [in] idx_point_begin
 * @param [in] idx_point_end
 * @param [in] i_depth
 */
void construct_kdtree(
    std::vector<Node> &nodes,
    unsigned int idx_node,
    std::vector<Eigen::Vector2f> &points,
    unsigned int idx_point_begin,
    unsigned int idx_point_end,
    int i_depth) {

  if (idx_point_end - idx_point_begin == 1) { // leaf node
    nodes[idx_node].pos = points[idx_point_begin];
    return;
  }

  if (i_depth % 2 == 0) { // sort by x-coordinate
    std::sort(points.begin() + idx_point_begin, points.begin() + idx_point_end,
              [](const Eigen::Vector2f &lhs, const Eigen::Vector2f &rhs) { return lhs.x() < rhs.x(); });
  } else { // sort by y-coordinate
    std::sort(points.begin() + idx_point_begin, points.begin() + idx_point_end,
              [](const Eigen::Vector2f &lhs, const Eigen::Vector2f &rhs) { return lhs.y() < rhs.y(); });
  }

  unsigned int idx_point_mid = (idx_point_end - idx_point_begin) / 2 + idx_point_begin; // median point
  nodes[idx_node].pos = points[idx_point_mid];

  if (idx_point_begin != idx_point_mid) { // there is at least one point smaller than median
    unsigned int idx_node_left = nodes.size();
    nodes.resize(nodes.size() + 1);
    nodes[idx_node].idx_node_left = idx_node_left;
    construct_kdtree(nodes, idx_node_left, points, idx_point_begin, idx_point_mid, i_depth + 1);
  }
  if (idx_point_mid + 1 != idx_point_end) { // there is at least one point larger than median
    unsigned int idx_node_right = nodes.size();
    nodes.resize(nodes.size() + 1);
    nodes[idx_node].idx_node_right = idx_node_right;
    construct_kdtree(nodes, idx_node_right, points, idx_point_mid + 1, idx_point_end, i_depth + 1);
  }
}

/**
 * signed distance from axis-aligned bounding box
 * @param [in] pos_in where the signed distance is evaluated
 * @param [in] x_min bounding box's x-coordinate minimum
 * @param [in] x_max bounding box's x-coordinate maximum
 * @param [in] y_min bounding box's y-coordinate minimum
 * @param [in] y_max bounding box's y-coordinate maximum
 * @return signed distance (inside is negative)
 */
float signed_distance_aabb(
    const Eigen::Vector2f &pos_in,
    float x_min, float x_max,
    float y_min, float y_max) {
  const float x_center = (x_max + x_min) * 0.5f;
  const float y_center = (y_max + y_min) * 0.5f;
  float x_dist = std::abs(pos_in.x() - x_center) - (x_max - x_min) * 0.5f;
  float y_dist = std::abs(pos_in.y() - y_center) - (y_max - y_min) * 0.5f;
  return std::max(x_dist, y_dist);
}

/**
 * compute nearest position efficiently using kd-tree
 * @param [in,out] pos_near the current best nearest position
 * @param [in] pos_in the input position
 * @param nodes array of nodes
 * @param idx_node the index of current node
 * @param x_min bounding box's x-coordinate minimum
 * @param x_max bounding box's x-coordinate maximum
 * @param y_min bounding box's y-coordinate minimum
 * @param y_max bounding box's y-coordinate maximum
 * @param i_depth current depth
 */
void nearest_kdtree(
    Eigen::Vector2f &pos_near,
    const Eigen::Vector2f &pos_in,
    const std::vector<Node> &nodes,
    unsigned int idx_node,
    float x_min, float x_max,
    float y_min, float y_max,
    int i_depth) {
  if (idx_node >= nodes.size()) { return; } // this node does not exist

  // Write some codes below to accelerate the computation to find minimum
  // Currently, the computation is brute force i.e., computing distance against all the points.
  // Cull the tree branch whose nodes will not be the minimum distance points.
  // Use the "signed_distance_aabb" function above.

  const Eigen::Vector2f pos = nodes[idx_node].pos;
  if ((pos - pos_in).norm() < (pos_near - pos_in).norm()) { pos_near = pos; } // update the nearest position

  if (i_depth % 2 == 0) { // division in x direction
    nearest_kdtree(pos_near, pos_in, nodes, nodes[idx_node].idx_node_left, x_min, pos.x(), y_min, y_max, i_depth + 1);
    nearest_kdtree(pos_near, pos_in, nodes, nodes[idx_node].idx_node_right, pos.x(), x_max, y_min, y_max, i_depth + 1);
  } else { // division in y-direction
    nearest_kdtree(pos_near, pos_in, nodes, nodes[idx_node].idx_node_left, x_min, x_max, y_min, pos.y(), i_depth + 1);
    nearest_kdtree(pos_near, pos_in, nodes, nodes[idx_node].idx_node_right, x_min, x_max, pos.y(), y_max, i_depth + 1);
  }
}

/**
 * Draw kd-tree
 * @param [in] nodes array of nodes
 * @param idx_node index of current node
 * @param x_min bounding box's x minimum
 * @param x_max bounding box's x maximum
 * @param y_min bounding box's y minimum
 * @param y_max bounding box's y maximum
 * @param i_depth depth of current node
 */
void draw_kdtree(
    const std::vector<Node> &nodes,
    unsigned int idx_node,
    float x_min, float x_max,
    float y_min, float y_max,
    int i_depth) {
  if (idx_node >= nodes.size()) { return; }
  const auto pos = nodes[idx_node].pos;
  if (i_depth % 2 == 0) {
    ::glColor3f(0.0f, 1.0f, 0.0f);
    ::glBegin(GL_LINES);
    ::glVertex2f(pos.x(), y_min);
    ::glVertex2f(pos.x(), y_max);
    ::glEnd();
    draw_kdtree(nodes, nodes[idx_node].idx_node_left, x_min, pos.x(), y_min, y_max, i_depth + 1);
    draw_kdtree(nodes, nodes[idx_node].idx_node_right, pos.x(), x_max, y_min, y_max, i_depth + 1);
  } else {
    ::glColor3f(0.0f, 1.0f, 0.0f);
    ::glBegin(GL_LINES);
    ::glVertex2f(x_min, pos.y());
    ::glVertex2f(x_max, pos.y());
    ::glEnd();
    draw_kdtree(nodes, nodes[idx_node].idx_node_left, x_min, x_max, y_min, pos.y(), i_depth + 1);
    draw_kdtree(nodes, nodes[idx_node].idx_node_right, x_min, x_max, pos.y(), y_max, i_depth + 1);
  }
  ::glColor3f(1.0f, 0.0f, 1.0f);
  ::glPointSize(5);
  ::glBegin(GL_POINTS);
  ::glVertex2fv(pos.data());
  ::glEnd();
}

int main() {

  GLFWwindow *window = pba::window_initialization("task04: Accelerated Nearest Search using Kd-Tree");

  constexpr float box_size = 1.8;

  std::vector<Node> nodes;
  { // constructing Kd-tree's node
    std::vector<Eigen::Vector2f> particles(5000); // set number of particles
    for (auto &p: particles) { // // set coordinates
      p = Eigen::Vector2f::Random() * box_size * 0.5f;
    }
    nodes.reserve(particles.size());
    nodes.resize(1);
    construct_kdtree(nodes, 0,
                     particles, 0, particles.size(),
                     0);
  }

  std::chrono::system_clock::time_point start = std::chrono::system_clock::now(); // record starting time

  unsigned int num_div = 256; // grid resolution
  std::vector<float> grid2dist((num_div + 1) * (num_div + 1)); // grid data array storing distances
  for (unsigned int iy = 0; iy < num_div + 1; ++iy) {
    for (unsigned int ix = 0; ix < num_div + 1; ++ix) {
      const float h = box_size / static_cast<float>(num_div);
      float x = static_cast<float>(ix) * h - box_size * 0.5f; // grid point's x-coordinate
      float y = static_cast<float>(iy) * h - box_size * 0.5f; // grid point's y-coordinate
      Eigen::Vector2f pos_near(100.f, 100.f); // initial random guess of the nearest point
      nearest_kdtree( // find the nearest point's coordinates from {x,y}
          pos_near,
          {x, y},
          nodes, 0,
          -box_size * 0.5f, +box_size * 0.5f,
          -box_size * 0.5f, +box_size * 0.5f,
          0);
      grid2dist[iy*(num_div+1)+ix] = (Eigen::Vector2f(x, y) - pos_near).norm(); // putting distance to the array
    }
  }
  std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "total computation time: " << elapsed << "ms" << std::endl;

  while (!::glfwWindowShouldClose(window)) {
    pba::default_window_2d(window);

    { // draw distance field using colour map
      const float h = box_size / static_cast<float>(num_div);
      const float scale = sqrt(static_cast<float>(nodes.size())) * 0.2f;
      ::glMatrixMode(GL_MODELVIEW);
      ::glPushMatrix();
      ::glTranslatef( - box_size * 0.5f,  - box_size * 0.5f, 0.f);
      ::glScalef(h, h, h);
      ::glColor3f(0.9f, 0.9f, 0.9f);
      ::glBegin(GL_QUADS);
      for (unsigned int iy = 0; iy < num_div; ++iy) {
        for (unsigned int ix = 0; ix < num_div; ++ix) {
          pba::colormap_hot(grid2dist[(iy+0)*(num_div+1)+(ix+0)], scale);
          glVertex2f(ix + 0, iy + 0);
          pba::colormap_hot(grid2dist[(iy+0)*(num_div+1)+(ix+1)], scale);
          glVertex2f(ix + 1, iy + 0);
          pba::colormap_hot(grid2dist[(iy+1)*(num_div+1)+(ix+1)], scale);
          glVertex2f(ix + 1, iy + 1);
          pba::colormap_hot(grid2dist[(iy+1)*(num_div+1)+(ix+0)], scale);
          glVertex2f(ix + 0, iy + 1);
        }
      }
      ::glEnd();
      ::glPopMatrix();
    }

    ::glColor3f(0.0f, 0.0f, 0.0f);
    pba::draw_box_wireframe(0.f, 0.f, box_size, box_size);

    draw_kdtree(nodes, 0,
                -box_size * 0.5f, +box_size * 0.5f,
                -box_size * 0.5f, +box_size * 0.5f,
                0);

    // finalize drawing by swapping buffer
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
