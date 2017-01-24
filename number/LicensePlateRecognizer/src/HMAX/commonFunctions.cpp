#include "commonFunctions.h"
#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

namespace UTIL_CV{


//! Write cv::Mat as binary
/*!
\param[out] ofs output file stream
\param[in] out_mat mat to save
*/
bool writeMatBinary(std::ofstream& ofs, const cv::Mat& out_mat)
{
	if(!ofs.is_open()){
		return false;
	}
	if(out_mat.empty()){
		int s = 0;
		ofs.write((const char*)(&s), sizeof(int));
		return true;
	}
	int type = out_mat.type();
	ofs.write((const char*)(&out_mat.rows), sizeof(int));
	ofs.write((const char*)(&out_mat.cols), sizeof(int));
	ofs.write((const char*)(&type), sizeof(int));
	ofs.write((const char*)(out_mat.data), out_mat.elemSize() * out_mat.total());

	return true;
}


//! Read cv::Mat from binary
/*!
\param[in] ifs input file stream
\param[out] in_mat mat to load
*/
bool readMatBinary(std::ifstream& ifs, cv::Mat& in_mat)
{
	if(!ifs.is_open()){
		return false;
	}
	
	int rows, cols, type;
	ifs.read((char*)(&rows), sizeof(int));
	if(rows==0){
		return true;
	}
	ifs.read((char*)(&cols), sizeof(int));
	ifs.read((char*)(&type), sizeof(int));

	in_mat.release();
	in_mat.create(rows, cols, type);
	ifs.read((char*)(in_mat.data), in_mat.elemSize() * in_mat.total());

	return true;
}


//! 配列の８近傍で極大値と棚を取る点を求める。
void ExtractMaximaAndShelf(const cv::Mat& src_mat, cv::Mat& dst_mat, float threshold)
{
	assert(src_mat.type() == CV_32FC1);

	// thresholdで二値化
	cv::Mat bin_mat;
	cv::threshold(src_mat, bin_mat, threshold, 255, cv::THRESH_BINARY);
	bin_mat.convertTo(dst_mat, CV_8UC1);

	int w = src_mat.cols;
	int h = src_mat.rows;
	int x,y;

	// 注目点の周辺オフセット
	int offset_x[] = {-1,0,1,-1,1,-1,0,1};
	int offset_y[] = {-1,-1,-1,0,0,1,1,1};

	// 極大点探索
	for(int i=0; i<8; i++){
		for(y=0;y<h;y++){
			int y2 = y+offset_y[i];
			if(y2 >= 0 && y2 < h){
				for(x=0;x<w;x++){
					if(dst_mat.at<unsigned char>(y,x)){
						int x2 = x+offset_x[i];
						if(x2 >= 0 && x2 < w){
							if(src_mat.at<float>(y,x) < src_mat.at<float>(y2, x2)){
								dst_mat.at<unsigned char>(y,x) = 0;
							}
						}
					}
				}
			}
		}
	}
}


//! 二値画像中のID番目の白画素の位置を返す
cv::Point PositionOfPositiveElementID(const cv::Mat& bin_img, int id)
{
	assert(bin_img.type() == CV_8UC1);

	unsigned char* mask_ptr = bin_img.data;
	int total = bin_img.total();
	int offset=0, count=0;
	do{
		if(*(mask_ptr+offset)){
			count++;
		}
		offset++;
	}while(offset < total && count <= id); 

	if(offset>=total){
		return cv::Point(-1,-1);
	}

	int x = (offset - 1) % bin_img.cols;
	int y = (offset - 1) / bin_img.cols;
	return cv::Point(x,y);
}


//! 二値画像中のランダムな白画素の位置を返す
cv::Point RandomPositiveElement(const cv::Mat& bin_img)
{
	int pos_num = (cv::sum(bin_img))[0] / 255;
	if(pos_num <= 0){
		return cv::Point(-1,-1);
	}

	int pos_id = rand() % pos_num;

	return PositionOfPositiveElementID(bin_img, pos_id);
}


//! 指定した領域を切り取り
std::vector<cv::Mat> CutMats(const std::vector<cv::Mat>& c1_inputs, const cv::Rect& region)
{
	std::vector<cv::Mat> s2_patch;
	int size = c1_inputs.size();
	for(int i=0; i<size; i++){
		s2_patch.push_back(cv::Mat(c1_inputs[i], region).clone());
	}
	return s2_patch;
}



//! 画像の中心を算出して、angleだけ回転させる。
void RotateMat(const cv::Mat& src_mat, cv::Mat& dst_mat, cv::Mat& AffineMat, double angle, const cv::Size& dst_size)
{
	double rad_angle = angle * CV_PI / 180;
	double cos_val = std::cos(rad_angle);
	double sin_val = std::sin(rad_angle);
	int src_h = src_mat.rows;
	int src_w = src_mat.cols;

	float dest_w, dest_h;
	int dest_w_i, dest_h_i;
	if(dst_size.width <= 0 || dst_size.height <= 0){
		dest_w = cos_val * src_w + sin_val * src_h;
		dest_h = sin_val * src_w + cos_val * src_h;
		dest_w_i = (int)(dest_w + 1);
		dest_h_i = (int)(dest_h + 1);
		//dst_mat.create(dest_h_i, dest_w_i, src_mat.type());
	}
	else{
		dest_w = dst_size.width;
		dest_h = dst_size.height;
		dest_w_i = dest_w;
		dest_h_i = dest_h;
	}

	// 回転行列領域の確保
	AffineMat = cv::getRotationMatrix2D(cv::Point2f((float)src_w/2, (float)src_h/2),angle,1);

	float trans_x = (dest_w - src_w) / 2;
	float trans_y = (dest_h - src_h) / 2;

	// 平行移動成分追加
	AffineMat.at<double>(0,2) += trans_x;		// x方向
	AffineMat.at<double>(1,2) += trans_y;		// y方向

	// 回転
	cv::warpAffine(src_mat, dst_mat, AffineMat, cv::Size(dest_w_i, dest_h_i));
}


// 指定サイズをはみ出る点をサイズ内近傍点に近似
void truncatePoint(const cv::Size& size, cv::Point& pt)
{
	if(pt.x < 0)
		pt.x = 0;
	else if(pt.x >= size.width)
		pt.x = size.width - 1;
	if(pt.y < 0)
		pt.y = 0;
	else if(pt.y >= size.height)
		pt.y = size.height - 1;
}


//! 矩形の四隅の座標をcv::Transform()用Mat型へ変更
std::vector<cv::Point2d> RectToPoints(const cv::Rect& src_rect)
{
	std::vector<cv::Point2d> ret_vec(4);
	ret_vec[0] = cv::Point2d(src_rect.x, src_rect.y);
	ret_vec[1] = cv::Point2d(src_rect.x, src_rect.y + src_rect.height);
	ret_vec[2] = cv::Point2d(src_rect.x + src_rect.width, src_rect.y + src_rect.height);
	ret_vec[3] = cv::Point2d(src_rect.x + src_rect.width, src_rect.y);
	return ret_vec;
}


// pts[0]:Top Left, pts[1]:Bottom Left, pts[2]:Bottom Right, pts[3]:Top Right
cv::Mat createMask(const cv::Size& img_size, const std::vector<cv::Point2d>& pts)
{
	cv::Mat mask(img_size,CV_8UC1);
	mask = 0;

	// ax+by+c=0
	double a[4];
	double b[4];
	double c[4];

	a[0] = pts[3].y - pts[0].y;
	a[1] = pts[2].y - pts[1].y;
	a[2] = pts[1].y - pts[0].y;
	a[3] = pts[2].y - pts[3].y;

	b[0] = pts[0].x - pts[3].x;
	b[1] = pts[1].x - pts[2].x;
	b[2] = pts[0].x - pts[1].x;
	b[3] = pts[3].x - pts[2].x;

	c[0] = pts[0].y * pts[3].x - pts[3].y * pts[0].x;
	c[1] = pts[1].y * pts[2].x - pts[2].y * pts[1].x;
	c[2] = pts[0].y * pts[1].x - pts[1].y * pts[0].x;
	c[3] = pts[3].y * pts[2].x - pts[2].y * pts[3].x;

	double max_x, min_x, max_y, min_y;
	max_x = 0;
	min_x = img_size.width;
	max_y = 0;
	min_y = img_size.height;

	int i;
	for(i=0;i<4;i++){
		if(pts[i].x > max_x)
			max_x = pts[i].x;
		if(pts[i].x < min_x)
			min_x = pts[i].x;
		if(pts[i].y > max_y)
			max_y = pts[i].y;
		if(pts[i].y < min_y)
			min_y = pts[i].y;
	}
	if(max_x >= img_size.width)
		max_x = img_size.width - 1;
	if(max_y >= img_size.height)
		max_y = img_size.height - 1;
	if(min_x < 0)
		min_x = 0;
	if(min_y < 0)
		min_y = 0;
	
	unsigned char *ptr = mask.data;
	int x,y;
	int offset;
	double val[4];
	for(y=min_y; y<=max_y; y++){
		offset = y * img_size.width;
		for(x=min_x; x<=max_x; x++){
			for(i=0; i<4; i++){
				val[i] = a[i]*x + b[i]*y + c[i];
			}
			if(val[0]*val[1] <= 0 && val[2]*val[3] <= 0)
				*(ptr + offset + x)=255;
		}
	}

	return mask;
}


void resize(const std::vector<cv::Mat>& src_array, std::vector<cv::Mat>& dst_array, cv::Size dsize, double fx, double fy, int interpolation)
{
	int src_num = src_array.size();
	dst_array.resize(src_num);
	for(int i = 0; i<src_num; i++){
		cv::resize(src_array[i], dst_array[i], dsize, fx, fy, interpolation);
	}
}

};
