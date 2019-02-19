#include <vector>
#include <algorithm>
#include <thread>
#include <time.h>
#include "myfunction\create_delete_largeSize_array.h"
#include "myfunction\time_spend.h"
#include "myfunction\file_to_array.h"

#define DATA_NUM 1000000000
#define THREAD_NUM 10
#define TABLE_NUM 16
#define CLUSTER_NUM 256
#define DATA_TABLE_DIM 8
#define QUERY_NUM 10000
#define TOP_RECALL 1
//#define CANDIDATE_SIZE 0

//string methodNumber = "M8"; 
string adcType = "16_";
string candidateSize = "10K";
string candidatepickMethod = "_new_10_";

class data_dist {
public:
	unsigned __int32 ID;
	double Distance;
	data_dist(unsigned __int32 id = 0, double d = 0) {
		ID = id;
		Distance = d;
	}
	bool operator < (const data_dist &m)const {
		return Distance < m.Distance; //小到大
	}
};

/*bool Compare_dist(const data_dist &a, const data_dist &b)
{
	return a.Distance < b.Distance;
}*/

char *g_input_folder = "..\\..\\Input\\ADC\\JIM\\";//10.16.173.110/Public/output_1B_for_65536_class/"; // "..//..//input//";//smae folder for this project
char *g_output_folder = "..\\..\\Output\\";
string g_file_name = "";

unsigned __int8 **g_data_cluster = NEW2D(TABLE_NUM, DATA_NUM, unsigned __int8);
double ***g_query = NEW3D(TABLE_NUM, QUERY_NUM, DATA_TABLE_DIM, double);

double ***g_codebook = NEW3D(TABLE_NUM, CLUSTER_NUM, DATA_TABLE_DIM, double);

int **g_query_ground_truth = NEW2D(QUERY_NUM, TOP_RECALL, int);

void LoadCodebook(FILE *input_file, double **codebook, int data_num) {
	FileToArray2D_double(input_file, codebook, data_num, DATA_TABLE_DIM);
}

void LoadDataCluster(FILE *input_file, unsigned __int8 *data_cluster) {
	/*char *saveptr = NULL;
	char *buffer = new char[INT_MAX];
	int num;
	for (int i = 0; i < (CLUSTER_NUM); i++) {
		fgets(buffer, INT_MAX, input_file);
		saveptr = buffer;
		while (CharToIntOnlyAutoStop(&saveptr, &num)) {
			data_cluster[num] = i;
		}
	}
	delete buffer;
	buffer = NULL;*/
	FileToArray1D_unint8(input_file, data_cluster, DATA_NUM);
}

void LoadFile() {
	FILE *input_file_query[TABLE_NUM];
	FILE *input_file_codebook[TABLE_NUM];
	FILE *input_file_data_clulste[TABLE_NUM];

	thread load_data_clulste[TABLE_NUM];
	thread load_codebook[TABLE_NUM];
	thread load_query[TABLE_NUM];

	for (int table_id = 0; table_id < TABLE_NUM; table_id++) {

		g_file_name = "SIFT_Query_table" + to_string(table_id) + ".txt";
		CheckOpenFile(input_file_query[table_id], g_input_folder, g_file_name, "rb");
		load_query[table_id] = thread(LoadCodebook, input_file_query[table_id], g_query[table_id], QUERY_NUM);

		g_file_name = "16_codebook_" + to_string(table_id) + ".txt"; //每張表分256群後的codebook
		CheckOpenFile(input_file_codebook[table_id], g_input_folder, g_file_name, "rb");
		load_codebook[table_id] = thread(LoadCodebook, input_file_codebook[table_id], g_codebook[table_id], CLUSTER_NUM);

		g_file_name = "16_clustered_" + to_string(table_id) + ".txt"; //每張表中每一群的資料ID
		CheckOpenFile(input_file_data_clulste[table_id], g_input_folder, g_file_name, "rb");
		load_data_clulste[table_id] = thread(LoadDataCluster, input_file_data_clulste[table_id], g_data_cluster[table_id]);

	}
	for (int table_id = 0; table_id < TABLE_NUM; table_id++) {
		load_codebook[table_id].join();
		load_data_clulste[table_id].join();
		load_query[table_id].join();
		fclose(input_file_codebook[table_id]);
		fclose(input_file_data_clulste[table_id]);
	}
	FILE *input_file_ground_truth;
	g_file_name = "Gnd.txt";
	CheckOpenFile(input_file_ground_truth, g_input_folder, g_file_name, "rb");
	FileToArray2D_int(input_file_ground_truth, g_query_ground_truth, QUERY_NUM, TOP_RECALL);
	fclose(input_file_ground_truth);

	cout << "File Loaded\n";

}

inline double Distance(double data[], double codebook[]) {
	double dist = 0;
	double res;
	for (int j = 0; j < DATA_TABLE_DIM; j++) {
		res = data[j] - codebook[j];
		dist += res * res;
	}
	return dist;
}

