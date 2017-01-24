#include "C1.h"


C1::C1(void)
{
}


C1::~C1(void)
{
}


void C1::Save(const std::string& filename) const
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	this->Write(fs, "C1");
}


void C1::Load(const std::string& filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	this->Read(fs["C1"]);
}


void C1::Write(cv::FileStorage& file_storage, const std::string& node_name) const
{
	file_storage << node_name << "{";
	file_storage << "NumOfLayers" << this->NumOfLayers;

	file_storage << "MaxFilterSize" << "[";
	file_storage.writeRaw("i", (const uchar*)&MaxFilterSize[0], sizeof(int) * NumOfLayers);
	file_storage << "]";

	file_storage << "MaxFilterScale" << this->MaxFilterScale;

	file_storage << "OverlapRatio" << this->OverlapRatio;

	file_storage << "}";
}


void C1::Read(const cv::FileNode& file_node)
{
	int layer_num = (int)file_node["NumOfLayers"];
	std::vector<int> max_filter_size(layer_num);
	file_node["MaxFilterSize"].readRaw("i", (uchar*)&(max_filter_size[0]), layer_num);
	int max_scale_layer = (int)file_node["MaxFilterScale"];
	float overlap = (float)file_node["OverlapRatio"];
	init(max_filter_size, max_scale_layer, overlap);
}


void C1::init()
{
	int layer_num =  8;
	int max_filter_size[] = {8, 10, 12, 14, 16, 18, 20, 22};
	int max_scale_layer = 2;
	float overlap = 0.5;
	std::vector<int> max_filter_size_vec;
	max_filter_size_vec.insert(max_filter_size_vec.begin(), max_filter_size, max_filter_size + layer_num);
	init(max_filter_size_vec, max_scale_layer, overlap);
}


void C1::init(const std::vector<int>& max_filter_size, int max_scale_layer, float overlap)
{
	this->NumOfLayers = max_filter_size.size();
	this->MaxFilterSize = max_filter_size;
	this->MaxFilterScale = max_scale_layer;
	this->OverlapRatio = overlap;
}


void C1::filter(const std::vector<std::vector<cv::Mat>>& input, std::vector<std::vector<cv::Mat>>& output, const std::pair<int,int>& layer_mask) const
{
	// Set size of Layer
	int layer_size, begin_l, end_l;
	if(layer_mask.second >= layer_mask.first && layer_mask.second >= 0){
		begin_l = std::max<int>(layer_mask.first, 0);
		end_l = std::min<int>(layer_mask.second + 1, this->NumOfLayers);
		layer_size = end_l - begin_l;
	}
	else{
		begin_l = 0;
		end_l = this->NumOfLayers;
		layer_size = this->NumOfLayers;
	}
	output.clear();
	output.reserve(layer_size);

	int s;
	int i,j,k,n;
	
	// C1‘w‚Ì¶¬	
	cv::Rect rect;
	int width;
	int height;
	double min_val, max_val, tmp_max;
	int x,y;

	int input_layer_max = input.size() + 1 - this->MaxFilterScale;
	for(i=begin_l, k=0; i < end_l && k < input_layer_max; i++, k += this->MaxFilterScale){
		s = this->MaxFilterSize[i];
		rect.width= s;
		rect.height= s;

		width = this->calcSizeS1toC1(input[k][0].cols,i);
		height = this->calcSizeS1toC1(input[k][0].rows,i);

		int feature_num = input[k].size();
		std::vector<cv::Mat> mat_vec;
		mat_vec.reserve(feature_num);
		for(j=0;j<feature_num;j++){
			cv::Mat c1_mat(height,width,CV_32FC1);
			for(y=0;y<height;y++){
				rect.y= (int)((float)y * s *(1.0 - this->OverlapRatio));
				for(x=0;x<width;x++){
					rect.x = (int)((float)x * s *(1.0 - this->OverlapRatio));
					tmp_max = -255;
					for(n=0;n < this->MaxFilterScale ;n++){
						cv::Mat local_mat(input[k + n][j],rect);
						cv::minMaxLoc(local_mat,&min_val,&max_val);
						if(tmp_max < max_val)	tmp_max = max_val;
					}
					c1_mat.at<float>(y,x) = (float)tmp_max;
				}
			}
			mat_vec.push_back(c1_mat);
		}
		output.push_back(mat_vec);
	}
}

