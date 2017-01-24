#ifndef __S1__
#define __S1__

#include <opencv2/core/core.hpp>

class S1
{
public:
	S1(void);
	virtual ~S1(void);

	static S1* create();

	virtual void init() = 0;
	virtual void filter(const cv::Mat& img, std::vector<std::vector<cv::Mat>>& output,
		const std::pair<int,int>& layer_mask = std::pair<int,int>(-1,-1)) const = 0;

	//! 処理対象画像をセット
	virtual void SetProcImage(const cv::Mat& img) = 0;

	//! 処理対象画像をサイズ変更してフィルタ処理
	virtual void filter(const cv::Size& proc_size, std::vector<std::vector<cv::Mat>>& output,
		const std::pair<int,int>& layer_mask = std::pair<int,int>(-1,-1)) const = 0;

	virtual void Save(const std::string& filename) const;
	virtual void Load(const std::string& filename);

	virtual void Write(cv::FileStorage& file_storage, const std::string& node_name) const = 0; 
	virtual void Read(const cv::FileNode& file_node) = 0;

	int GetNumOfLayers() const{return NumOfLayers;};
	int GetNumOfFeatures() const{return NumOfFeatures;};

protected:
	int NumOfLayers;
	int NumOfFeatures;

	void CalcScaleRange(const std::pair<int,int>& layer_mask, int& start_scale_id, int& stop_scale_id, int& num_scale) const;
};

#endif