void CalculateTopRecall_thread(string g_file_name, int thread_id, int &hit_sum, double &precision_sum, int &candidate_count_avg, double &time, int recallR) {
	bool *candidate = new bool[DATA_NUM]();
	FILE *input_file;
	CheckOpenFile(input_file, g_input_folder, g_file_name, "rb");
	char *saveptr = NULL;
	char *buffer = new char[INT_MAX];
	CheckBuffer(buffer);
	int canditate_id;
	int hit = 0;
	int candidate_count = 0;
	double dist = 0;
	double timeStart, timeEnd;
	double time_per_query = 0;
	vector<data_dist> candidat_id_dist;
	int query_id = QUERY_NUM / THREAD_NUM * thread_id;
	for (int i = 0; i < query_id; i++) {
		fgets(buffer, INT_MAX, input_file);
	}
	for (int i = 0; i < QUERY_NUM / THREAD_NUM; i++, query_id++) {
		for (int j = 0; j < 1; j++) {
			candidate[g_query_ground_truth[query_id][j]] = 1;
		}
		hit = 0;
		candidate_count = 0;
		time_per_query = 0;
		fgets(buffer, INT_MAX, input_file);
		saveptr = buffer;
		timeStart = clock();
		while (CharToIntOnlyAutoStop(&saveptr, &canditate_id)) {
			dist = 0;
			for (int table_id = 0; table_id < TABLE_NUM; table_id++) {
				dist += Distance(g_query[table_id][query_id], g_codebook[table_id][g_data_cluster[table_id][canditate_id]]);
			}
			candidat_id_dist.push_back(data_dist(canditate_id, dist));
			candidate_count++;
			//if (candidate_count >= CANDIDATE_SIZE) break;
		}

		partial_sort(candidat_id_dist.begin(), candidat_id_dist.begin() + recallR, candidat_id_dist.end());
		//partial_sort(candidat_id_dist.begin(), candidat_id_dist.begin() + TOP_RECALL, candidat_id_dist.end(), Compare_dist);
		//stable_sort(candidat_id_dist.begin(), candidat_id_dist.end());
		timeEnd = clock();
		for (int j = 0; j < recallR; j++) {
			if (candidate[candidat_id_dist[j].ID]) {//==1
				hit = 1;
			}
		}
		hit_sum += hit;
		precision_sum += (double)hit / (double)recallR;
		for (int j = 0; j < 1; j++) {
			candidate[g_query_ground_truth[query_id][j]] = 0;
		}
		candidat_id_dist.clear();
		candidate_count_avg += candidate_count;
		time_per_query = timeEnd - timeStart;
		time += time_per_query;
	}
	candidate_count_avg = candidate_count_avg / (QUERY_NUM / THREAD_NUM);
	fclose(input_file);
	delete candidate;
	delete buffer;
	vector<data_dist> zero; // zero(0);
	candidat_id_dist.swap(zero);
}
void CalculateTopRecall(string file_name, FILE *output_file, int recallR) {
	int *hit_sum = new int[THREAD_NUM]();
	double recall = 0;
	double precision = 0;
	int candidate_coumt = 0;
	double time_sum = 0;
	double *precision_sum = new double[THREAD_NUM]();
	int *candidate_count_avg = new int[THREAD_NUM]();
	double *time = new double[THREAD_NUM]();
	thread thread_cal[THREAD_NUM];
	for (int i = 0; i < THREAD_NUM; i++) {
		thread_cal[i] = thread(CalculateTopRecall_thread, file_name, i, ref(hit_sum[i]), ref(precision_sum[i]), ref(candidate_count_avg[i]), ref(time[i]), recallR);
	}
	for (int i = 0; i < THREAD_NUM; i++) {
		thread_cal[i].join();
		recall += hit_sum[i];
		precision += precision_sum[i];
		candidate_coumt += candidate_count_avg[i];
		time_sum += time[i];
	}
	candidate_coumt /= THREAD_NUM;
	cout << "Total time (unthread): " << time_sum << endl;
	cout << "Total time (threaded): " << time_sum / (double)THREAD_NUM << endl;
	cout << "Total time (ms/query): " << time_sum / (double)THREAD_NUM / (double)QUERY_NUM << endl;
	cout << "Candidate size " << candidate_coumt << " result:\n";
	cout << "hit: " << recall << endl;
	recall = recall / (double)QUERY_NUM;
	cout << recall * 100 << "%" << endl << endl;
	//fwrite(file_name.c_str(), strlen(file_name.c_str()), 1, output_file);
	//fwrite("\n", 1, 1, output_file);
	//string str = "Candidate size " + to_string(candidate_coumt) + " result:\n";
	//fwrite(str.c_str(), strlen(str.c_str()), 1, output_file);
	//str = "Top " + to_string(TOP_RECALL) + " recall:\n";
	//fwrite(str.c_str(), strlen(str.c_str()), 1, output_file);
	fwrite(to_string(recall).c_str(), strlen(to_string(recall).c_str()), 1, output_file);
	fwrite("\n", 1, 1, output_file);
	//fwrite(to_string(recall * 100).c_str(), strlen(to_string(recall * 100).c_str()), 1, output_file);
	//fwrite("%\n\n", 3, 1, output_file);

	/*cout << "Precision\n";
	precision = precision / (double)QUERY_NUM;*/
	//cout << precision * 100 << "%" << endl << endl;

}

