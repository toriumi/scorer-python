#ifndef __PLATE_SEGMENTATION__
#define __PLATE_SEGMENTATION__

#include <opencv2/core.hpp>
#include "LicensePlateInfo.h"

namespace lpr{

class PlateSegmentation
{
public:
	PlateSegmentation(void);
	~PlateSegmentation(void);

	//! パラメータ設定
	/*!
	¥param[in] proc_rect_width 入力矩形の幅を処理時にこの長さに正規化する。0の時オリジナルサイズで処理
	*/
	void SetProcPlateWidth(int proc_rect_width){
		_proc_width = proc_rect_width;
	};


	//! 画像からナンバープレート文字領域を抽出
	/*!
	¥param[in] img 入力画像
	¥param[in] plate_region ナンバープレート領域
	¥param[out] local_name 地名領域
	¥param[out] class_number 分類番号領域
	¥param[out] hiragana ひらがな領域
	¥param[out] car_number 車両番号領域
	¥param[in] plate_corners ナンバープレートの四隅（左上から時計回り）
	*/
	void ExtractCharRegions(const cv::Mat& img,
		cv::Rect& local_name, std::vector<cv::Rect>& class_number,
		cv::Rect& hiragana, std::vector<cv::Rect>& car_number, bool& large_plate) const;

private:
	///////// ExtractCharRegions //////////////
	int _proc_width;	// 入力矩形をこのサイズに正規化する。

	//! Haar特徴を用いた文字列（ひらがな＋車両番号、または地名＋分類番号）の探索
	/*!
	¥param[in] integ 積分画像
	¥param[out] found 出力矩形
	¥param[in] initial 初期位置
	¥param[in] margin 矩形の上下に持たせるマージン。この領域の画素の平均と矩形の画素の平均の差を用いる
	¥param[in] search_range 探索範囲
	*/
	static float SearchVerticalRegion(const cv::Mat& integ, cv::Rect2f& found,
		const cv::Rect2f& initial, int margin, int search_range);

	//! 矩形の両端の画素のうち矩形内の画素平均より大きい数をカウント
	/*!
	¥param[in] src 入力グレースケール画像
	¥param[in] integ 積分画像
	¥param[in] rects 矩形群
	¥param[in] weight 各矩形への重み
	¥param[in] left 左側の何列の画素をカウントするか
	¥param[in] right 右側の何列の画素をカウントするか
	*/
	static float EvalCharRegionSide(const cv::Mat& src, const cv::Mat& integ, const std::vector<cv::Rect2f>& rects, const std::vector<float>& weight,
		const std::vector<int>& left = std::vector<int>(), const std::vector<int>& right = std::vector<int>());

	static float EvalCharRegionSide(const cv::Mat& src, const cv::Mat& integ, const cv::Rect2f& src_rect, int left = 1, int right = 1);

	//! 矩形の上下端の画素のうち矩形内の画素平均より大きい数をカウント
	/*!
	¥param[in] src 入力グレースケール画像
	¥param[in] integ 積分画像
	¥param[in] src_rect 入力矩形
	¥param[in] top 上側の何列の画素をカウントするか
	¥param[in] bottom 下側の何列の画素をカウントするか
	*/
	static float EvalCharRegionVer(const cv::Mat& src, const cv::Mat& integ, const cv::Rect2f& src_rect, int top = 1, int bottom = 1);

	//static void EvalStringRegionsSide(const cv::Mat& src, const cv::Mat& integ, std::vector<float>& signal,
	//	const cv::Rect2f& string_region, int char_size, bool left, bool right);

	//! Haar特徴による矩形評価
	/*!
	¥param[in] integ 積分画像
	¥param[in] in_rects 負の矩形
	¥param[in] out_rects 正の矩形
	¥return 評価値
	*/
	static float EvalCharRegion(const cv::Mat& integ,
		const cv::Rect2f& src_in_rect, const cv::Rect2f& src_out_rect);

	//! 矩形の集合を横方向に伸縮かつ移動しながら最も文字列と当てはまる領域を探索
	/*!
	¥param[in] src 入力グレースケール画像
	¥param[in] integ 積分画像
	¥param[in] anchor_rect ref_rectsの参照となる矩形。anchor_rectを伸縮変形しながら、ref_rectsをそれに合わせて変形する。
	¥param[in] init_rect 初期anchor_rect位置
	¥param[out] dst_rects 出力矩形
	¥param[in] left_search_range_in 左内側方向の探索範囲
	¥param[in] left_search_range_out 左外側方向の探索範囲
	¥param[in] right_search_range_in 右内側方向の探索範囲
	¥param[in] right_search_range_out 右外側方向の探索範囲
	¥param[in] delta 探索ステップ幅
	¥param[in] weight 各矩形の重み
	¥param[in] lambda 初期位置からのずれに対するペナルティ
	*/
	static float FitMultiRectHorizontal(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& anchor_rect, const std::vector<cv::Rect2f>& ref_rects,
		const cv::Rect2f& init_rect, std::vector<cv::Rect2f>& dst_rects,
		int left_search_range_in, int left_search_range_out,
		int right_search_range_in, int right_search_range_out,
		int delta,
		const std::vector<float>& weight = std::vector<float>(), float lambda = 0.0);

	// 位置の横方向の微調整
	static float FitRectHorizontal(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& ref_rect, cv::Rect2f& dst_rect,
		int left_search_range, int right_search_range,
		int left = 1, int right = 1);

	static float FitRectVertical(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& ref_rect, cv::Rect2f& dst_rect,
		int top_search_range, int bottom_search_range,
		int top = 1, int bottom = 1);

	//! 地名＋分類番号のFitting
	/*!
	¥param[in] src 入力グレースケール画像
	¥param[in] inten 積分画像
	¥param[in] init_upper_region 初期位置
	¥param[in] plate_info ナンバープレートのモデル
	¥param[out] local_name 地名位置
	¥param[out] class_number 分類番号
	¥param[in] lambda 評価値（文字領域内の画素値平均とそれ以外の画素値平均の差）への重み
	¥param[in] lambda2 正則化項（分類番号の桁数）に対する重み
	*/
	static float DetectLocalNameAndClassNumberRegions(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& init_upper_region, const LicensePlateInfo& plate_info,
		cv::Rect2f& local_name, std::vector<cv::Rect2f>& class_numbers,
		float lambda = 1.0, float lambda2 = 0.5);

	static float ClassNumberRegularizationCost(int num_chars);

	//! 文字領域の画素の平均とその周辺の画素の平均の差分算出
	static float DiffCharRegionAndMargin(const cv::Mat& integ, const cv::Rect2f& plate_rect,
		const std::vector<cv::Rect2f>& local_name, const std::vector<cv::Rect2f>& class_numbers);

	//! 積分画像を用いて矩形内の画素の総和を算出
	static int RectSum(const cv::Mat& integ, const cv::Rect& rect) {
		return integ.at<int>(rect.y + rect.height, rect.x + rect.width)
			- integ.at<int>(rect.y + rect.height, rect.x)
			- integ.at<int>(rect.y, rect.x + rect.width)
			+ integ.at<int>(rect.y, rect.x);
	}

};

}

#endif
