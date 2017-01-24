#include "PlateSegmentation.h"
#include <opencv2/imgproc.hpp>
#include "common/argsort.hpp"
#include "common/RectFunctions.h"

#define __DEBUG__ 0

#if __DEBUG__
#include <opencv2/highgui.hpp>
#include "util_cv.h"
#endif

namespace lpr {

	PlateSegmentation::PlateSegmentation(void)
	{
		_proc_width = 240;
	}


	PlateSegmentation::~PlateSegmentation(void)
	{
	}



	float PlateSegmentation::EvalCharRegionSide(const cv::Mat& src, const cv::Mat& integ, const cv::Rect2f& src_rect, int left, int right)
	{
		cv::Rect rect = TruncateRect(src_rect, src.size());
		int rect_sum = RectSum(integ, cv::Rect(rect.x + 1, rect.y + 1, rect.width - 2, rect.height - 2));
		float avg = (float)rect_sum / ((rect.width - 2) * (rect.height - 2));

		int count = 0;
		int sum_pix = 0;
		int xe = rect.x + rect.width - 1;
		int ye = rect.y + rect.height - 1;
		for (int y = rect.y; y <= ye; y++) {
			for (int x = 0; x < left; x++) {
				if (rect.x - x < 0)
					break;
				sum_pix++;
				if (avg > src.at<uchar>(y, rect.x - x))
					count++;
			}
			for (int x = 0; x < right; x++) {
				if (xe + x >= src.cols)
					break;
				sum_pix++;
				if (avg > src.at<uchar>(y, xe + x))
					count++;
			}
		}
		if (sum_pix == 0)
			return 1.0;

		float ret = (float)count / sum_pix;

		return ret;
	}



	float PlateSegmentation::EvalCharRegionVer(const cv::Mat& src, const cv::Mat& integ, const cv::Rect2f& src_rect, int top, int bottom)
	{
		cv::Rect rect = TruncateRect(src_rect, src.size());
		int rect_sum = RectSum(integ, cv::Rect(rect.x + 1, rect.y + 1, rect.width - 2, rect.height - 2));
		float avg = (float)rect_sum / ((rect.width - 2) * (rect.height - 2));

		int count = 0;
		int sum_pix = 0;
		int xe = rect.x + rect.width - 1;
		int ye = rect.y + rect.height - 1;
		for (int x = rect.x; x <= xe; x++) {
			for (int y = 0; y < top; y++) {
				if (rect.y - y < 0)
					break;
				sum_pix++;
				if (avg > src.at<uchar>(rect.y - y, x))
					count++;
			}
			for (int y = 0; y < bottom; y++) {
				if (ye + y >= src.rows)
					break;
				sum_pix++;
				if (avg > src.at<uchar>(ye + y, x))
					count++;
			}
		}
		if (sum_pix == 0)
			return 1.0;

		float ret = (float)count / sum_pix;

		return ret;
	}


	
	float PlateSegmentation::EvalCharRegion(const cv::Mat& integ,
		const cv::Rect2f& src_in_rect, const cv::Rect2f& src_out_rect) {
		cv::Size trunc_size(integ.cols - 1, integ.rows - 1);
		cv::Rect2f in_rect = TruncateRect(src_in_rect, trunc_size);
		cv::Rect2f out_rect = TruncateRect(src_out_rect, trunc_size);
		int in_rect_sum = RectSum(integ, in_rect);
		int out_rect_sum = RectSum(integ, out_rect);
		int in_area = src_in_rect.width * src_in_rect.height;
		int out_area = src_out_rect.width * src_out_rect.height;
		//int in_area = in_rect.width * in_rect.height;
		//int out_area = out_rect.width * out_rect.height;
		float in_avg = (float)in_rect_sum / in_area;
		float out_avg = (float)(out_rect_sum - in_rect_sum) / (out_area - in_area);
		//printf("%f,%f\n", in_avg, out_avg);
		return (in_avg - out_avg) / 255;
	}


