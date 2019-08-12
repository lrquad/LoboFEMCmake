#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

void computeTriangleNorm(Eigen::Vector3d &v1, Eigen::Vector3d &v2, Eigen::Vector3d &v3, Eigen::Vector3d &normal);

double computeTriangleArea(Eigen::Vector3d n0, Eigen::Vector3d n1, Eigen::Vector3d n2);

double computeDistancePointToTriangle(Eigen::Vector3d &v1, Eigen::Vector3d &v2, Eigen::Vector3d &v3, Eigen::Vector3d &normal, Eigen::Vector3d &p);

bool checkPointTriangleProjection(Eigen::Vector3d &v1, Eigen::Vector3d &v2, Eigen::Vector3d &v3, const Eigen::Vector3d &p);
