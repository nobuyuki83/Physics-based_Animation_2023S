//
// Created by Nobuyuki Umetani on 2023/06/04.
//

#ifndef PBA_BLOCK_SPARSE_MATRIX_H_
#define PBA_BLOCK_SPARSE_MATRIX_H_

namespace pba {

template<int N>
class BlockSparseMatrix {
  using Vector = Eigen::Matrix<double, Eigen::Dynamic, N>;
  using BlockVector = Eigen::Vector<double, N>;
  using BlockMatrix = Eigen::Matrix<double, N, N>;
 public:
  void initialize(const Eigen::MatrixXi &elem2vtx, unsigned int num_vtx) {
    auto[vtx2idx, idx2vtx] = pba::vertex_to_vertex(elem2vtx, num_vtx);
    this->row2idx = vtx2idx;
    this->idx2col = idx2vtx;
    idx2block.resize(this->idx2col.size());
  }

  void setZero() {
    for (auto &block: idx2block) {
      block.setZero();
    }
  }

  auto &coeff(unsigned int i_row, unsigned int i_col) {
    auto itr0 = idx2col.begin() + row2idx[i_row];
    auto itr1 = idx2col.begin() + row2idx[i_row + 1];
    auto itr2 = std::find(itr0, itr1, i_col);
    assert(itr2 != itr1);
    unsigned int idx0 = std::distance(idx2col.begin(), itr2);
    return idx2block[idx0];
  }

  void set_is_free(
      const Vector &vtx2isfree) {
    unsigned int num_row = row2idx.size() - 1;
    for (unsigned int irow = 0; irow < num_row; ++irow) {
      for (unsigned int idx0 = row2idx[irow]; idx0 < row2idx[irow + 1]; ++idx0) {
        unsigned int icol = idx2col[idx0];
        BlockMatrix dia_row = vtx2isfree.row(irow).asDiagonal();
        BlockMatrix dia_col = vtx2isfree.row(icol).asDiagonal();
        idx2block[idx0] = dia_col * idx2block[idx0] * dia_row;
        if (icol == irow) {
          for (int i = 0; i < N; ++i) {
            idx2block[idx0](i, i) += 1.0 - vtx2isfree(irow, i);
          }
        }
      }
    }
  }
  Vector solve_conjugate_gradient(
      Vector &r) const {
    Vector x = Eigen::MatrixX3d::Zero(r.rows(), r.cols());
    Vector p = r;
    Vector Ap = p;
    const double r_squared_norm_ini = r.squaredNorm();
    double r_squared_norm_pre = r_squared_norm_ini;
    for (unsigned int itr = 0; itr < 10; ++itr) {
      this->multiply_vector(Ap, p);
      double alpha = r_squared_norm_pre / (p.cwiseProduct(Ap)).sum();
      x += alpha * p;
      r -= alpha * Ap;
      const double r_squared_norm_pos = r.squaredNorm();
      if (r_squared_norm_pos < r_squared_norm_ini * 1.0e-4) { return x; }
      double beta = r_squared_norm_pos / r_squared_norm_pre;
      r_squared_norm_pre = r_squared_norm_pos;
      p = r + beta * p;
    }
    return x;
  }
 private:
  void multiply_vector(Vector &y,
                       const Vector &x) const {
    unsigned int num_row = row2idx.size() - 1;
    y.setZero();
    for (unsigned int irow = 0; irow < num_row; ++irow) {
      for (unsigned int idx0 = row2idx[irow]; idx0 < row2idx[irow + 1]; ++idx0) {
        unsigned int icol = idx2col[idx0];
        BlockVector x0 = x.row(icol);
        y.row(irow) += idx2block[idx0] * x0;
      }
    }
  }
 public:
  std::vector<unsigned int> row2idx;
  std::vector<unsigned int> idx2col;
  std::vector<BlockMatrix> idx2block;
};

} // namespace pba

#endif //PBA_BLOCK_SPARSE_MATRIX_H_