	float PlateSegmentation::EvalCharRegionSide(const cv::Mat& src, const cv::Mat& integ, const std::vector<cv::Rect2f>& rects, const std::vector<float>& weight,
		const std::vector<int>& left, const std::vector<int>& right) {
		assert(weight.size() == rects.size() || weight.empty());
		assert(left.size() == rects.size() || left.empty());
		assert(right.size() == rects.size() || right.empty());
		float score = 0;
		float weight_sum = 0;
		for (int i = 0;i < rects.size();i++) {
			float w = (weight.empty()) ? 1.0 : weight[i];
			int l = (left.empty()) ? 1 : left[i];
			int r = (right.empty()) ? 1 : right[i];
			score += EvalCharRegionSide(src, integ, rects[i], l, r) * w;
			weight_sum += w;
		}
		score /= weight_sum;
		return score;
	}


	float PlateSegmentation::SearchVerticalRegion(const cv::Mat& integ, cv::Rect2f& found,
		const cv::Rect2f& initial, int margin, int search_range)
	{
		int height_margin = 2 * margin;

		// Topをサーチ
		found = initial;
		cv::Rect2f search = found;
		cv::Rect2f out_rect = search;
		float min_val = 1.0;
		int min_y = search_range + 1;
		for (int y = -search_range; y <= search_range;y++) {
			search.y = found.y + y;
			search.height = found.height - y;
			out_rect.y = search.y - margin;
			out_rect.height = search.height + height_margin;
			if (search.y < 1)
				continue;
			//printf("%d,", y);
			float val = EvalCharRegion(integ, search, out_rect);
			if (min_val > val) {
				min_val = val;
				min_y = y;
			}
			else if (min_val == val) {
				if (std::abs(min_y) > std::abs(y))
					min_y = y;
			}
		}
		found.y += min_y;
		found.height -= min_y;

		// Bottomをサーチ
		search = found;
		out_rect = search;
		out_rect.y = found.y - margin;
		min_y = search_range + 1;
		for (int y = -search_range; y <= search_range;y++) {
			search.height = found.height + y;
			out_rect.height = search.height + height_margin;
			if (search.y + search.height >= integ.rows)
				continue;
			float val = EvalCharRegion(integ, search, out_rect);
			if (min_val > val) {
				min_val = val;
				min_y = y;
			}
			else if (min_val == val) {
				if (std::abs(min_y) > std::abs(y))
					min_y = y;
			}
		}
		found.height += min_y;

		return min_val;
	}

	// 位置の横方向の微調整
	float PlateSegmentation::FitRectHorizontal(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& ref_rect, cv::Rect2f& dst_rect,
		int left_search_range, int right_search_range,
		int left, int right)
	{
		float min_val = 1.0;
		if (ref_rect.height <= 0)
			return min_val;

		cv::Rect2f tmp_rect = ref_rect;
		float init_tail = ref_rect.x + ref_rect.width;
		for (int head = -left_search_range; head <= left_search_range; head++) {
			tmp_rect.x = ref_rect.x + head;
			if (tmp_rect.x < 0)
				continue;
			float init_target_rect_width = init_tail - tmp_rect.x;
			for (int tail = -right_search_range; tail <= right_search_range; tail++) {
				tmp_rect.width = init_target_rect_width + tail;
				if (tmp_rect.width <= 0)
					continue;
				float val = EvalCharRegionSide(src, integ, tmp_rect, left, right);
				if (val < min_val ||
					(val == min_val && dst_rect.width > tmp_rect.width)) {
					min_val = val;
					dst_rect = tmp_rect;
				}
			}
		}
		return min_val;
	}

	
	// 位置の縦方向の微調整
	float PlateSegmentation::FitRectVertical(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& ref_rect, cv::Rect2f& dst_rect,
		int top_search_range, int bottom_search_range,
		int top, int bottom)
	{
		float min_val = 1.0;
		if (ref_rect.width <= 0)
			return min_val;

		cv::Rect2f tmp_rect = ref_rect;
		float init_tail = ref_rect.y + ref_rect.height;
		for (int head = -top_search_range; head <= top_search_range; head++) {
			tmp_rect.y = ref_rect.y + head;
			if (tmp_rect.y < 0)
				continue;
			float init_target_rect_height = init_tail - tmp_rect.y;
			for (int tail = -bottom_search_range; tail <= bottom_search_range; tail ++) {
				tmp_rect.height = init_target_rect_height + tail;
				if (tmp_rect.height <= 0)
					continue;
				float val = EvalCharRegionVer(src, integ, tmp_rect, top, bottom);
				if (val < min_val ||
					(val == min_val && dst_rect.height > tmp_rect.height)) {
					min_val = val;
					dst_rect = tmp_rect;
				}
			}
		}
		return min_val;
	}
	


