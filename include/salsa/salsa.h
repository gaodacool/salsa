#pragma once

#include <memory>

#include <Eigen/Core>
#include "geometry/xform.h"
#include "multirotor_sim/estimator_base.h"
#include "multirotor_sim/satellite.h"
#include "multirotor_sim/wsg84.h"

#include "factors/imu.h"
#include "factors/mocap.h"
#include "factors/xform.h"

#include "salsa/logger.h"

using namespace std;
using namespace Eigen;
using namespace xform;

#ifndef SALSA_WINDOW_SIZE
#define SALSA_WINDOW_SIZE 10
#endif

#ifndef SALSA_NUM_FEATURES
#define SALSA_NUM_FEATURES 120
#endif

class MTLogger;
class Logger;
class Salsa : public multirotor_sim::EstimatorBase
{
public:

  enum
  {
    N = SALSA_WINDOW_SIZE,
    M = SALSA_NUM_FEATURES,
  };


  Salsa(std::string filename = "/tmp/Salsa");

  void initState();
  void initialize(const double& t, const Xformd &x0, const Vector3d& v0, const Vector2d& tau0);
  void initSolverOptions();
  void initLog(std::string& filename);

  void finishNode(const double& t);

  void solve();
  void addResidualBlocks(ceres::Problem& problem);
  void addImuFactors(ceres::Problem& problem);
  void addMocapFactors(ceres::Problem& problem);

  void imuCallback(const double &t, const Vector6d &z, const Matrix6d &R) override;
  void mocapCallback(const double &t, const Xformd &z, const Matrix6d &R) override;

  double current_t_;
  Xformd current_x_;
  Vector3d current_v_;

  bool initialized_[N];
  Matrix<double, N, 1> t_;
  Matrix<double, 7, N> x_; int x_idx_;
  Matrix<double, 3, N> v_;
  Matrix<double, 2, N> tau_;
  Vector6d imu_bias_;
  double dt_mocap_;
  int current_node_;

  ImuFunctor imu_[N]; int imu_idx_;

  MocapFunctor mocap_[N]; int mocap_idx_;

  ceres::Solver::Options options_;
  ceres::Solver::Summary summary_;

  Logger* state_log_ = nullptr;
  Logger* opt_log_ = nullptr;
};