void Processing(int recallR) {
	/*string file_name[] = { "CandidateID_4096x4096_" + methodNumber + "_1.txt",
							"CandidateID_4096x4096_" + methodNumber + "_2.txt",
							"CandidateID_4096x4096_" + methodNumber + "_3.txt",
							"CandidateID_4096x4096_" + methodNumber + "_4.txt",
							"CandidateID_4096x4096_" + methodNumber + "_5.txt",
							"CandidateID_4096x4096_" + methodNumber + "_6.txt",
							"CandidateID_4096x4096_" + methodNumber + "_7.txt",
							"CandidateID_4096x4096_" + methodNumber + "_8.txt",
							"CandidateID_4096x4096_" + methodNumber + "_9.txt",
							"CandidateID_4096x4096_" + methodNumber + "_10.txt",
							"CandidateID_4096x4096_" + methodNumber + "_11.txt",
							"CandidateID_4096x4096_" + methodNumber + "_12.txt",
							"CandidateID_4096x4096_" + methodNumber + "_13.txt",
							"CandidateID_4096x4096_" + methodNumber + "_14.txt",
							"CandidateID_4096x4096_" + methodNumber + "_15.txt" };*/
	string file_name[] = { 
		"CandidateID_4096x4096_M6_1.txt", "CandidateID_4096x4096_M6_2.txt", "CandidateID_4096x4096_M6_3.txt", 
		"CandidateID_4096x4096_M6_4.txt", "CandidateID_4096x4096_M6_5.txt", "CandidateID_4096x4096_M6_6.txt", 
		"CandidateID_4096x4096_M6_7.txt", "CandidateID_4096x4096_M6_8.txt", "CandidateID_4096x4096_M6_9.txt", 
		"CandidateID_4096x4096_M6_10.txt", "CandidateID_4096x4096_M6_11.txt", "CandidateID_4096x4096_M6_12.txt", 
		"CandidateID_4096x4096_M6_13.txt", "CandidateID_4096x4096_M6_14.txt", "CandidateID_4096x4096_M6_15.txt", 
		"CandidateID_4096x4096_M7_1.txt", "CandidateID_4096x4096_M7_2.txt", "CandidateID_4096x4096_M7_3.txt",
		"CandidateID_4096x4096_M7_4.txt", "CandidateID_4096x4096_M7_5.txt", "CandidateID_4096x4096_M7_6.txt",
		"CandidateID_4096x4096_M7_7.txt", "CandidateID_4096x4096_M7_8.txt", "CandidateID_4096x4096_M7_9.txt",
		"CandidateID_4096x4096_M7_10.txt", "CandidateID_4096x4096_M7_11.txt", "CandidateID_4096x4096_M7_12.txt",
		"CandidateID_4096x4096_M7_13.txt", "CandidateID_4096x4096_M7_14.txt", "CandidateID_4096x4096_M7_15.txt",
		"CandidateID_4096x4096_M8_1.txt", "CandidateID_4096x4096_M8_2.txt", "CandidateID_4096x4096_M8_3.txt",
		"CandidateID_4096x4096_M8_4.txt", "CandidateID_4096x4096_M8_5.txt", "CandidateID_4096x4096_M8_6.txt",
		"CandidateID_4096x4096_M8_7.txt", "CandidateID_4096x4096_M8_8.txt", "CandidateID_4096x4096_M8_9.txt",
		"CandidateID_4096x4096_M8_10.txt", "CandidateID_4096x4096_M8_11.txt", "CandidateID_4096x4096_M8_12.txt",
		"CandidateID_4096x4096_M8_13.txt", "CandidateID_4096x4096_M8_14.txt", "CandidateID_4096x4096_M8_15.txt", };
	int inputFileNum = sizeof(file_name) / sizeof(file_name[0]);
	cout << inputFileNum << " file input" << endl;

	FILE *outputFile;//write
	//g_file_name = adcType + methodNumber + candidatepickMethod + candidateSize + "_recall@" + to_string(TOP_RECALL) + ".txt";
	g_file_name = adcType + candidatepickMethod + candidateSize + "recall@" + to_string(recallR) + ".txt";
	CheckOpenFile(outputFile, g_output_folder, g_file_name, "wb"); //"wb" write by binary

	for (int f = 0; f < inputFileNum; f++) {
		cout << "processing file = " << file_name[f] << endl;
		double startTime = clock();
		CalculateTopRecall(file_name[f], outputFile, recallR);
		TimeSpend(startTime);
	}
	fclose(outputFile);
}


int main() {
	CreateDirectory(g_output_folder, NULL);//create output floder
	LoadFile();
	for (int recallR = 1; recallR <= 100; recallR *= 10) {
		Processing(recallR);
	}
	system("pause");
}