	float PlateSegmentation::FitMultiRectHorizontal(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& anchor_rect, const std::vector<cv::Rect2f>& ref_rects,
		const cv::Rect2f& init_rect, std::vector<cv::Rect2f>& dst_rects,
		int left_search_range_in, int left_search_range_out,
		int right_search_range_in, int right_search_range_out,
		int delta,
		const std::vector<float>& weight, float lambda)
	{
		cv::Rect2f target_rect = init_rect;
		float min_val = 1.0;
		std::vector<cv::Rect2f> tmp_rects;
		std::vector<int> left(ref_rects.size(), 1), right(ref_rects.size(), 1);
		float init_tail = init_rect.x + init_rect.width;
		for (int head = -left_search_range_out; head <= left_search_range_in; head += delta) {
			target_rect.x = init_rect.x + head;
			float init_target_rect_width = init_tail - target_rect.x;
			for (int tail = -right_search_range_in; tail <= right_search_range_out; tail += delta) {
				target_rect.width = init_target_rect_width + tail;
				LicensePlateInfo::TransformRectPosition(anchor_rect, ref_rects, target_rect, tmp_rects);
				float val = EvalCharRegionSide(src, integ, tmp_rects, weight, left, right);
				val += lambda * (head * head + tail * tail) / 4.0;
				//float val = EvalCharRegion(src, integ, tmp_rects, weight);
				if (val < min_val ||
					(val == min_val && !dst_rects.empty()
						&& dst_rects[0].width > tmp_rects[0].width)) {
					min_val = val;
					dst_rects = tmp_rects;
				}
			}
		}
		return min_val;
	}


