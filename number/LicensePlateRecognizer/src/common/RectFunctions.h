#ifndef __RECT_FUNCTIONS__
#define __RECT_FUNCTIONS__

#include <opencv2/core/core.hpp>

//! 画像領域からはみ出した矩形をカット
/*!
\param[in] obj_rect 入力矩形
\param[in] img_size 画像サイズ
\retrun 切り取られた矩形
*/
cv::Rect TruncateRect(const cv::Rect& obj_rect, const cv::Size& img_size);

//! 矩形を拡縮する
/*!
中心位置は固定したまま、矩形の幅または高さを変更する。
\param[in] src_rect 入力矩形
\param[in] rescale 幅、高さの拡縮スケール
*/
cv::Rect RescaleRectSize(const cv::Rect& src_rect, const cv::Size2f& rescale);

//! 矩形の四隅座標を取得
/*!
左上の座標から時計回りに
\param[in] src_rect 入力矩形
\param[out] dst_corners 出力コーナー
*/
template <typename T>
void Rect2Corners(const cv::Rect_<T>& src_rect, std::vector<cv::Point_<T> >& dst_corners)
{
	dst_corners.clear();
	dst_corners.push_back(cv::Point_<T>(src_rect.x, src_rect.y));
	dst_corners.push_back(cv::Point_<T>(src_rect.x + src_rect.width, src_rect.y));
	dst_corners.push_back(cv::Point_<T>(src_rect.x + src_rect.width, src_rect.y + src_rect.height));
	dst_corners.push_back(cv::Point_<T>(src_rect.x, src_rect.y + src_rect.height));
}

//! 矩形を拡縮する
/*!
x, y, width, height全てをリスケール
\param[in] src_rect 入力矩形
\param[in] dst_rect 出力矩形
\param[in] scale 幅、高さの拡縮スケール
*/
template <typename T> 
void RescaleRect(const cv::Rect_<T>& src_rect, cv::Rect_<T>& dst_rect, float scale){
	dst_rect = cv::Rect_<T>(scale * src_rect.x, scale * src_rect.y, scale * src_rect.width, scale * src_rect.height);
};

template <typename T>
void RescaleRect(const cv::Rect_<T>& src_rect, cv::Rect_<T>& dst_rect, const cv::Size2f& scale) {
	dst_rect = cv::Rect_<T>(src_rect.x * scale.width, src_rect.y * scale.height,
		src_rect.width * scale.width, src_rect.height * scale.height);
};

template <typename T>
void RescaleRect(const std::vector<cv::Rect_<T> >& src_rects, std::vector<cv::Rect_<T> >& dst_rects, float scale){
	dst_rects.clear();
	typename std::vector<cv::Rect_<T> >::const_iterator it, it_e = src_rects.end();
	for(it = src_rects.begin(); it != it_e; it++){
		cv::Rect_<T> rect;
		RescaleRect(*it, rect, scale);
		dst_rects.push_back(rect);
	}
};


template <typename T>
void RescaleRect(const std::vector<cv::Rect_<T> >& src_rects, std::vector<cv::Rect_<T> >& dst_rects, const cv::Size2f scale) {
	dst_rects.clear();
	typename std::vector<cv::Rect_<T> >::const_iterator it, it_e = src_rects.end();
	for (it = src_rects.begin(); it != it_e; it++) {
		cv::Rect_<T> rect;
		RescaleRect(*it, rect, scale);
		dst_rects.push_back(rect);
	}
};

// 矩形の移動
template <typename T>
void MoveRect(const cv::Rect_<T>& src_rect, cv::Rect_<T>& dst_rect, const cv::Point_<T>& mv){
	dst_rect = src_rect;
	dst_rect.x += mv.x;
	dst_rect.y += mv.y;
}

template <typename T>
void MoveRect(const std::vector<cv::Rect_<T> >& src_rects, std::vector<cv::Rect_<T> >& dst_rects, const cv::Point_<T>& mv){
	dst_rects.clear();
	typename std::vector<cv::Rect_<T> >::const_iterator it, it_e = src_rects.end();
	for(it = src_rects.begin(); it != it_e; it++){
		cv::Rect_<T> rect;
		MoveRect(*it, rect, mv);
		dst_rects.push_back(rect);
	}
}


template <typename T>
void MoveRect(const std::vector<std::vector<cv::Rect_<T>>>& src_rects, std::vector<std::vector<cv::Rect_<T>>>& dst_rects, const cv::Point_<T>& mv){
	dst_rects.clear();
	typename std::vector<std::vector<cv::Rect_<T> > >::const_iterator it, it_e = src_rects.end();
	for(it = src_rects.begin(); it != it_e; it++){
		std::vector<cv::Rect_<T> > rects;
		MoveRect(*it, rects, mv);
		dst_rects.push_back(rects);
	}
}


//! ２つの矩形の外接矩形を算出
template <typename T>
cv::Rect_<T> CombineRect(const cv::Rect_<T>& a, const cv::Rect_<T>& b){
	T bx = std::min(a.x, b.x);
	T by = std::min(a.y, b.y);
	T ex = std::max(a.x + a.width, b.x + b.width);
	T ey = std::max(a.y + a.height, b.y + b.height);
	return cv::Rect_<T>(bx, by, ex-bx, ey-by);
}


//! 矩形の外接矩形を算出
template <typename T>
cv::Rect_<T> CombineRect(const std::vector<cv::Rect_<T>>& a) {
	if (a.empty())
		return cv::Rect_<T>(0, 0, 0, 0);
	cv::Rect_<T> tmp = a[0];
	for (int i = 1;i < a.size(); i++) {
		tmp = CombineRect(tmp, a[i]);
	}
	return tmp;
}


template <typename T>
bool isInsideRect(const cv::Rect_<T>& inside, const cv::Rect_<T>& outside) {
	return (inside.x >= outside.x && inside.y >= outside.y &&
		inside.x + inside.width <= outside.x + outside.width &&
		inside.y + inside.height <= outside.y + outside.height);
}

#endif