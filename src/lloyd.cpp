#include "common.hpp"
#include <iostream>
#include <Eigen/Dense>
#include <memory>
#include <iostream>


bool lloyd_update_center(const DataMat &data, const ClusterVec &cluster, CenterMat &center, double precision, Eigen::MatrixXd &workspace1, Eigen::VectorXi &workspace2) {
	//std::cerr << "start lloyd update center" << std::endl;
	int N = data.cols();
	int K = center.cols();
	double l1 = compute_loss(data, cluster, center);
	bool changed = false;
	workspace1.setZero();
	workspace2.setZero();
	for (int n = 0; n < N; ++n) {
		int c = cluster(n);
		workspace2(c) += 1;
		workspace1.col(c) += data.col(n).cast<double>();
	}
	for (int k = 0; k < K; ++k)
		if (0 == workspace2(k)) {
			std::cerr << "zero data point in cluster" << std::endl;
			continue;
		} else {
			Eigen::VectorXf new_center = (workspace1.col(k) / workspace2(k)).cast<float>();
			if ((new_center - center.col(k)).norm() > precision)
				changed = true;
			center.col(k) = new_center;
		}
	double l2 = compute_loss(data, cluster, center);
	if (l2 - l1 > 1)
		std::cerr << "Loss increases in update center" << std::endl;
	//std::cerr << "finished lloyd update center" << std::endl;
	return changed;
}

bool lloyd_update_cluster(const DataMat &data, ClusterVec &cluster, const CenterMat &center) {
	//std::cerr << "start lloyd update cluster" << std::endl;
	double l1 = compute_loss(data, cluster, center);
	bool changed = false;
	int N = data.cols();
	int K = center.cols();
	for (int n = 0; n < N; ++n) {
		//std::cerr << "lloyd update cluster iteration " << n << std::endl;
		int mp = 0;
		float mv = (data.col(n) - center.col(0)).squaredNorm();
		for (int k = 1; k < K; ++k) {
			float nv = (data.col(n) - center.col(k)).squaredNorm();
			if (nv < mv) {
				mp = k;
				mv = nv;
			}
		}
		if (cluster(n) != mp)
			changed = true;
		double d1 = (data.col(n) - center.col(mp)).squaredNorm();
		double d2 = (data.col(n) - center.col(cluster(n))).squaredNorm();
		if (mp != cluster(n) && d1 > d2)
			std::cerr << "Loss increases in single update cluster " << mp << " " << cluster(n) << std::endl;
		cluster(n) = mp;
	}
	double l2 = compute_loss(data, cluster, center);
	if (l2 > l1)
		std::cerr << "Loss increases in update cluster" << std::endl;
	//std::cerr << "finished lloyd update cluster" << std::endl;
	return changed;
}

int lloyd(const DataMat &data, ClusterVec &cluster, CenterMat &center, double precision, int max_interation, bool until_converge) {
	std::cerr << "start lloyd iteration" << std::endl;
	int K = center.cols();
	int D = data.rows();
	if (D != center.rows()) {
		std::cerr << "error checking data dimension in lloyd" << std::endl;
		std::cerr << "dimension of input data " << D << std::endl;
		std::cerr << "dimension of center " << center.cols() << std::endl;
		return 1;
	}
	Eigen::MatrixXd workspace1(D, K);
	Eigen::VectorXi workspace2(K);
	double ll;
	double nl;
	cluster.setZero();
	lloyd_update_cluster(data, cluster, center);
	for (int i = 0; i < max_interation; ++i) {
		//std::cerr << "start iteration " << i << std::endl;
		bool changed1, changed2;
		changed1 = lloyd_update_center(data, cluster, center, precision, workspace1, workspace2);
		//std::cerr << "center changed " << changed1 << std::endl;
		changed2 = lloyd_update_cluster(data, cluster, center);
		//std::cerr << "cluster changed " << changed2 << std::endl;
		if (!changed1 && !changed2) {
			std::cerr << "converges at step " << i << std::endl;
			break;
		}
		if (until_converge)
			max_interation += 1;
		nl = compute_loss(data, cluster, center);
		if (0 != i && nl - ll > 1) {
			std::cerr << "loss increase in step " << i << std::endl;
		}
		std::cerr << "step " << i << " loss " << nl << std::endl;
		ll = nl;
	}

	return 0;
}

