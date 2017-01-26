#include "common.hpp"
#include "lloyd.hpp"
#include "yinyang.hpp"
#include "kmeans_plus_plus.hpp"
#include <iostream>


int main(int argc, const char* argv[]) {
	std::shared_ptr<KMeans_config> conf = KMeans_parse_arg(argc, argv);
	if (!conf)
		return 0;
	int N = conf->data_number;
	int K = conf->cluster_number;
	int D = conf->data_dimension;
	DataMat data(D, N);
	LabelVec label(N);
	ClusterVec cluster(N);
	CenterMat center(D, K);
	if (KMeans_get_data(conf, data, label)) {
		std::cerr << "error when get data" << std::endl;
		return 1;
	}

	if (conf->input_seed) {
		int tmp_K = -1, tmp_D = -1;
		std::cerr << "start load seed" << std::endl;
		if (KMeans_load_seed(conf->input_seed_file_name, tmp_K, tmp_D, center)) {
			std::cerr << "error when get seed" << std::endl;
			return 0;
		}
		if (K != tmp_K)
			std::cerr << "error: wrong data number in seed file, get " << tmp_K << " expect " << K << std::endl;
		if (D != tmp_D)
			std::cerr << "error: wrong data dimension in seed file, get" << tmp_D << " expect " << D << std::endl;
		if (K!= tmp_K || D != tmp_D)
			return 0;
	} else if (conf->kmeans_plus_plus_initialization) {
		if (kmeans_plus_plus_initialize(data, center)) {
			std::cerr << "error when kmeans plus plus initialization" << std::endl;
		}
	} else
		generate_random_initial_cluster(conf, data, center);
	if (conf->output_seed)
		KMeans_export_seed(conf->output_seed_file_name, center);

	if (conf->yinyang) {
		yinyang(data, cluster, center, conf->group_number, conf->norm_precision, conf->max_interation, conf->until_converge, conf->debug);
	} else {
		lloyd(data, cluster, center, conf->norm_precision, conf->max_interation, conf->until_converge); 
	}
	output_cluster(conf, cluster);

	return 0;
}

