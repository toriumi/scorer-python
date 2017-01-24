#ifndef __LICENSE_PLATE_INFO__
#define __LICENSE_PLATE_INFO__

#include <opencv2/core.hpp>

namespace lpr{

//! ナンバープレート形状の事前知識
class LicensePlateInfo
{
private:
	int _Type;
	cv::Rect2f _PlatePosition;	//!< ナンバープレートの位置
	cv::Rect2f _UpperCharRegion; //!< 地名＋分類番号領域
	cv::Rect2f _LowerCharRegion; //!< ひらがな＋車両番号領域
	cv::Rect2f _LocalNameRegion;	//!< 地名領域
	cv::Rect2f _ClassNumberRegion;	//!< 分離番号領域
	cv::Rect2f _HiraganaRegionMax;	//!< ひらがな領域の最小サイズ 
	cv::Rect2f _HiraganaRegionMin;	//!< ひらがな領域の最大サイズ
	cv::Rect2f _CarNumberRegion;	//!< 車両番号領域
	std::vector<cv::Rect2f> _CarNumberChars;	//!< 車両番号各文字の領域（４文字分）
	cv::Rect2f _Hyphen;
	cv::Rect2f _CloutNailTopLeft;	//!< 左上の鋲
	cv::Rect2f _CloutNailTopRight;	//!< 右上の鋲
	float _ClassNumFontWidth;	//!< 分類番号のフォントの線幅
	float _CarNumFontWidth;		//!< 車両番号のフォントの線幅
	

public:

	const static int MIDDLEPLATE = 0;
	const static int LARGEPLATE_SHORT_LOCALNAME = 1;
	const static int LARGEPLATE_LONG_LOCALNAME = 2;
	const static int LARGEPLATE_SHORT_CLASSNUMBER = 3;

	LicensePlateInfo(int type = MIDDLEPLATE) {
		Initialize(type);
	}

	LicensePlateInfo(const cv::Rect2f& plate_position, int type = MIDDLEPLATE) {
		Initialize(plate_position, type);
	}

	LicensePlateInfo(const cv::Size2f& plate_size, int type = MIDDLEPLATE) {
		Initialize(plate_size, type);
	}

	//! 初期パラメータ割り当て
	void Initialize(int type) {
		_Type = type;
		if (type == LARGEPLATE_LONG_LOCALNAME ||
			type == LARGEPLATE_SHORT_CLASSNUMBER ||
			type == LARGEPLATE_SHORT_LOCALNAME)
			InitializeL(type);
		else
			Initialize();
	}


	//! 初期パラメータ割り当て（中板）
	void Initialize() {
		_PlatePosition = cv::Rect2f(0, 0, 500, 250);
		_UpperCharRegion = cv::Rect2f(105,13,294,77);
		_LowerCharRegion = cv::Rect2f(25,100,455,135);
		_LocalNameRegion = cv::Rect2f(105, 13, 162, 77);
		_ClassNumberRegion = cv::Rect2f(267, 13, 132, 77);
		_HiraganaRegionMax = cv::Rect2f(25, 133, 70, 70);
		_HiraganaRegionMin = cv::Rect2f(39, 133, 42, 70);
		_CarNumberRegion = cv::Rect2f(105, 100, 375, 135);
		_Hyphen = cv::Rect2f(267, 158, 51, 18);
		_ClassNumFontWidth = 9;
		_CarNumFontWidth = 18;
		_CarNumberChars.resize(4);
		_CarNumberChars[0] = cv::Rect2f(105, 100, 81, 135);
		_CarNumberChars[1] = cv::Rect2f(186, 100, 81, 135);
		_CarNumberChars[2] = cv::Rect2f(318, 100, 81, 135);
		_CarNumberChars[3] = cv::Rect2f(399, 100, 81, 135);
		_CloutNailTopLeft = cv::Rect2f(55,13,50,50);
		_CloutNailTopRight = cv::Rect2f(399, 13, 38, 38);
	};


