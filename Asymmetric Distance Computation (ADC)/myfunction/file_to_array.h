
#include "char_to_num.h"
#include"open_file_check_buffer.h"
#define BUFFER_SIZE_FOR_ONE_NUM 64

void FileToArray1D_int(FILE *input_file, int *arr, int data_num) {
	char *saveptr = NULL;
	char *buffer = new char[BUFFER_SIZE_FOR_ONE_NUM * 1];
	CheckBuffer(buffer);
	for (int j = 0; j < data_num; j++) {
		fgets(buffer, BUFFER_SIZE_FOR_ONE_NUM * 1, input_file);
		saveptr = buffer;
		arr[j] = CharToDouble(&saveptr);
	}
	delete buffer;
}

void FileToArray1D_unint8(FILE *input_file, unsigned __int8 *arr, int data_num) {
	char *saveptr = NULL;
	char *buffer = new char[BUFFER_SIZE_FOR_ONE_NUM * 1];
	CheckBuffer(buffer);
	for (int j = 0; j < data_num; j++) {
		fgets(buffer, BUFFER_SIZE_FOR_ONE_NUM * 1, input_file);
		saveptr = buffer;
		arr[j] = CharToDouble(&saveptr);
	}
	delete buffer;
}

void FileToArray1D(FILE *input_file, double *arr, int data_num, int data_dim) {
	char *saveptr = NULL;
	char *buffer = new char[BUFFER_SIZE_FOR_ONE_NUM * data_dim];
	CheckBuffer(buffer);
	fgets(buffer, BUFFER_SIZE_FOR_ONE_NUM * data_dim, input_file);
	saveptr = buffer;
	for (int j = 0; j < data_dim; j++) {
		arr[j] = CharToDouble(&saveptr);
	}
	delete buffer;
}

void FileToArray2D_int(FILE *input_file, int **arr, int data_num, int data_dim) {
	char *saveptr = NULL;
	char *buffer = new char[BUFFER_SIZE_FOR_ONE_NUM * 1000];
	CheckBuffer(buffer);
	for (int i = 0; i < data_num; i++) {
		fgets(buffer, BUFFER_SIZE_FOR_ONE_NUM *1000, input_file);
		saveptr = buffer;
		for (int j = 0; j < data_dim; j++) {
			arr[i][j] = CharToIntOnly(&saveptr);//¥[³t
		}
	}
	delete buffer;
}

void FileToArray2D_double(FILE *input_file, double **arr, int data_num, int data_dim) {
	char *saveptr = NULL;
	char *buffer = new char[BUFFER_SIZE_FOR_ONE_NUM * data_dim];
	CheckBuffer(buffer);
	for (int i = 0; i < data_num; i++) {
		fgets(buffer, BUFFER_SIZE_FOR_ONE_NUM*data_dim, input_file);
		saveptr = buffer;
		for (int j = 0; j < data_dim; j++) {
			arr[i][j] = CharToDouble(&saveptr);
		}
	}
	delete buffer;
}

void FileToArray2D_double_uchar(FILE *input_file, unsigned char **arr, int data_num, int data_dim) {//0~255
	char *saveptr = NULL;
	char *buffer = new char[BUFFER_SIZE_FOR_ONE_NUM * data_dim];
	CheckBuffer(buffer);
	for (int i = 0; i < data_num; i++) {
		fgets(buffer, BUFFER_SIZE_FOR_ONE_NUM*data_dim, input_file);
		saveptr = buffer;
		for (int j = 0; j < data_dim; j++) {
			arr[i][j] = CharToDouble(&saveptr);
		}
	}
	delete buffer;
}

void CheckArray(double **arr, int dim) {
	int row;
	while (scanf_s("%d", &row)) {
		for (int i = 0; i < dim; i++) {
			printf("%f ", arr[row][i]);
		}
	}
}

void CheckArray(int **arr, int dim) {
	int row;
	while (scanf_s("%d", &row)) {
		for (int i = 0; i < dim; i++) {
			printf("%d ", arr[row][i]);
		}
	}
}