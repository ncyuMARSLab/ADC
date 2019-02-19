#define NEW2D(arr_i, arr_j, TYPE) (TYPE **)New2d(arr_i, arr_j, sizeof(TYPE))
#define NEW3D(arr_i, arr_j, arr_k, TYPE) (TYPE ***)New3d(arr_i, arr_j, arr_k, sizeof(TYPE))
#define NEW4D(arr_i, arr_j, arr_k, arr_l, TYPE) (TYPE ****)New4d(arr_i, arr_j, arr_k, arr_l, sizeof(TYPE))

void* New2d(unsigned __int64 arr_i, unsigned __int64 arr_j, unsigned __int64 size) {
	void **p = (void**)new char *[arr_i];
	for (int i = 0; i < arr_i; i++) {
		p[i] = (void*)new char[arr_j*size]();
	}
	return p;
}

void* New3d(unsigned __int64 arr_i, unsigned __int64 arr_j, unsigned __int64 arr_k, unsigned __int64 size) {
	void ***p = (void***)new char **[arr_i];
	for (int i = 0; i < arr_i; i++) {
		p[i] = (void**)new char*[arr_j];
		for (int j = 0; j < arr_j; j++) {
			p[i][j] = (void*)new char[arr_k*size]();
		}
	}
	return p;
}

void* New4d(unsigned __int64 arr_i, unsigned __int64 arr_j, unsigned __int64 arr_k, unsigned __int64 arr_l, unsigned __int64 size) {
	void ****p = (void****)new char ***[arr_i];
	for (int i = 0; i < arr_i; i++) {
		p[i] = (void***)new char**[arr_j];
		for (int j = 0; j < arr_j; j++) {
			p[i][j] = (void**)new char*[arr_k];
			for (int k = 0; k < arr_k; k++) {
				p[i][j][k] = (void*)new char[arr_l*size]();
			}
		}
	}
	return p;
}
void Delete4DArray(void**** arr, unsigned __int64 arr_i, unsigned __int64 arr_j, unsigned __int64 arr_k, unsigned __int64 arr_l) {
	for (int i = 0; i < arr_i; i++) {
		for (int j = 0; j < arr_j; j++) {
			for (int k = 0; k < arr_k; k++) {
				delete[] arr[i][j][k];
			}
			delete[] arr[i][j];
		}
		delete[] arr[i];
	}
	delete[] arr;
}

void Delete3DArray(void*** arr, unsigned __int64 arr_i, unsigned __int64 arr_j, unsigned __int64 arr_k) {
	for (int i = 0; i < arr_i; i++) {
		for (int j = 0; j < arr_j; j++) {
			delete[] arr[i][j];
		}
		delete[] arr[i];
	}
	delete[] arr;
}
void Delete2DArray(void** arr, unsigned __int64 arr_i, unsigned __int64 arr_j) {
	for (int i = 0; i < arr_i; i++)
		delete[] arr[i];
	delete[] arr;
}
