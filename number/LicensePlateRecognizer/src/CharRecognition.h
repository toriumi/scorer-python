#ifndef __CHAR_RECOGNITION__
#define __CHAR_RECOGNITION__

#include "HMAX/HMAX.h"

namespace lpr{

class CharRecognition
{
public:
	CharRecognition(void);
	~CharRecognition(void);

	bool init(const cv::Size& train_size, const cv::Size& lcn_size = cv::Size(-1,-1));

	bool isReady() const{
		return !_Filter.empty();
	};

	static void init_hmax();

	void ExtractFeatures(const cv::Mat& img, cv::Mat& features) const;

	void SetTrainedParameters(const cv::Mat& filter, const cv::Mat& bias, const std::vector<int>& labels) {
		_Filter = filter;
		_Bias = bias;
		_Labels = labels;
	}

	void EnableLCN(bool enable){
		_LcnEnable = enable;
	};

	int Predict(const cv::Mat& img) const;

	int Predict(const cv::Mat& img, float& prob) const;

	void Probability(const cv::Mat& img, std::vector<float>& probs) const;

	int NumClasses() const {
		return _Labels.size();
	}

	bool Load(const std::string& data_file);
	
	bool Read(const cv::FileNode& fn);

	bool Save(const std::string& train_data) const;

	bool Write(cv::FileStorage& fs, const std::string& node_name) const;

	//! Matをつなげて、行数がMatの数、列数がMatの要素数となる１つのMatを生成
	static void ConcatMatFeature2D(const std::vector<cv::Mat>& train_features, cv::Mat& concat_feature);

private:
	static HMAX _hmax;	// 特徴抽出器
	cv::Size _TrainSize;	// 学習画像サイズ
	cv::Size _LcnSize;	// 局所コントラスト正規化のウィンドウサイズ
	bool _LcnEnable; // 局所コントラスト正規化を使用するかどうか
	cv::Mat _Filter;	// 学習済みデータ
	cv::Mat _Bias;
	std::vector<int> _Labels;

	cv::Mat _Scores(const cv::Mat& img) const{
		cv::Mat feature;
		ExtractFeatures(img, feature);
		return _Filter * feature.t() + _Bias;
	}

};

}

#endif