	//! 初期パラメータ割り当て（大板）
	void InitializeL(int type) {
		_PlatePosition = cv::Rect2f(0, 0, 500, 250);
		_UpperCharRegion = cv::Rect2f(100, 19, 280, 55);
		_LowerCharRegion = cv::Rect2f(25, 81, 459, 150);

		// 地名文字数>=分類番号文字数
		if (type == LARGEPLATE_LONG_LOCALNAME) {
			_LocalNameRegion = cv::Rect2f(100, 19, 160, 55);
			_ClassNumberRegion = cv::Rect2f(260, 19, 120, 55);
		}
		// 地名文字数>=3 && 分類番号文字数<=2
		else if (type == LARGEPLATE_SHORT_CLASSNUMBER) {
			_LocalNameRegion = cv::Rect2f(100, 19, 185, 55);
			_ClassNumberRegion = cv::Rect2f(285, 19, 95, 55);
		}
		// 地名文字数 < 分類番号文字数
		else if(type == LARGEPLATE_SHORT_LOCALNAME){
			_LocalNameRegion = cv::Rect2f(100, 19, 133, 55);
			_ClassNumberRegion = cv::Rect2f(233, 19, 147, 55);
		}
		_HiraganaRegionMax = cv::Rect2f(25, 125, 57, 57);
		_HiraganaRegionMin = cv::Rect2f(36, 125, 35, 67);
		_CarNumberRegion = cv::Rect2f(86, 81, 398, 150);
		_Hyphen = cv::Rect2f(262, 145, 46, 22);
		_ClassNumFontWidth = 13;
		_CarNumFontWidth = 22;
		_CarNumberChars.resize(4);
		_CarNumberChars[0] = cv::Rect2f(86, 81, 88, 150);
		_CarNumberChars[1] = cv::Rect2f(174, 81, 88, 150);
		_CarNumberChars[2] = cv::Rect2f(308, 81, 88, 150);
		_CarNumberChars[3] = cv::Rect2f(396, 81, 88, 150);
		_CloutNailTopLeft = cv::Rect2f(50, 19, 50, 50);
		_CloutNailTopRight = cv::Rect2f(380, 19, 38, 38);
	};

	void Initialize(const cv::Rect2f& plate_position, int type);

	void Initialize(const cv::Size2f& plate_size, int type) {
		Initialize(cv::Rect2f(0, 0, plate_size.width, plate_size.height), type);
	}

	int Type() const {
		return _Type;
	}

	//! ナンバープレートのサイズ
	cv::Size2f PlateSize() const {
		return _PlatePosition.size();
	};	

	//! ナンバープレートの位置
	cv::Rect2f PlatePosition() const {
		return _PlatePosition;
	};

	//! 地名領域
	cv::Rect2f LocalNameRegion() const {
		return _LocalNameRegion;
	}

	//! 分離番号領域
	cv::Rect2f ClassNumberRegion() const {
		return _ClassNumberRegion;
	}

	//! ひらがな領域の最小サイズ
	cv::Rect2f HiraganaRegionMax() const {
		return _HiraganaRegionMax;
	}

	//! ひらがな領域の最大サイズ
	cv::Rect2f HiraganaRegionMin() const{
		return _HiraganaRegionMin;
	}
	
	//! 車両番号領域
	cv::Rect2f CarNumberRegion() const {
		return _CarNumberRegion;
	}

	//! 車両番号各文字の領域（４文字分）
	std::vector<cv::Rect2f> CarNumberChars() const {
		return _CarNumberChars;
	}

	//! 上文字列領域（地名＋分類番号）
	cv::Rect2f UpperCharRegion() const {
		return _UpperCharRegion;
	}

	//! 下文字列領域（ひらがな＋車両番号）
	cv::Rect2f LowerCharRegion() const {
		return _LowerCharRegion;
	}

	//! ハイフン
	cv::Rect2f Hyphen() const {
		return _Hyphen;
	}

	//! 左上鋲
	cv::Rect2f CloutNailTopLeft() const {
		return _CloutNailTopLeft;
	}

	//! 右上鋲
	cv::Rect2f CloutNailTopRight() const {
		return _CloutNailTopRight;
	}

	//! 分類番号のフォントの線幅
	float ClassNumFontWidth() const{
		return _ClassNumFontWidth;
	}

	//! 車両番号のフォントの線幅
	float CarNumFontWidth() const {
		return _CarNumFontWidth;
	}

	//! 地名の各文字領域を算出
	/*!
	\param[in] num_char 地名文字数
	*/
	std::vector<cv::Rect2f> LocalNameChars(int num_char) const;

	//! 地名のとりうる各文字領域を算出
	std::vector<std::vector<cv::Rect2f> > LocalNameCharsAllPatterns() const;

	//! 分類番号の各文字領域を算出
	/*!
	\param[in] num_char 分類番号桁数
	*/
	std::vector<cv::Rect2f> ClassNumberChars(int num_char) const;

	//! 分類番号のとりうる各文字領域を算出
	std::vector<std::vector<cv::Rect2f> > ClassNumberCharsAllPatterns() const;

	//! 矩形の座標変換
	/*!
	anchor_rectに対するsrc_rectsの相対位置を保存するように、anchor_rectをtransformed_anchorへ変換した時のsrc_rectsの位置をdst_rectsとして求める。
	*/
	static void TransformRectPosition(const cv::Rect2f& anchor_rect, const std::vector<cv::Rect2f>& src_rects,
		const cv::Rect2f& transformed_anchor, std::vector<cv::Rect2f>& dst_rects);

	static void TransformRectPosition(const cv::Rect2f& anchor_rect, const cv::Rect2f& src_rect,
		const cv::Rect2f& transformed_anchor, cv::Rect2f& dst_rect);

};

}
#endif