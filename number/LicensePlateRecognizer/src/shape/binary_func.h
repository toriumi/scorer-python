#ifndef __BINARY_FUNC_CV__
#define __BINARY_FUNC_CV__

#include <opencv2/core.hpp>

namespace cv_util {

	void writeMat(const cv::Mat& mat, std::ostream& out);

	template <typename _T>
	void writeMat(const cv::Mat_<_T>& mat, std::ostream& out) {
		writeMat((cv::Mat)mat, out);
	}

	void readMat(cv::Mat& mat, std::istream& in);

	template <typename _T>
	void readMat(cv::Mat_<_T>& mat, std::istream& in) {
		cv::Mat tmp;
		readMat(tmp, in);
		mat = tmp;
	}

	template <typename _T>
	void writeVector(const std::vector<_T>& vec, std::ostream& out) {
		int num_elem = vec.size();
		out.write((const char*)&num_elem, sizeof(int));
		size_t size = sizeof(_T) * num_elem;
		out.write((const char*)vec.data(), size);
	}

	template <typename _T>
	void writeVector(const std::vector<std::vector<_T> >& vec, std::ostream& out) {
		int num_elem = vec.size();
		out.write((const char*)&num_elem, sizeof(int));
		for (int i = 0;i < num_elem;i++) {
			writeVector(vec[i], out);
		}
	}


	template <typename _T>
	void writeVector(const std::vector<_T>& vec, std::ostream& out, void(*write_elem)(const _T&, std::ostream&)) {
		int num_elem = vec.size();
		out.write((const char*)&num_elem, sizeof(int));
		for (int i = 0;i < num_elem;i++) {
			write_elem(vec[i], out);
		}
	}

	template <typename _T>
	void writeVector(const std::vector<std::vector<_T> >& vec, std::ostream& out, void(*write_elem)(const _T&, std::ostream&)) {
		int num_elem = vec.size();
		out.write((const char*)&num_elem, sizeof(int));
		for (int i = 0;i < num_elem;i++) {
			writeVector(vec[i], out, write_elem);
		}
	}


	template <typename _T>
	void readVector(std::vector<_T>& vec, std::istream& in) {
		int num_elem;
		in.read((char*)&num_elem, sizeof(int));
		size_t size = num_elem * sizeof(_T);
		_T* data = (_T*)malloc(size);
		in.read((char*)data, size);
		vec = std::vector<_T>(data, data + num_elem);
		free(data);
	}

	template <typename _T>
	void readVector(std::vector<std::vector<_T>>& vec, std::istream& in) {
		int num_elem;
		in.read((char*)&num_elem, sizeof(int));
		vec.resize(num_elem);
		for (int i = 0;i < num_elem;i++) {
			readVector(vec[i], in);
		}
	}


	template <typename _T>
	void readVector(std::vector<_T>& vec, std::istream& in, void(*read_elem)(_T&, std::istream&)) {
		int num_elem;
		in.read((char*)&num_elem, sizeof(int));
		vec.resize(num_elem);
		for (int i = 0;i < num_elem;i++) {
			read_elem(vec[i], in);
		}
	}

	template <typename _T>
	void readVector(std::vector<std::vector<_T>>& vec, std::istream& in, void(*read_elem)(_T&, std::istream&)) {
		int num_elem;
		in.read((char*)&num_elem, sizeof(int));
		vec.resize(num_elem);
		for (int i = 0;i < num_elem;i++) {
			readVector(vec[i], in, read_elem);
		}
	}

}

#endif
