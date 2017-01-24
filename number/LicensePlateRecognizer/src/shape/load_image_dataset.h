#ifndef __LOAD_IMAGE_DATASET__
#define __LOAD_IMAGE_DATASET__

#include <opencv2/core.hpp>
#include "shape_predictor.h"

namespace cvsr {

	//! XMLファイルから学習データ/テストデータを読み込み
	/*!
	\param[in] filename XMLファイル
	\param[out] image_list 画像
	\param[out] rectangles 矩形
	\param[out] landmarks 特徴点
	\param[in] relative_path_from_file filenameからの相対パスとして扱う
	*/
	bool load_image_dataset(const std::string& filename,
		std::vector<std::string>& image_list,
		std::vector<std::vector<cv::Rect> >& rectangles, 
		std::vector<std::vector<cvsr::shape_landmarks> >& landmarks,
		bool relative_path_from_file = false);


	//! XMLファイルへ学習データ/テストデータを保存
	/*!
	\param[in] filename XMLファイル
	\param[in] image_list 画像ファイル
	\param[in] rectangles 矩形
	\param[in] landmarks 特徴点
	*/
	bool save_image_dataset(const std::string& filename,
		const std::string& title,
		const std::vector<std::string>& image_list,
		const std::vector<std::vector<cv::Rect> >& rectangles,
		const std::vector<std::vector<cvsr::shape_landmarks> >& landmarks,
		const std::string& comment = std::string());
}

#endif
