#ifndef KMEANS_COMMON_HPP
#define KMEANS_COMMON_HPP

#include <string>
#include <memory>
#include <Eigen/Dense>

struct KMEANS_config {
	std::string data_file_name;
	std::string output_file_name;

	std::string input_seed_file_name;
	std::string output_seed_file_name;
	bool input_seed;
	bool output_seed;

	int data_number;
	int data_dimension;
	int cluster_number;

	int max_interation;
	bool until_converge;
	float norm_precision;

	bool kmeans_plus_plus_initialization;
	bool yinyang;
	int group_number;
};

using DataMat = Eigen::MatrixXf;
using LabelVec = Eigen::VectorXf;
using CenterMat = Eigen::MatrixXf;
using ClusterVec = Eigen::VectorXi;
using PConf = std::shared_ptr<KMEANS_config>;

std::ostream& operator<<(std::ostream& os, const KMEANS_config& kc);
PConf KMEANS_parse_arg(int argc, const char *argv[]);
int KMEANS_get_data(PConf conf, DataMat &data, LabelVec &label);
int KMEANS_get_seed(PConf conf, ClusterVec &cluster);
void generate_libsvm_data_file(std::string file_name, PConf conf, DataMat &data, LabelVec &label);
int generate_random_initial_cluster(PConf conf, DataMat &data, CenterMat &center);
void output_cluster(PConf conf, ClusterVec &cluster);
double compute_loss(const DataMat &data, const ClusterVec &cluster, const CenterMat &center);

#endif
