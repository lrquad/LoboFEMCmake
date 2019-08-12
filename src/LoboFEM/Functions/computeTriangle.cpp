#include "computeTriangle.h"

void computeTriangleNorm(Eigen::Vector3d &v1, Eigen::Vector3d &v2, Eigen::Vector3d &v3, Eigen::Vector3d &normal)
{
	normal = (v2 - v1).cross(v3 - v1);
	normal.normalize();
}

double computeTriangleArea(Eigen::Vector3d n0, Eigen::Vector3d n1, Eigen::Vector3d n2)
{
	n1 = n1 - n0;
	n2 = n2 - n0;

	n0 = n1.cross(n2);

	double area = 0.5 * n0.norm();
	return area;
}

double computeDistancePointToTriangle(Eigen::Vector3d &v1, Eigen::Vector3d &v2, Eigen::Vector3d &v3, Eigen::Vector3d &normal, Eigen::Vector3d &p)
{
	Eigen::Vector3d pv1 = (p - v1);
	return (p - v1).norm()*pv1.dot(normal) / (pv1.norm());
}

bool checkPointTriangleProjection(Eigen::Vector3d &v1, Eigen::Vector3d &v2, Eigen::Vector3d &v3,  const Eigen::Vector3d &p)
{
	Eigen::Vector3d u = v2 - v1;
	Eigen::Vector3d v = v3 - v1;
	Eigen::Vector3d n = u.cross(v);
	Eigen::Vector3d w = p - v1;

	double gamma = (u.cross(w)).dot(n) / n.dot(n);
	double beta = w.cross(v).dot(n) / n.dot(n);
	double alpha = 1 - gamma - beta;

	return ((0 <= alpha) && (alpha <= 1) &&
		(0 <= beta) && (beta <= 1) &&
		(0 <= gamma) && (gamma <= 1));
}

