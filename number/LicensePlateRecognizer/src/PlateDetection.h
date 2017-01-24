#ifndef __PLATE_DETECTION__
#define __PLATE_DETECTION__

#include <opencv2/objdetect.hpp>
#include "shape/shape_predictor.h"


namespace lpr{

class PlateDetection
{
public:
	PlateDetection(void);
	//PlateDetection(const std::string& cascade_file);
	//PlateDetection(const std::string& cascade_file, const std::string& shape_predict_file);

	~PlateDetection(void);

	bool Init(const std::string& cascade_file){
		return _detector.load(cascade_file);
	}

	bool Init(const std::string& cascade_file, const std::string& shape_predict_file) {
		bool detector_ret = _detector.load(cascade_file);
		bool shape_predictor_ret = _shape_predictor.Load(shape_predict_file);
		return detector_ret & shape_predictor_ret;
	}

	bool Detect(const cv::Mat& img, std::vector<cv::Rect>& plates, int min_width = 0, int max_widht = 0, const cv::Rect& search_area = cv::Rect(0,0,0,0)) const;

	//! より正確なナンバープレート位置を取得
	/*!
	\param[in] img 入力画像
	\param[in] init_plate 画像内の初期プレート位置
	\param[in] class_num_area 分類番号領域(DetectCharStringsRectで抽出)
	\param[in] car_num_area 車両番号領域(DetectCharStringsRectで抽出)
	\param[out] corners 左上から時計回りにプレートの位置
	*/
	void AlignPlateCorners(const cv::Mat& img, const cv::Rect& init_plate, std::vector<cv::Point2f>& corners) const;

	void AlignAllPlateCorners(const cv::Mat& img, const std::vector<cv::Rect>& init_plate, std::vector<std::vector<cv::Point2f>>& corners) const;

	void setRectRescale(const cv::Size2f& rescale) {
		assert(rescale.width > 0 && rescale.height > 0);
		_rect_rescale = rescale;
	}

	cv::Size2f getRectRescale() const {
		return _rect_rescale;
	}

	//! コーナー情報を基に画像からプレート領域を切り取る
	/*!
	\param[in] src_img 入力画像
	\param[in] dst_size 出力画像サイズ
	\param[in] corners プレートの四隅（左上から時計回り）
	\return 切り取られた画像
	*/
	static cv::Mat extractPlateImage(const cv::Mat& src_img, const cv::Size& dst_size, const std::vector<cv::Point2f>& corners);

private:
	//! ナンバープレート検出器
	mutable cv::CascadeClassifier _detector;

	//! プレート形状推定機
	cvsr::shape_predictor _shape_predictor;

	//! 検出結果の矩形を拡大するスケール（形状検出はこの中で行われる）
	cv::Size2f _rect_rescale;

};

}

#endif

