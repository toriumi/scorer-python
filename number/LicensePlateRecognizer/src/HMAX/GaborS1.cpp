#include <opencv2/imgproc/imgproc.hpp>
#include "GaborS1.h"
#include <opencv2/imgproc/imgproc_c.h>

using namespace std;

GaborS1::GaborS1(void)
{
}

GaborS1::~GaborS1(void)
{
}

void GaborS1::init()
{
	int layer_num = 16;
	int feature_num = 4;
	float gamma[] = {2.8, 3.6, 4.5, 5.4, 6.3, 7.3, 8.2, 9.2, 10.2, 11.3, 12.3, 13.4, 14.6, 15.8, 17.0, 18.2};
	float lambda[] = {3.5, 4.6, 5.6, 6.8, 7.9, 9.1, 10.3, 11.5, 12.7, 14.1, 15.4, 16.8, 18.2, 19.7, 21.2, 22.8};
	int gwsize[] = {7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37};
	
	std::vector<float> gammaVec, lambdaVec;
	std::vector<int> gwsizeVec;

	gammaVec.insert(gammaVec.begin(), gamma, gamma+layer_num);
	lambdaVec.insert(lambdaVec.begin(), lambda, lambda+layer_num);
	gwsizeVec.insert(gwsizeVec.begin(), gwsize, gwsize + layer_num);

	init(feature_num, gammaVec, lambdaVec, gwsizeVec);
}

void GaborS1::init(int feature_num, const std::vector<float>& gamma, const std::vector<float>& lambda, const std::vector<int>& gwsize)
{
	int layer_num = gamma.size();
	assert(layer_num == lambda.size() && layer_num == gwsize.size());
	this->NumOfLayers = layer_num;
	this->NumOfFeatures = feature_num;

	this->GaborGammas = gamma;
	this->GaborLambdas = lambda;
	this->GaborSizes = gwsize;

	// 領域確保
	this->GaborFilters.clear();

	for(int i=0;i<NumOfLayers;i++){
		vector<cv::Mat> GaborVec;
		for(int j=0;j<NumOfFeatures;j++){
			GaborVec.push_back(CreateGaborFilter(gamma[i], lambda[i], j*CV_PI/NumOfFeatures, gwsize[i]));
		}
		this->GaborFilters.push_back(GaborVec);
	}
}


void GaborS1::Write(cv::FileStorage& file_storage, const std::string& node_name) const
{
	file_storage << node_name << "{";
	file_storage << "NumOfFeatures" << this->NumOfFeatures
		<< "NumOfLayers" << this->NumOfLayers;

	file_storage << "GaborGammas" << "[";
	file_storage.writeRaw("f", (const uchar*)&GaborGammas[0], sizeof(float) * NumOfLayers);
	file_storage << "]";

	file_storage << "GaborLambdas" << "[";
	file_storage.writeRaw("f", (const uchar*)&GaborLambdas[0], sizeof(float) * NumOfLayers);
	file_storage << "]";

	file_storage << "GaborSizes" << "[";
	file_storage.writeRaw("i", (const uchar*)&GaborSizes[0], sizeof(int) * NumOfLayers);
	file_storage << "]";

	file_storage << "}";
}


void GaborS1::Read(const cv::FileNode& file_node)
{
	int layer_num = (int)file_node["NumOfLayers"];
	int feature_num = (int)file_node["NumOfFeatures"];
	std::vector<float> gammaVec(layer_num), lambdaVec(layer_num);
	std::vector<int> gwsizeVec(layer_num);
	file_node["GaborGammas"].readRaw("f", (uchar*)&gammaVec[0], layer_num);
	file_node["GaborLambdas"].readRaw("f", (uchar*)&lambdaVec[0], layer_num);
	file_node["GaborSizes"].readRaw("i", (uchar*)&gwsizeVec[0], layer_num);
	
	init(feature_num, gammaVec, lambdaVec, gwsizeVec);
}


// ガボールフィルターの作成（個別）
cv::Mat GaborS1::CreateGaborFilter(float gamma, float lambda, float angle, int size, bool circle_mask)
{
	cv::Mat gwFilter(size,size,CV_32FC1);
	double aspect = 1;

	int i,j;
	double x,y,xi,yj,c,s,xc,xs,yc,ys,gamma2;
	float val;
	double center = (double)size/2;
	double radius = center;
	s = sin(angle);
	c = cos(angle);
	gamma2 = 2*pow(gamma,2);

	float* gw_ptr = (float*)gwFilter.data;

	double div = (circle_mask) ? CV_PI * radius * radius : size*size;
	for(j=0;j<size;j++){
		yj = (double)j - center + 0.5;
		yc = yj * c;
		ys = yj * s;
		for(i=0;i<size;i++){
			xi = (double)i - center + 0.5;
			xc = xi * c;
			xs = xi * s;

			if(circle_mask && yj*yj+xi*xi > radius*radius){
				val = 0;
			}
			else{
				x = xc + ys;
				y = yc - xs;

				val = (float)(exp((-(pow(x,2)+pow(aspect*y,2))/gamma2)) * cos(2*CV_PI*x/lambda)) / div;
			}
			*gw_ptr = val;
			gw_ptr++;
		}
	}
	return gwFilter;
}


void GaborS1::filter(const cv::Mat& src_img, std::vector<std::vector<cv::Mat>>& output,
	const std::pair<int,int>& layer_mask) const
{
	// 入力画像変換
	cv::Mat	gray_img;
	if(src_img.channels() ==3){
		cv::cvtColor(src_img, gray_img, CV_RGB2GRAY);
	}
	else{
		gray_img = src_img;
	}

	// ToDo: チューニングポイント２(正規化)
//	cv::Mat gray_float_img, gray_norm_img;
//	cv::normalize(gray_img, gray_norm_img,  1, 0, CV_MINMAX, CV_32FC1);

	// Set size of Layer
	int layer_size, begin_l, end_l;
	CalcScaleRange(layer_mask, begin_l, end_l, layer_size);
	output.clear();
	output.reserve(layer_size);
	
	//フィルタ処理
	int i,j;
	for(i=begin_l;i<end_l;i++){
		vector<cv::Mat> responseVec;
		responseVec.reserve(this->NumOfFeatures);
		for(j=0;j<this->NumOfFeatures;j++){
			cv::Mat s1_mat;
			// ToDo: チューニングポイント３（第３引数でs1_matの型を指定できるため、convertToなど不要かも）
//			cv::filter2D(gray_norm_img, s1_mat, -1, GaborFilters[i][j]);
			cv::filter2D(gray_img, s1_mat, CV_32FC1, GaborFilters[i][j]);

//			s1_mat.create(gray_norm_img.rows, gray_norm_img.cols, CV_32FC1);
//			cvFilter2D(&(CvMat)gray_norm_img, &(CvMat)s1_mat, &(CvMat)GaborFilters[i][j]);
			// ToDo: チューニングポイント４（s1_matをなんらかの形で正規化した方が良いか？）
			responseVec.push_back(s1_mat);
		}
		output.push_back(responseVec);
	}
}


//! 処理対象画像をサイズ変更してフィルタ処理
void GaborS1::filter(const cv::Size& proc_size, std::vector<std::vector<cv::Mat>>& output,
	const std::pair<int,int>& layer_mask) const
{
	// 画像の拡大
	cv::Mat resized;
	cv::resize(_src_image, resized, proc_size);

	// S1, C1フィルタ処理
	filter(resized, output, layer_mask);
}