	//! 文字領域の画素の平均とその周辺の画素の平均の差分算出
	float PlateSegmentation::DiffCharRegionAndMargin(const cv::Mat& integ, const cv::Rect2f& plate_rect,
		const std::vector<cv::Rect2f>& local_name, const std::vector<cv::Rect2f>& class_numbers)
	{
		cv::Rect2f first_local_name_char = local_name[0];
		cv::Rect2f last_local_name_char = local_name.back();
		cv::Rect2f first_class_number = class_numbers[0];
		cv::Rect2f last_class_number = class_numbers.back();

		if (last_local_name_char.x + last_local_name_char.width / 2 >= first_class_number.x ||
			last_local_name_char.x + last_local_name_char.width >= first_class_number.x + first_class_number.width / 2)
			return 0;

		std::vector<cv::Rect> all_char_rects;
		for (int i = 0;i < local_name.size();i++)
			all_char_rects.push_back(local_name[i]);
		for (int i = 0;i < class_numbers.size();i++)
			all_char_rects.push_back(class_numbers[i]);
		for (int i = 1;i < all_char_rects.size();i++) {
			int prev_end = all_char_rects[i - 1].x + all_char_rects[i - 1].width;
			if (prev_end > all_char_rects[i].x) {
				int mid_point = (prev_end + (int)all_char_rects[i].x) / 2;
				all_char_rects[i - 1].width = mid_point - all_char_rects[i - 1].x - 1;
				all_char_rects[i].x = mid_point;
			}
			if (prev_end == all_char_rects[i].x) {
				all_char_rects[i - 1].width--;
			}
		}

		//文字列の左右のマージンを見る
		cv::Rect plate_rect_ext = plate_rect;
		plate_rect_ext.x = (plate_rect.x + first_local_name_char.x) / 2;
		plate_rect_ext.width = (plate_rect.x + plate_rect.width + last_class_number.x + last_class_number.width) / 2 - plate_rect_ext.x;
		if (plate_rect_ext.x > first_local_name_char.x) {
			plate_rect_ext.width += (plate_rect_ext.x - first_local_name_char.x + 1);
			plate_rect_ext.x = local_name[0].x - 1;
		}
		if (plate_rect_ext.x + plate_rect_ext.width < last_class_number.x + last_class_number.width) {
			plate_rect_ext.width = (last_class_number.x + last_class_number.width - plate_rect_ext.x + 1);
		}

		// 文字領域の画素の平均とそれ以外の領域の画素の平均との差
		cv::Rect img_rect(0, 0, integ.cols - 1, integ.rows - 1);
		if (!isInsideRect(plate_rect_ext, img_rect))
			return 0;
		float plate_rect_val = RectSum(integ, plate_rect_ext);
		float plate_area = plate_rect_ext.width * plate_rect_ext.height;
		float sum_char_rects_val = 0;
		float sum_areas = 0;
		//float sum_avg_char_vals = 0;
		for (int i = 0;i < all_char_rects.size();i++) {
			cv::Rect char_rec = all_char_rects[i];
			if (!isInsideRect(char_rec, img_rect))
				return 0;
			float rect_sum = RectSum(integ, char_rec);
			sum_char_rects_val += rect_sum;
			float char_area = char_rec.width * char_rec.height;
			sum_areas += char_area;
			//sum_avg_char_vals += (rect_sum / char_area);
		}

		//float eval_val = (2 * sum_char_rects_val - plate_rect_val) / (plate_rect_ext.width * plate_rect_ext.height);
		//float eval_val = (2 * sum_char_rects_val - plate_rect_val) / (local_name.size() + class_numbers.size());
		plate_rect_val -= sum_char_rects_val;
		plate_rect_val /= (plate_area == sum_areas) ? 1 : (plate_area - sum_areas);
		sum_char_rects_val /= sum_areas;
		//sum_avg_char_vals /= all_char_rects.size();

		float score = plate_rect_val - sum_char_rects_val;
		if (score <= 0)
			return 0;

		//return 2.0 - local_name_cost - class_number_costs + lambda * score;
		return score;
	}


	float PlateSegmentation::ClassNumberRegularizationCost(int num_chars)
	{
		return 1.0 / num_chars;
	}


#if __DEBUG__
	void DebugEval(const cv::Mat& src, const std::vector<float>& comb_signal,
		int signal_x, const cv::Rect2f& init_upper_region,
		const std::vector<cv::Rect2f>& local_name_rects,
		const std::vector<cv::Rect2f>& class_number_rects)
	{
		cv::Mat histogram(1, comb_signal.size(), CV_32FC1);
		for (int i = 0;i < comb_signal.size();i++)
			histogram.at<float>(0, i) = comb_signal[i];
		cv::Mat histimg;
		DrawHistogram(histogram, histimg, 100);

		cv::Mat draw_img = cv::Mat::zeros(histimg.rows, src.cols, CV_8UC1);
		histimg.copyTo(draw_img(cv::Rect(signal_x, 0, histogram.cols, histimg.rows)));

		cv::Mat color_img = ConcatinateImage(src, draw_img, false);
		//cv::Mat color_img;
		//cv::cvtColor(result_img, color_img, cv::COLOR_GRAY2BGR);
		cv::rectangle(color_img, init_upper_region, cv::Scalar(255, 255, 0));
		for (int i = 0;i < local_name_rects.size();i++) {
			cv::rectangle(color_img, local_name_rects[i], cv::Scalar(0, 255, 0));
		}
		for (int j = 0;j < class_number_rects.size();j++) {
			cv::rectangle(color_img, class_number_rects[j], cv::Scalar(255, 0, 0));
		}
		cv::imshow("debug", color_img);
		cv::waitKey();
	}

