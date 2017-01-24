#include "PlateDetection.h"
#include "Exception.h"
//#include "common/BinaryEdge.h"
//#include "common/argsort.hpp"
//#include "common/SkewProjection.h"
//#include "common/Mat2Vector.h"
#include "common/RectFunctions.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#define __DETECT_DEBUG__ 0

#if __DETECT_DEBUG__
#include <opencv2/highgui.hpp>
#include "util_cv.h"
#endif

namespace lpr{

PlateDetection::PlateDetection(void)
{
	_rect_rescale = cv::Size2f(1.5, 2.0);
}

/*
PlateDetection::PlateDetection(const std::string& cascade_file)
{
	if(!Init(cascade_file)){
		throw lpr::Exception(lpr::Exception::IO_ERROR, "Fail to load " + cascade_file, __FUNCTION__, __FILE__, __LINE__);
	}
}


PlateDetection::PlateDetection(const std::string& cascade_file, const std::string& shape_file)
{
	if (!Init(cascade_file, shape_file)) {
		throw lpr::Exception(lpr::Exception::IO_ERROR, "Fail to load " + cascade_file + " and " + shape_file, __FUNCTION__, __FILE__, __LINE__);
	}
}
*/

PlateDetection::~PlateDetection(void)
{
}


bool PlateDetection::Detect(const cv::Mat& src_img, std::vector<cv::Rect>& plates, int min_width, int max_width, const cv::Rect& search_area) const
{
	if(_detector.empty()){
		return false;
	}

	cv::Size obj_size = _detector.getOriginalWindowSize();

	cv::Rect roi = (search_area.width <= 0 || search_area.height <= 0) ? cv::Rect(0,0,src_img.cols,src_img.rows) : search_area;

	cv::Size min_size = (min_width > 0) ? cv::Size(min_width, obj_size.height * min_width / obj_size.width) : cv::Size();
	cv::Size max_size = (max_width > 0) ? cv::Size(max_width, obj_size.height * max_width / obj_size.width) : cv::Size();

	_detector.detectMultiScale(src_img(roi), plates, 1.1, 3, 0, min_size, max_size);

	std::vector<cv::Rect>::iterator plates_it, plates_end = plates.end();
	for(plates_it = plates.begin(); plates_it != plates_end; plates_it++){
		plates_it->x += roi.x;
		plates_it->y += roi.y;
	}

	return true;
}


//! より正確なナンバープレート位置を取得
void PlateDetection::AlignPlateCorners(const cv::Mat& img, const cv::Rect& detected_plate, std::vector<cv::Point2f>& corners) const
{
	assert(img.type() == CV_8UC1 || img.type() == CV_8UC3);

	if (!_shape_predictor.isReady()) {
		cv::Rect2f plate = detected_plate;
		Rect2Corners(plate, corners);
		return;
	}

	///////////// 前処理 //////////////////
	// 処理領域算出
	cv::Rect proc_rect = RescaleRectSize(detected_plate, _rect_rescale);
	proc_rect = TruncateRect(proc_rect, img.size());

	// グレースケール変換＋画像切り取り
	cv::Mat gray;
	if(img.type() == CV_8UC3){
		cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
	}
	else{
		gray = img;
	}

	_shape_predictor(gray, proc_rect, corners);
}


void PlateDetection::AlignAllPlateCorners(const cv::Mat& img, const std::vector<cv::Rect>& init_plate, std::vector<std::vector<cv::Point2f>>& corners) const
{
	std::vector<cv::Rect>::const_iterator it = init_plate.begin(), it_e = init_plate.end();
	corners.clear();
	for(;it != it_e; it++){
		std::vector<cv::Point2f> corner;
		AlignPlateCorners(img, *it, corner);
		corners.push_back(corner);
	}
}

//! コーナー情報を基に画像からプレート領域を切り取る
cv::Mat PlateDetection::extractPlateImage(const cv::Mat& src_img, const cv::Size& dst_size, const std::vector<cv::Point2f>& corners)
{
	std::vector<cv::Point2f> dst_corners;
	cv::Rect2f dst_rect(0, 0, dst_size.width, dst_size.height);
	Rect2Corners(dst_rect, dst_corners);
	cv::Mat H = cv::findHomography(corners, dst_corners);

	cv::Mat dst;
	cv::warpPerspective(src_img, dst, H, dst_size);
	return dst;
}

}