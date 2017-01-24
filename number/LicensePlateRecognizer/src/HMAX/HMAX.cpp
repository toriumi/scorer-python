#include "HMAX.h"
#include "GaborS1.h"
#include "commonFunctions.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <iostream>

HMAX::HMAX(void)
{
	//this->hmax_s1 = S1::create(S1::HAAR);
	this->hmax_s1 = S1::create();
	this->hmax_c1 = new C1();

	hmax_s1->init();
	hmax_c1->init();
}


HMAX::~HMAX(void)
{
	delete hmax_s1;
	delete hmax_c1;
}


void HMAX::GenerateQuery(const std::vector<std::vector<cv::Mat>>& outputs, cv::Mat& query_data)
{
	std::vector<std::vector<std::vector<cv::Mat>>> query_vec;
	query_vec.push_back(outputs);
	GenerateTrainData(query_vec, query_data);
}


//! 出力ベクトル群を訓練用データへコピー
void HMAX::CopyToTrainDataRow(const std::vector<std::vector<cv::Mat>>& outputs, cv::Mat& train_data, int idx)
{
	if(outputs.empty() || outputs[0].empty() || outputs[0][0].empty()){
		return;
	}
	int num_layer = outputs.size(),
		num_feature = outputs[0].size(),
		s_len = outputs[0][0].cols * outputs[0][0].rows;
	float* dest_ptr = &(train_data.at<float>(idx,0));
	for(int l=0; l<num_layer;l++){
		for(int f=0; f<num_feature; f++){
			float *src_ptr = (float*)(outputs[l][f].data);
			for(int s = 0; s < s_len; s++){
				*dest_ptr = *src_ptr;
				dest_ptr++;
				src_ptr++;
			}
		}
	}
}


void HMAX::GenerateTrainData(const std::vector<std::vector<std::vector<cv::Mat>>>& outputs, cv::Mat& train_data)
{
	int idx = 0;
	while(outputs[idx].empty()){
		idx++;
	}
	int num_layer = outputs[idx].size();
	int num_feature = outputs[idx][0].size();
	int s_len = outputs[idx][0][0].cols * outputs[idx][0][0].rows;
	int d_cols = num_layer * num_feature * s_len;
	int d_rows = outputs.size();

	train_data = cv::Mat::zeros(d_rows, d_cols, CV_32FC1);
	
	int dr;
	for(dr = 0; dr < d_rows; dr++){
		CopyToTrainDataRow(outputs[dr], train_data, dr);
	}
}


bool HMAX::Save(const std::string& filename) const
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	return Write(fs, "HMAX");
}


bool HMAX::Load(const std::string& filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	return Read(fs["HMAX"]);
}


bool HMAX::Write(cv::FileStorage& fs, const std::string& node_name) const
{
	hmax_s1->Write(fs, "S1");
	hmax_c1->Write(fs, "C1");
	return true;
}


bool HMAX::Read(const cv::FileNode& fn)
{
	delete hmax_s1;
	hmax_s1 = S1::create();

	hmax_s1->Read(fn["S1"]);
	hmax_c1->Read(fn["C1"]);
	return true;
}


//! 信号をファイルに保存（主にデバッグ用）
void HMAX::WriteSignal(cv::FileStorage& fs, const std::string& nodename, const std::vector<std::vector<cv::Mat>>& hmax_signal)
{
	if(!nodename.empty()){
		fs << nodename;
	}
	fs << "{";
	int ln = hmax_signal.size();
	fs << "NumLayers" << ln;
	fs << "Layers" << "[";
	for(int i=0; i<ln; i++){
		fs << "{";
		int feat_num = hmax_signal[i].size();
		fs << "NumFeatures" << feat_num;
		fs << "FeatureMap" << "[";
		for(int j=0; j<feat_num; j++){
			if(!hmax_signal[i][j].empty()){
				fs << "{";
				fs << "id" << j;
				cv::write(fs, "Map", hmax_signal[i][j]);
				fs << "}";
			}
		}
		fs << "]";
		fs << "}";
	}
	fs << "]";
	fs << "}";
}


//! 信号をファイルに保存（主にデバッグ用）
void HMAX::ReadSignal(const cv::FileNode& fn, std::vector<std::vector<cv::Mat>>& hmax_signal)
{
	hmax_signal.clear();
	int ln = (int)fn["NumLayers"];
	cv::FileNode fn1 = fn["Layers"];
	for(int i=0; i<ln; i++){
		int feat_num = (int)fn1[i]["NumFeatures"];
		cv::FileNode fn2 = fn1[i]["FeatureMap"];
		std::vector<cv::Mat> feature_map(feat_num);
		cv::FileNodeIterator it = fn2.begin(),
			it_end = fn2.end();
		for(; it != it_end; it++){
			int id = (int)((*it)["id"]);
			cv::Mat fmap;
			(*it)["Map"] >> fmap;
			feature_map[id] = fmap;
		}
		hmax_signal.push_back(feature_map);
	}
}


//! 出力ベクトルのファイル書き込み
bool HMAX::WriteSignalBinary(std::ofstream& ofs, const std::vector<std::vector<cv::Mat>>& outputs)
{
	if(!ofs.is_open()){
		return false;
	}
	int num1 = outputs.size();
	ofs.write((const char*)(&num1), sizeof(int));
	for(int i=0; i<num1; i++){
		int num2 = outputs[i].size();
		ofs.write((const char*)(&num2), sizeof(int));
		for(int j=0; j<num2; j++){
			if(!UTIL_CV::writeMatBinary(ofs, outputs[i][j])){
				return false;
			}
		}
	}
	return true;
}

bool HMAX::SaveSignalBinary(const std::string& filename, const std::vector<std::vector<cv::Mat>>& outputs){
	std::ofstream ofs(filename, std::ios::binary);
	return WriteSignalBinary(ofs, outputs);
};


//! フィルター出力のファイル読み込み
bool HMAX::ReadSignalBinary(std::ifstream& ifs, std::vector<std::vector<cv::Mat>>& outputs)
{
	if(!ifs.is_open()){
		return false;
	}
	int num1;
	ifs.read((char*)(&num1), sizeof(int));
	for(int i=0; i<num1; i++){
		int num2;
		ifs.read((char*)(&num2), sizeof(int));
		std::vector<cv::Mat> mat_vec;
		for(int j=0; j<num2; j++){
			cv::Mat data;
			if(!UTIL_CV::readMatBinary(ifs, data)){
				return false;
			}
			mat_vec.push_back(data);
		}
		outputs.push_back(mat_vec);
	}
	return true;
}


bool HMAX::LoadSignalBinary(const std::string& filename, std::vector<std::vector<cv::Mat>>& outputs){
	std::ifstream ifs(filename, std::ios::binary);
	return ReadSignalBinary(ifs, outputs);
};