	void DebugEval(const cv::Mat& src, const cv::Rect2f& init_upper_region,
		const std::vector<cv::Rect2f>& local_name_rects,
		const std::vector<cv::Rect2f>& class_number_rects)
	{
		cv::Mat color_img;
		cv::cvtColor(src, color_img, cv::COLOR_GRAY2BGR);
		cv::rectangle(color_img, init_upper_region, cv::Scalar(255, 255, 0));
		for (int i = 0;i < local_name_rects.size();i++) {
			cv::rectangle(color_img, local_name_rects[i], cv::Scalar(0, 255, 0));
		}
		for (int j = 0;j < class_number_rects.size();j++) {
			cv::rectangle(color_img, class_number_rects[j], cv::Scalar(255, 0, 0));
		}
		cv::imshow("debug", color_img);
		cv::waitKey();
	}
#endif


	float PlateSegmentation::DetectLocalNameAndClassNumberRegions(const cv::Mat& src, const cv::Mat& integ,
		const cv::Rect2f& init_upper_region, const LicensePlateInfo& plate_info,
		cv::Rect2f& local_name, std::vector<cv::Rect2f>& class_numbers,
		float lambda, float lambda2)
	{
		bool large_plate = plate_info.Type() != LicensePlateInfo::MIDDLEPLATE;

		cv::Rect2f upper_rect = plate_info.UpperCharRegion();

		std::vector<std::vector<cv::Rect2f> > class_number_rects = plate_info.ClassNumberCharsAllPatterns();

		std::vector<float> class_number_vals;
		std::vector<std::vector<cv::Rect2f> > tmp_class_number_rects;
		int search_range_in, search_range_out;
		if (large_plate) {
			search_range_in = init_upper_region.height;
			search_range_out = init_upper_region.height / 4;
		}
		else {
			search_range_in = init_upper_region.height / 3;
			search_range_out = init_upper_region.height / 4;
		}


		float position_weight = (large_plate) ? 0.1 / init_upper_region.width : 0;

		for (int i = 0;i < class_number_rects.size();i++) {
			std::vector<cv::Rect2f> upper_regions = class_number_rects[i];
			std::vector<float> weights(upper_regions.size(), 1.0);
			if (!large_plate) {
				upper_regions.push_back(plate_info.CloutNailTopRight());
				weights.push_back(0.1);
			}
			std::vector<cv::Rect2f> tmp_regions;
			float val = FitMultiRectHorizontal(src, integ, upper_rect, upper_regions, init_upper_region, tmp_regions,
				search_range_in, search_range_in, search_range_in, search_range_out, 1, weights);
			class_number_vals.push_back(val);
			tmp_class_number_rects.push_back(tmp_regions);
		}

		std::vector<std::vector<cv::Rect2f> > local_name_rects = plate_info.LocalNameCharsAllPatterns();

		std::vector<float> local_name_vals;
		std::vector<std::vector<cv::Rect2f> > tmp_local_name_rects;
		for (int i = 0;i < local_name_rects.size();i++) {
			std::vector<cv::Rect2f> upper_regions = local_name_rects[i];
			std::vector<float> weights(upper_regions.size(), 1.0);
			if (!large_plate) {
				upper_regions.push_back(plate_info.CloutNailTopLeft());
				weights.push_back(0.1);
			}

			std::vector<cv::Rect2f> tmp_regions;
			float val = FitMultiRectHorizontal(src, integ, upper_rect, upper_regions, init_upper_region, tmp_regions,
				search_range_in, search_range_out, search_range_in, search_range_in, 1, weights, position_weight);
			local_name_vals.push_back(val);
			tmp_local_name_rects.push_back(tmp_regions);
		}
		if (!large_plate) {
			for (int i = 0;i < tmp_class_number_rects.size();i++) {
				tmp_class_number_rects[i].pop_back();
			}
			for (int j = 0;j < tmp_local_name_rects.size(); j++) {
				tmp_local_name_rects[j].pop_back();
			}
		}

		double min_pix, max_pix;
		cv::minMaxIdx(src, &min_pix, &max_pix);
		float pix_scale = max_pix - min_pix;

		float min_val;
		int min_i, min_j;
		for (int i = 0;i < tmp_class_number_rects.size();i++) {
			float reg_cost = lambda2 * ClassNumberRegularizationCost(tmp_class_number_rects[i].size());
			for (int j = 0;j < tmp_local_name_rects.size();j++) {
				float shape_cost = 255.0 - DiffCharRegionAndMargin(integ, init_upper_region, tmp_local_name_rects[j], tmp_class_number_rects[i]);
				shape_cost /= pix_scale;
				float cost = local_name_vals[j] + class_number_vals[i] + lambda * shape_cost + reg_cost;
#if __DEBUG__
				printf("%d x %d, cost: %f\n", tmp_local_name_rects[j].size(), tmp_class_number_rects[i].size(), cost);
				printf("side_cost: %f,%f, shape_score: %f, reg_cost: %f\n",
					local_name_vals[j], class_number_vals[i], lambda * shape_cost, reg_cost);
				DebugEval(src, init_upper_region, tmp_local_name_rects[j], tmp_class_number_rects[i]);
#endif
				if (i == 0 && j == 0 || min_val > cost) {
					//if (i == 0 && j == 0 || max_val > val) {
					min_val = cost;
					min_i = i;
					min_j = j;
				}
			}
		}

		class_numbers = tmp_class_number_rects[min_i];
		local_name = CombineRect(tmp_local_name_rects[min_j]);

		if (local_name.x + local_name.width > class_numbers[0].x) {
			float mid_point = (local_name.x + local_name.width + class_numbers[0].x) / 2.0;
			local_name.width = mid_point - local_name.x;
			class_numbers[0].x = mid_point;
		}
		//if (large_plate) {
		//	LicensePlateInfo::TransformRectPosition(upper_rect, plate_info.ClassNumberChars(3), init_upper_region, class_numbers);
		//	LicensePlateInfo::TransformRectPosition(upper_rect, plate_info.LocalNameRegion(), init_upper_region, local_name);
		//}
		return 1.0 - min_val / (2.0 + lambda + lambda2);
	}



