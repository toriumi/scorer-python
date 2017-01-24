#ifndef __COMMON_FUNCTIONS__
#define __COMMON_FUNCTIONS__

#include <opencv2/core/core.hpp>
#include <fstream>

namespace UTIL_CV{

// 小数点以下四捨五入(HaarS1)
inline int round(double a){return (int)(a+0.5);};

//! Write cv::Mat as binary(HMAX)
/*!
\param[out] ofs output file stream
\param[in] out_mat mat to save
*/
bool writeMatBinary(std::ofstream& ofs, const cv::Mat& out_mat);

//! Read cv::Mat from binary(HMAX)
/*!
\param[in] ifs input file stream
\param[out] in_mat mat to load
*/
bool readMatBinary(std::ifstream& ifs, cv::Mat& in_mat);

//! 配列の８近傍で極大値と棚を取る点を求める。(HaarS1)
/*!
\param[in] src_mat 入力（CV_32FC1）
\param[out] dst_mat 出力：二値(CV_8UC1)
\param[in] threshold 閾値
*/
void ExtractMaximaAndShelf(const cv::Mat& src_mat, cv::Mat& dst_mat, float threshold = 0.0);

//! 二値画像中のID番目の白画素の位置を返す
cv::Point PositionOfPositiveElementID(const cv::Mat& bin_img, int id);

//! 二値画像中のランダムな白画素の位置を返す(S2)
cv::Point RandomPositiveElement(const cv::Mat& bin_img);

//! 指定した領域を切り取り(S2)
std::vector<cv::Mat> CutMats(const std::vector<cv::Mat>& c1_inputs, const cv::Rect& region);


//! 指定サイズをはみ出る点をサイズ内近傍点に近似(HaarS1)
/*!
\param[in] size 指定サイズ
\param pt もしサイズからはみ出る場合、近傍点に近似される
*/
void truncatePoint(const cv::Size& size, cv::Point& pt);

//! 画像の中心を算出して、angleだけ回転させる。(HaarS1)
/*!
\param[in] src_mat 入力画像
\param[out] dst_mat 出力画像。emptyの時は回転した画像が全て収まるように自動調整される。emptyで無い時は画像の中心位置が合うように。
\param[out] src_matからdst_matを作成するためのアフィン変換行列
\param[in] angle 回転角度（時計回り）
\param[in] dst_size 出力画像のサイズ
*/
void RotateMat(const cv::Mat& src_mat, cv::Mat& dst_mat, cv::Mat& AffineMat, double angle, const cv::Size& dst_size = cv::Size(0,0));

//! 矩形の四隅の座標をcv::Transform()用Mat型へ変更(HaarS1)
std::vector<cv::Point2d> RectToPoints(const cv::Rect& src_rect);

//! 矩形を塗りつぶしたマスクを作成(HaarS1)
cv::Mat createMask(const cv::Size& img_size, const std::vector<cv::Point2d>& pts);

//! サイズ変更(HaarS1)
void resize(const std::vector<cv::Mat>& src_array, std::vector<cv::Mat>& dst_array, cv::Size dsize, double fx = 0.0, double fy = 0.0, int interpolation = 1);


};

#endif
