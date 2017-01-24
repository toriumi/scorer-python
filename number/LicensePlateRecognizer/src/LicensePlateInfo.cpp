#include "LicensePlateInfo.h"

namespace lpr {
	void LicensePlateInfo::Initialize(const cv::Rect2f& plate_position, int type) {
		assert(plate_position.width > 0 && plate_position.height > 0);

		Initialize(type);
		std::vector<cv::Rect2f> src_rects = _CarNumberChars;
		src_rects.push_back(_LocalNameRegion);
		src_rects.push_back(_ClassNumberRegion);
		src_rects.push_back(_HiraganaRegionMax);
		src_rects.push_back(_HiraganaRegionMin);
		src_rects.push_back(_CarNumberRegion);
		src_rects.push_back(_Hyphen);
		src_rects.push_back(_CloutNailTopLeft);
		src_rects.push_back(_CloutNailTopRight);
		src_rects.push_back(_UpperCharRegion);
		src_rects.push_back(_LowerCharRegion);
		std::vector<cv::Rect2f> dst_rects;
		TransformRectPosition(_PlatePosition, src_rects, plate_position, dst_rects);

		_PlatePosition = plate_position;
		for (int i = 0;i < 4;i++)
			_CarNumberChars[i] = dst_rects[i];
		_LocalNameRegion = dst_rects[4];
		_ClassNumberRegion = dst_rects[5];
		_HiraganaRegionMax = dst_rects[6];
		_HiraganaRegionMin = dst_rects[7];
		_CarNumberRegion = dst_rects[8];
		_Hyphen = dst_rects[9];
		_CloutNailTopLeft = dst_rects[10];
		_CloutNailTopRight = dst_rects[11];
		_UpperCharRegion = dst_rects[12];
		_LowerCharRegion = dst_rects[13];

		cv::Size2f scale(plate_position.width / _PlatePosition.width, plate_position.height / _PlatePosition.height);

		float avg_scale = (scale.width + scale.height) / 2.0;
		_ClassNumFontWidth *= avg_scale;
		_CarNumFontWidth *= avg_scale;
	}



	//! 地名の各文字領域を算出
	/*!
	\param[in] num_char 地名文字数
	*/
	std::vector<cv::Rect2f> LicensePlateInfo::LocalNameChars(int num_char) const
	{
		std::vector<cv::Rect_<float>> CharRects;
		if (num_char == 1) {
			float s = _LocalNameRegion.height;
			float x = _LocalNameRegion.x + (_LocalNameRegion.width - s) / 2;
			CharRects.push_back(cv::Rect_<float>(x, _LocalNameRegion.y, s, s));
		}
		else if (num_char == 2) {
			float s = _LocalNameRegion.height;
			cv::Rect2f first_rect(_LocalNameRegion.x, _LocalNameRegion.y, s, s);
			cv::Rect2f second_rect(first_rect.x + first_rect.width, first_rect.y, s, s);
			CharRects.push_back(first_rect);
			CharRects.push_back(second_rect);
		}
		else if (num_char == 3 || num_char == 4) {
			float w = _LocalNameRegion.width / num_char;
			float h = _LocalNameRegion.height;
			float x = _LocalNameRegion.x;
			float y = _LocalNameRegion.y;
			for (int i = 0; i<num_char; i++) {
				CharRects.push_back(cv::Rect_<float>(x, y, w, h));
				x += w;
			}
		}
		return CharRects;
	}


	//! 分類番号の各文字領域を算出
	/*!
	\param[in] num_char 分類番号桁数
	*/
	std::vector<cv::Rect2f> LicensePlateInfo::ClassNumberChars(int num_char) const
	{
		std::vector<cv::Rect2f> CharRects;
		if (num_char == 1) {
			float s = _ClassNumberRegion.height;
			float x = _ClassNumberRegion.x + (_ClassNumberRegion.width - s) / 2;
			CharRects.push_back(cv::Rect2f(x, _ClassNumberRegion.y, s, s));
		}
		else if (num_char == 2 || num_char == 3) {
			float w = _ClassNumberRegion.width / num_char;
			float h = _ClassNumberRegion.height;
			float x = _ClassNumberRegion.x;
			float y = _ClassNumberRegion.y;
			for (int i = 0; i<num_char; i++) {
				CharRects.push_back(cv::Rect_<float>(x, y, w, h));
				x += w;
			}
		}
		return CharRects;
	}


	//! 分類番号のとりうる各文字領域を算出
	std::vector<std::vector<cv::Rect2f> > LicensePlateInfo::ClassNumberCharsAllPatterns() const
	{
		std::vector<std::vector<cv::Rect2f> > ret;
		if (_Type == MIDDLEPLATE || _Type == LARGEPLATE_LONG_LOCALNAME) {
			for (int i = 1;i <= 3;i++) {
				ret.push_back(ClassNumberChars(i));
			}
		}
		else if (_Type == LARGEPLATE_SHORT_LOCALNAME) {
			ret.push_back(ClassNumberChars(2));
			ret.push_back(ClassNumberChars(3));
		}
		else if (_Type == LARGEPLATE_SHORT_CLASSNUMBER) {
			ret.push_back(ClassNumberChars(1));
			ret.push_back(ClassNumberChars(2));
		}
		//for (int i = 1;i <= 3;i++) {
		//	ret.push_back(ClassNumberChars(i));
		//}
		return ret;
	}


	//! 地名のとりうる各文字領域を算出
	std::vector<std::vector<cv::Rect2f> > LicensePlateInfo::LocalNameCharsAllPatterns() const
	{
		std::vector<std::vector<cv::Rect2f> > ret;
		for (int i = 1;i <= 4;i++) {
			ret.push_back(LocalNameChars(i));
		}
		return ret;
	}


	void LicensePlateInfo::TransformRectPosition(const cv::Rect2f& anchor_rect, const std::vector<cv::Rect2f>& src_rects,
		const cv::Rect2f& transformed_anchor, std::vector<cv::Rect2f>& dst_rects)
	{
		cv::Size2f scale(transformed_anchor.width / anchor_rect.width,
			transformed_anchor.height / anchor_rect.height);

		dst_rects.resize(src_rects.size());
		for (int i = 0;i < src_rects.size();i++) {
			dst_rects[i].x = transformed_anchor.x + (src_rects[i].x - anchor_rect.x) * scale.width;
			dst_rects[i].y = transformed_anchor.y + (src_rects[i].y - anchor_rect.y) * scale.height;
			dst_rects[i].width = src_rects[i].width * scale.width;
			dst_rects[i].height = src_rects[i].height * scale.height;
		}
	}


	void LicensePlateInfo::TransformRectPosition(const cv::Rect2f& anchor_rect, const cv::Rect2f& src_rect,
		const cv::Rect2f& transformed_anchor, cv::Rect2f& dst_rect)
	{
		std::vector<cv::Rect2f> src_rects, dst_rects;
		src_rects.push_back(src_rect);
		TransformRectPosition(anchor_rect, src_rects, transformed_anchor, dst_rects);
		dst_rect = dst_rects[0];
	}
}