#ifndef __GABOR_S1__
#define __GABOR_S1__

#include <opencv2/core/core.hpp>
#include "S1.h"

class GaborS1 :
	public S1
{
public:
	GaborS1(void);
	~GaborS1(void);

	void init();
	void init(int feature_num, const std::vector<float>& gamma, const std::vector<float>& lambda, const std::vector<int>& size);
	void filter(const cv::Mat& src_img, std::vector<std::vector<cv::Mat>>& output, const std::pair<int,int>& layer_mask = std::pair<int,int>(-1,-1)) const;

	//! 処理対象画像をセット
	void SetProcImage(const cv::Mat& img){
		_src_image = img;
	};

	//! 処理対象画像をサイズ変更してフィルタ処理
	void filter(const cv::Size& proc_size, std::vector<std::vector<cv::Mat>>& output,
		const std::pair<int,int>& layer_mask = std::pair<int,int>(-1,-1)) const;

	void Write(cv::FileStorage& file_storage, const std::string& node_name) const; 
	void Read(const cv::FileNode& file_node);

	std::vector<std::vector<cv::Mat>> GaborFilters;

private:
	cv::Mat CreateGaborFilter(float gamma, float lambda, float angle, int size, bool circle_mask = true);

	//! Parameters of Gabor Filter
	std::vector<float> GaborGammas;	
	std::vector<float> GaborLambdas;
	std::vector<int> GaborSizes;

	cv::Mat _src_image;
};

#endif