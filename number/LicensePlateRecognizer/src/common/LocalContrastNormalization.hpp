#ifndef __LCN__
#define __LCN__

#include <opencv2/imgproc/imgproc.hpp>

//局所コントラスト正規化
/*!
filter_sizeは奇数
*/
template<typename T>
void LocalContrastNormalization_(const cv::Mat_<T>& src, cv::Mat_<T>& dst, const cv::Size& filter_size)
{
	cv::Size filter_size2 = filter_size;
	if(filter_size2.width % 2 == 0)
		filter_size2.width += 1;
	if(filter_size2.height % 2 == 0)
		filter_size2.height += 1;

	cv::Mat sum, sqsum;
	cv::integral(src, sum, sqsum, CV_64FC1);	// 積分画像作成

	cv::Point filter_center(filter_size2.width / 2, filter_size2.height / 2);

	cv::Rect proc_area;
	dst.create(src.size());
	for(int r=0; r<src.rows; r++){
		///// マージン処理 /////
		proc_area.y = r - filter_center.y;
		proc_area.height = filter_size2.height;
		if(proc_area.y < 0){
			proc_area.height += proc_area.y;
			proc_area.y = 0;
		}
		else if(proc_area.y + proc_area.height > src.rows){
			proc_area.height = src.rows - proc_area.y;
		}
		int br = proc_area.y+1;
		int er = proc_area.y + proc_area.height;
		/////////////////////////

		for(int c=0; c<src.cols; c++){
			///// マージン処理 /////
			proc_area.x = c - filter_center.x;
			proc_area.width = filter_size2.width;
			if(proc_area.x < 0){
				proc_area.width += proc_area.x;
				proc_area.x = 0;
			}
			else if(proc_area.x + proc_area.width > src.cols){
				proc_area.width = src.cols - proc_area.x;
			}
			int bc = proc_area.x+1;
			int ec = proc_area.x + proc_area.width;
			/////////////////////////

			int area = proc_area.width * proc_area.height;
			double mean = (sum.at<double>(br,bc) + sum.at<double>(er,ec) - sum.at<double>(br, ec) - sum.at<double>(er, bc)) / area;
			double dev = (sqsum.at<double>(br,bc) + sqsum.at<double>(er,ec) - sqsum.at<double>(br, ec) - sqsum.at<double>(er, bc)) / area;
			dev -= (mean * mean);
			if(dev == 0)
				dst.template at<T>(r, c) = src.template at<T>(r, c);
			else
				dst.template at<T>(r, c) = (src.template at<T>(r, c) - mean) / dev;
		}
	}
};


void LocalContrastNormalization(const cv::Mat& src, cv::Mat& dst, const cv::Size& filter_size);

void LocalContrastNormalization(const std::vector<cv::Mat>& src, std::vector<cv::Mat>& dst, const cv::Size& filter_size);

void LocalContrastNormalization(const std::vector<std::vector<cv::Mat>>& src, 
	std::vector<std::vector<cv::Mat>>& dst, const cv::Size& filter_size);

#endif