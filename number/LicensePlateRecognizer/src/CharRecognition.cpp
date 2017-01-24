#include "CharRecognition.h"
#include "Exception.h"
#include <opencv2/imgproc.hpp>
#include "common/argsort.hpp"
#include "common/LocalContrastNormalization.hpp"

namespace lpr{

HMAX CharRecognition::_hmax;

CharRecognition::CharRecognition(void)
{
	_TrainSize = cv::Size(48, 96);
	_LcnEnable = true;
}


CharRecognition::~CharRecognition(void)
{
}


bool CharRecognition::init(const cv::Size& train_size, const cv::Size& lcn_size)
{
	if(train_size.width < 1 || train_size.height < 1)
		return false;
	_TrainSize = train_size;

	if(lcn_size.width < 1 || lcn_size.height < 1){
		int lcn_size = (_TrainSize.width < _TrainSize.height) ? _TrainSize.width : _TrainSize.height;
		lcn_size /= 2;
		lcn_size += (lcn_size+1) % 2;
		_LcnSize = cv::Size(lcn_size, lcn_size);
	}
	else
		_LcnSize = lcn_size;

	return true;
}


void CharRecognition::init_hmax()
{
	std::vector<int> max_filter_size_vec;
	max_filter_size_vec.push_back(4);
	_hmax.hmax_c1->init(max_filter_size_vec, 2, 0.5);
}


void CharRecognition::ExtractFeatures(const cv::Mat& img, cv::Mat& features) const
{
	assert(img.type() == CV_8UC1 || img.type() == CV_8UC3);
	cv::Mat gray;

	if(img.channels() == 3){
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	}
	else{
		gray = img;
	}

	cv::Mat resizeMat;
	cv::resize(gray, resizeMat, _TrainSize);
//	cv::resize(gray, resizeMat, cv::Size(96,48));

	cv::Mat srcMat, lcnMat;
	resizeMat.convertTo(srcMat, CV_32FC1);
	if(_LcnEnable)
		LocalContrastNormalization(srcMat, lcnMat, _LcnSize);
	else
		lcnMat = srcMat;

	std::vector<std::vector<cv::Mat>> s1_out, c1_out;
	_hmax.hmax_s1->filter(lcnMat, s1_out, std::pair<int,int>(0,1));
	_hmax.hmax_c1->filter(s1_out, c1_out);

	cv::Mat hmax_out;
	HMAX::GenerateQuery(c1_out, hmax_out);
	cv::normalize(hmax_out, features);
	//HMAX::GenerateQuery(c1_out, features);
}


int CharRecognition::Predict(const cv::Mat& img) const
{
	cv::Mat scores = _Scores(img);

	double minVal, maxVal;
	cv::Point min_id, max_id;
	cv::minMaxLoc(scores, &minVal, &maxVal, &min_id, &max_id);

	return _Labels[max_id.y];
}


int CharRecognition::Predict(const cv::Mat& img, float& prob) const
{
	std::vector<float> probs;
	Probability(img, probs);
	return _Labels[max_arg(probs, prob)];
}


void CharRecognition::Probability(const cv::Mat& img, std::vector<float>& probs) const
{
	cv::Mat scores = _Scores(img);
	cv::Mat exp_scores;
	cv::exp(-scores, exp_scores);
	exp_scores += 1.0;
	cv::divide(1.0, exp_scores, scores);
	
	cv::Mat probmat;
	cv::normalize(scores, probmat, 1.0, 0, cv::NORM_L1);
	probs.resize(probmat.rows);
	for (int i = 0;i<probmat.rows;i++)
		probs[i] = probmat.at<float>(i,0);
}


bool CharRecognition::Load(const std::string& filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	return Read(fs["CharRecognition"]);
}


bool CharRecognition::Read(const cv::FileNode& fn)
{
	if(fn.empty())
		return false;

	fn["TrainSize"][0] >> _TrainSize.width;
	fn["TrainSize"][1] >> _TrainSize.height;
	_Labels.clear();
	cv::read(fn["Labels"], _Labels);
	//cv::FileNodeIterator it, it_e = fn["Labels"].end();
	//for(it = fn["Labels"].begin(); it != it_e; it++){
	//	_Labels.push_back((int)(*it));
	//}
	fn["Filter"] >> _Filter; 
	fn["Bias"] >> _Bias;
	return true;
}


bool CharRecognition::Save(const std::string& filename) const
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	return Write(fs, "CharRecognition");
}


bool CharRecognition::Write(cv::FileStorage& fs, const std::string& node_name) const
{
	if(!fs.isOpened())
		return false;

	fs << node_name << "{";
	fs << "TrainSize" << _TrainSize;
	cv::write(fs, "Labels", _Labels);
	fs << "Filter" << _Filter;
	fs << "Bias" << _Bias;
	fs << "}";
	return true;
}


//! Mat‚ð‚Â‚È‚°‚ÄAs”‚ªMat‚Ì”A—ñ”‚ªMat‚Ì—v‘f”‚Æ‚È‚é‚P‚Â‚ÌMat‚ð¶¬
void CharRecognition::ConcatMatFeature2D(const std::vector<cv::Mat>& train_features, cv::Mat& concat_feature)
{
	if(train_features.empty() || train_features[0].empty())
		return;

	int rows = train_features.size();
	int cols = train_features[0].total();
	int type = train_features[0].type();
	concat_feature.create(rows, cols, type);
	unsigned char* data_ptr = concat_feature.data;
	int data_size = concat_feature.elemSize() * cols;
	for(int i=0; i<rows; i++){
		assert(train_features[i].total() == cols);
		assert(train_features[i].type() == type);
		memcpy(data_ptr, train_features[i].data, data_size);
		data_ptr += data_size;
	}
}

}