	void PlateSegmentation::ExtractCharRegions(const cv::Mat& img,
		cv::Rect& local_name, std::vector<cv::Rect>& class_number,
		cv::Rect& hiragana, std::vector<cv::Rect>& car_number,
		bool& large_plate) const
	{
		assert(img.type() == CV_8UC1 || img.type() == CV_8UC3);

		////////////// 前処理 ////////////////
		// グレースケール変換＋画像切り取り
		cv::Mat gray;
		if (img.type() == CV_8UC3) {
			cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
		}
		else {
			gray = img;
		}

		// 処理領域のリサイズ
		float rescale = 1.0;
		cv::Mat proc_img;
		if (_proc_width <= 0) {
			proc_img = gray;
		}
		else {
			rescale = (float)_proc_width / gray.cols;
			cv::resize(gray, proc_img, cv::Size(_proc_width, gray.rows * rescale));
		}
		////////////////////////////////////////////////////////////////

		//LicensePlateInfo plate_info(proc_img.size());
		cv::Mat integ_img;
		cv::integral(proc_img, integ_img, CV_32SC1);

#if __DEBUG__
		cv::namedWindow("test");
#endif
		int margin = proc_img.rows / 30;

		cv::Rect2f plate_rect(0, 0, proc_img.cols, proc_img.rows);
		LicensePlateInfo plate_info(plate_rect);
		LicensePlateInfo plate_info_L1(plate_rect, LicensePlateInfo::LARGEPLATE_SHORT_LOCALNAME);

		//! 下領域（ひらがな＋車両番号）の検出 
		cv::Rect2f lower_rect = plate_info.LowerCharRegion();
		cv::Rect2f lower_regionV;
		float lower_evalV = SearchVerticalRegion(integ_img, lower_regionV, lower_rect, margin, lower_rect.height / 6);
		//float lower_eval = SearchHorizontalRegion(integ, lower_region, lower_regionV, margin, lower_rect.width / 6);

		//! 上領域（地名＋分類番号）の検出 
		cv::Rect2f upper_rect = plate_info.UpperCharRegion();
		cv::Rect2f upper_regionV;
		float upper_evalV = SearchVerticalRegion(integ_img, upper_regionV, upper_rect, margin, upper_rect.height / 6);
		//float upper_eval = SearchHorizontalRegion(integ, upper_region, upper_regionV, margin, upper_rect.height / 6);


		// 大板判定
		float middle_hight_ratio_inv = lower_rect.height / upper_rect.height;
		float large_hight_ratio_inv = plate_info_L1.LowerCharRegion().height / plate_info_L1.UpperCharRegion().height;
		float avg_hight_ratio = 2.0 / (middle_hight_ratio_inv + large_hight_ratio_inv);	//調和平均

		float height_ratio = upper_regionV.height / lower_regionV.height;
		large_plate = (height_ratio < avg_hight_ratio);
#if __DEBUG__
		if (large_plate)
			printf("<large plate>\n");
#endif
		LicensePlateInfo* info;
		if (large_plate) {
			info = &plate_info_L1;
			lower_regionV.x = lower_rect.x;
			lower_regionV.width = lower_rect.width;
			upper_regionV.x = upper_rect.x;
			upper_regionV.width = upper_rect.width;
		}
		else {
			info = &plate_info;
		}

		///////// 下領域のフィッティング //////////////
		lower_rect = info->LowerCharRegion();
		upper_rect = info->UpperCharRegion();
		std::vector<cv::Rect2f> lower_src_regions = info->CarNumberChars();
		lower_src_regions.insert(lower_src_regions.begin(), info->HiraganaRegionMax());
		std::vector<cv::Rect2f> lower_regions;
		std::vector<float> lower_rect_weight(5);
		for (int i = 1;i <= 4; i++) {
			lower_rect_weight[i] = 0.25 * (i + 1);
		}
		lower_rect_weight[0] = 1.0;
		int search_range = lower_regionV.width / 8;
		FitMultiRectHorizontal(proc_img, integ_img, lower_rect, lower_src_regions, lower_regionV, lower_regions,
			search_range, search_range, search_range, search_range, 1, lower_rect_weight);

		// ひらがなの微調整
		cv::Rect2f tmp_hiragana;
		int hiragana_search_range = lower_regions[0].height / 3;
		int hiragana_ver_margin = lower_regions[0].height / 3;
		int hiragana_hor_margin = lower_regions[0].height / 4;
		FitRectHorizontal(proc_img, integ_img, lower_regions[0], tmp_hiragana,
			hiragana_search_range, hiragana_search_range,
			hiragana_hor_margin, hiragana_hor_margin);
		FitRectVertical(proc_img, integ_img, tmp_hiragana, lower_regions[0],
			hiragana_search_range, hiragana_search_range,
			hiragana_ver_margin, hiragana_ver_margin);

		// 車両番号領域の微調整(かなりヒューリスティック)
		cv::Rect2f last_carnum = lower_regions.back();
		float hiragana_bottom = lower_regions[0].y + lower_regions[0].height;
		float carnum_bottom = last_carnum.y + last_carnum.height;
		if (carnum_bottom - hiragana_bottom < (proc_img.rows - carnum_bottom) / 4) {
			float h = proc_img.rows - lower_regions[1].y - 1;
			for (int i = 1; i < lower_regions.size(); i++) {
				lower_regions[i].height = h;
			}
		}

		/////// 下領域の結果をリサイズして格納 //////
		std::vector<cv::Rect2f> rescale_lower_regions;
		RescaleRect(lower_regions, rescale_lower_regions, 1.0 / rescale);
		hiragana = TruncateRect(RescaleRectSize(rescale_lower_regions[0], cv::Size2f(1.2,1.2)), img.size());
		for (int i = 1;i < rescale_lower_regions.size();i++) {
			cv::Rect expand_rect = RescaleRectSize(rescale_lower_regions[i], cv::Size2f(1.0, 1.1));
			car_number.push_back(TruncateRect(expand_rect, img.size()));
		}


		//////////// 上領域のフィッティング ///////////////
		// フィッティングした下領域に合わせて、上領域の初期位置を変更
		cv::Rect2f detected_lower_rect = CombineRect(lower_regions);
		cv::Rect2f init_upper_rect, upper_region_rect;
		LicensePlateInfo::TransformRectPosition(lower_regionV, upper_regionV, detected_lower_rect, init_upper_rect);
		//SearchHorizontalRegion(src, integ, upper_region_rect, init_upper_rect, init_upper_rect.height / 4);
		////////////////////

		// 上領域を構成する矩形群（地名＋分類番号＋鋲）、およびその重みを設定
		cv::Rect2f local_name_f;
		std::vector<cv::Rect2f> class_numbers;
		if (large_plate) {
			std::vector<LicensePlateInfo> plate_info_Ls;
			plate_info_Ls.push_back(plate_info_L1);
			plate_info_Ls.push_back(LicensePlateInfo(plate_rect, LicensePlateInfo::LARGEPLATE_LONG_LOCALNAME));
			plate_info_Ls.push_back(LicensePlateInfo(plate_rect, LicensePlateInfo::LARGEPLATE_SHORT_CLASSNUMBER));
			float max_val;
			for (int i = 0;i < plate_info_Ls.size();i++) {
				cv::Rect2f local_name_f2;
				std::vector<cv::Rect2f> class_numbers2;
				float val = DetectLocalNameAndClassNumberRegions(proc_img, integ_img, init_upper_rect, plate_info_Ls[i], local_name_f2, class_numbers2);
				if (i == 0 || val > max_val) {
					max_val = val;
					local_name_f = local_name_f2;
					class_numbers = class_numbers2;
				}
			}
		}
		else {
			DetectLocalNameAndClassNumberRegions(proc_img, integ_img, init_upper_rect, plate_info, local_name_f, class_numbers);
		}

		/////// 上領域の結果をリサイズして格納 //////
		cv::Rect2f rescale_local_name_f;
		RescaleRect(local_name_f, rescale_local_name_f, 1.0 / rescale);
		std::vector<cv::Rect2f> rescale_class_numbers;
		RescaleRect(class_numbers, rescale_class_numbers, 1.0 / rescale);
		for (int i = 0;i < rescale_class_numbers.size();i++) {
			cv::Rect expand_rect = RescaleRectSize(rescale_class_numbers[i], cv::Size2f(1.0, 1.2));
			class_number.push_back(TruncateRect(expand_rect, img.size()));
		}
		local_name = TruncateRect(RescaleRectSize(rescale_local_name_f, cv::Size2f(1.0, 1.2)), img.size());

#if __DEBUG__
		cv::Mat draw_img = proc_img.clone();

		draw_img = proc_img.clone();
		for (int i = 0;i < lower_regions.size();i++) {
			cv::rectangle(draw_img, lower_regions[i], cv::Scalar(127));
		}
		cv::rectangle(draw_img, local_name_f, cv::Scalar(127));
		for (int i = 0;i < class_numbers.size();i++) {
			cv::rectangle(draw_img, class_numbers[i], cv::Scalar(127));
		}
		cv::imshow("test", draw_img);
		cv::waitKey();
		//printf("%f, %f\n", upper_eval, lower_eval);
		//draw_img = src.clone();
		//cv::rectangle(draw_img, lower_region, cv::Scalar(127));
		//cv::rectangle(draw_img, upper_region, cv::Scalar(127));
		//cv::imshow("test", draw_img);
		//cv::waitKey();

		cv::destroyWindow("test");
#endif
	}

}

