#include "binary_func.h"

namespace cv_util {

	void writeMat(const cv::Mat& mat, std::ostream& out) {
		out.write((const char*)&mat.rows, sizeof(int));
		out.write((const char*)&mat.cols, sizeof(int));
		int type = mat.type();
		out.write((const char*)&type, sizeof(int));
		size_t data_size = mat.total() * mat.elemSize();
		out.write((const char*)mat.data, data_size);
	}

	void readMat(cv::Mat& mat, std::istream& in) {
		int rows, cols, type;
		in.read((char*)&rows, sizeof(int));
		in.read((char*)&cols, sizeof(int));
		in.read((char*)&type, sizeof(int));
		mat.create(rows, cols, type);
		size_t data_size = mat.total() * mat.elemSize();
		in.read((char*)mat.data, data_size);
	}

}