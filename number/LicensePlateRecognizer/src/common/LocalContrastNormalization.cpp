#include "LocalContrastNormalization.hpp"

//局所コントラスト正規化
/*!
filter_sizeは奇数
*/

void LocalContrastNormalization(const cv::Mat& src, cv::Mat& dst, const cv::Size& filter_size)
{
	assert(src.type() == CV_32FC1 || src.type() == CV_64FC1);

	if(src.type() == CV_32FC1){
		LocalContrastNormalization_((cv::Mat_<float>&)src, (cv::Mat_<float>&)dst, filter_size);
	}
	else if(src.type() == CV_64FC1){
		LocalContrastNormalization_((cv::Mat_<double>&)src, (cv::Mat_<double>&)dst, filter_size);
	}
}


void LocalContrastNormalization(const std::vector<cv::Mat>& src, std::vector<cv::Mat>& dst, const cv::Size& filter_size)
{
	dst.clear();
	std::vector<cv::Mat>::const_iterator it = src.begin(), it_e = src.end();
	for(;it!=it_e; it++){
		cv::Mat proc_mat;
		LocalContrastNormalization(*it, proc_mat, filter_size);
		dst.push_back(proc_mat);
	}
};


void LocalContrastNormalization(const std::vector<std::vector<cv::Mat>>& src, 
	std::vector<std::vector<cv::Mat>>& dst, const cv::Size& filter_size)
{
	dst.clear();
	std::vector<std::vector<cv::Mat>>::const_iterator it = src.begin(), it_e = src.end();
	for(;it!=it_e; it++){
		std::vector<cv::Mat> proc_mats;
		LocalContrastNormalization(*it, proc_mats, filter_size);
		dst.push_back(proc_mats);
	}
};
