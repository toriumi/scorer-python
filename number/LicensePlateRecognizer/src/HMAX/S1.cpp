#include "S1.h"
#include "GaborS1.h"


S1::S1(void)
{
}


S1::~S1(void)
{
}

void S1::Save(const std::string& filename) const
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	this->Write(fs, "S1");
}

void S1::Load(const std::string& filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	this->Read(fs["S1"]);
}

void S1::CalcScaleRange(const std::pair<int,int>& layer_mask, int& start_scale_id, int& stop_scale_id, int& num_scale) const
{
	// Set size of Layer
	if(layer_mask.second >= layer_mask.first && layer_mask.second >= 0){
		start_scale_id = std::max<int>(layer_mask.first, 0);
		stop_scale_id = std::min<int>(layer_mask.second + 1, this->NumOfLayers);
		num_scale = stop_scale_id - start_scale_id;
	}
	else{
		start_scale_id = 0;
		stop_scale_id = this->NumOfLayers;
		num_scale = this->NumOfLayers;
	}
}


S1* S1::create()
{
	return new GaborS1();
}
