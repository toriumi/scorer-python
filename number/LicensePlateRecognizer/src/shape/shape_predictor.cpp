#include "shape_predictor.h"
#include <stdexcept>

namespace cvsr{

	point_affine_transformer::point_affine_transformer(const cv::Matx33d& a) {
		_m.create(2, 2);
		_b.create(2, 1);
		for (int y = 0;y < 2;y++) {
			for (int x = 0;x < 2;x++)
				_m(y, x) = a(y, x);
			_b(y, 0) = a(y, 2);
		}
	}


	point_affine_transformer point_affine_transformer::find_tform_between_shapes(
		const cv::Mat1f& from_shape, const cv::Mat1f& to_shape)
	{
		assert(from_shape.rows == to_shape.rows && (from_shape.rows % 2) == 0 && from_shape.rows > 0);
		const unsigned long num = from_shape.rows / 2;
		if (num == 1)
		{
			// Just use an identity transform if there is only one landmark.
			return point_affine_transformer();
		}

		std::vector<cv::Mat1d> from_points(num), to_points(num);
		cv::Mat1d mean_from = cv::Mat1d::zeros(2,1), mean_to = cv::Mat1d::zeros(2,1);
		cv::Mat1d cov = cv::Mat1d::zeros(2,2);
		for (unsigned long i = 0; i < num; ++i)
		{
			//from_points[i] = location(from_shape, i);
			//to_points[i] = location(to_shape, i);
			location(from_shape, i).convertTo(from_points[i], CV_64FC1);
			location(to_shape, i).convertTo(to_points[i], CV_64FC1);
		}

		for (unsigned long i = 0; i < num; ++i)
		{
			mean_from += from_points[i];
			mean_to += to_points[i];
		}
		mean_from /= num;
		mean_to /= num;

		double sigma_from = 0, sigma_to = 0;
		for (unsigned long i = 0; i < num; ++i)
		{
			cv::Mat1d diff_from = from_points[i] - mean_from;
			cv::Mat1d diff_to = to_points[i] - mean_to;
			sigma_from += diff_from.dot(diff_from);
			sigma_to += diff_to.dot(diff_to);
			cov += diff_to * diff_from.t();
		}
		sigma_from /= num;
		sigma_to /= num;
		cov /= num;

		cv::Mat1d u, vt, d;
		cv::SVD::compute(cov, d, u, vt);
		cv::Mat1d s = cv::Mat1d::eye(cov.size());
		if (cv::determinant(cov) < 0 ||
			(cv::determinant(cov) == 0 && cv::determinant(u) * cv::determinant(vt) < 0))
		{
			if (d(1, 0) < d(0, 0))
				s(1, 1) = -1;
			else
				s(0, 0) = -1;
		}

		cv::Mat1d r = u * s * vt;
		double c = 1.0;
		if(sigma_from != 0)
			c = 1.0 / sigma_from * cv::sum(s * d)(0);

		r *= c;
		cv::Mat1d t = mean_to - r*mean_from;
		return point_affine_transformer(r, t);
	}

	
	point_affine_transformer point_affine_transformer::normalizing_tform(
		const cv::Rect& rect)
	{
		cv::Matx33d from_points, to_points = cv::Matx33d::ones();
		from_points(0, 0) = rect.x;
		from_points(1, 0) = rect.y;
		from_points(0, 1) = rect.x + rect.width - 1;
		from_points(1, 1) = rect.y;
		from_points(0, 2) = from_points(0, 1);
		from_points(1, 2) = rect.y + rect.height - 1;
		to_points(0, 0) = 0.0;
		to_points(1, 0) = 0.0;
		to_points(1, 1) = 0.0;

		for (int i = 0;i < 3;i++)
			from_points(2, i) = 1.0;

		cv::Matx33d affine = to_points * from_points.inv();
		return point_affine_transformer(affine);
	}


	point_affine_transformer point_affine_transformer::unnormalizing_tform(const cv::Rect& rect)
	{
		cv::Matx33d from_points = cv::Matx33d::ones(), to_points;
		to_points(0, 0) = rect.x;
		to_points(1, 0) = rect.y;
		to_points(0, 1) = rect.x + rect.width - 1;
		to_points(1, 1) = rect.y;
		to_points(0, 2) = to_points(0, 1);
		to_points(1, 2) = rect.y + rect.height - 1;
		from_points(0, 0) = 0.0;
		from_points(1, 0) = 0.0;
		from_points(1, 1) = 0.0;

		for (int i = 0;i < 3;i++)
			to_points(2, i) = 1.0;

		cv::Matx33d affine = to_points * from_points.inv();
		return point_affine_transformer(affine);
	}


	void extract_feature_pixel_values(
		const cv::Mat1b& img_,
		const point_affine_transformer& tform_to_img,
		const cv::Mat1f& current_shape,
		const cv::Mat1f& reference_shape,
		const std::vector<unsigned long>& reference_pixel_anchor_idx,
		const std::vector<cv::Mat1f >& reference_pixel_deltas,
		std::vector<float>& feature_pixel_values
		)
	{
		point_affine_transformer point_affine = point_affine_transformer::find_tform_between_shapes(reference_shape, current_shape);
		cv::Mat1f tform;
		point_affine.get_m().convertTo(tform, CV_32FC1);

		feature_pixel_values.resize(reference_pixel_deltas.size());
		for (unsigned long i = 0; i < feature_pixel_values.size(); ++i)
		{
			// Compute the point in the current shape corresponding to the i-th pixel and
			// then map it from the normalized shape space into pixel space.
			cv::Mat1f trans_p = tform*reference_pixel_deltas[i] + location(current_shape, reference_pixel_anchor_idx[i]);
			cv::Point2f pt = tform_to_img(trans_p);
			if (inside(img_.size(), pt))
				feature_pixel_values[i] = (float)img_(pt);
			else
				feature_pixel_values[i] = 0;
		}
	}
	

	void shape_predictor::operator()(
		const cv::Mat1b& img,
		//const image_type& img,
		const cv::Rect& rect,
		shape_landmarks& parts
		) const
	{
		cv::Mat1f current_shape = initial_shape.clone();
		std::vector<float> feature_pixel_values;
		point_affine_transformer tform_to_img = point_affine_transformer::unnormalizing_tform(rect);
		for (unsigned long iter = 0; iter < forests.size(); ++iter)
		{
			cvsr::extract_feature_pixel_values(img, tform_to_img, current_shape, initial_shape,
				anchor_idx[iter], deltas[iter], feature_pixel_values);
			unsigned long leaf_idx;
			// evaluate all the trees at this level of the cascade.
			for (unsigned long i = 0; i < forests[iter].size(); ++i) {
				current_shape += forests[iter][i](feature_pixel_values, leaf_idx);
			}
		}

		// convert the current_shape into a full_object_detection
		parts.resize(num_parts());
		for (unsigned long i = 0; i < parts.size(); ++i) {
			parts[i] = tform_to_img(location(current_shape, i));
		}
	}


	void shape_predictor::write(std::ostream& out) const
	{
		int version = 1;
		out.write((const char*)&version, sizeof(int));
		cv_util::writeMat(initial_shape, out);
		cv_util::writeVector<regression_tree>(forests, out, regression_tree::write);
		cv_util::writeVector<unsigned long>(anchor_idx, out);
		cv_util::writeVector<cv::Mat1f>(deltas, out, cv_util::writeMat<float>);
	}

	void shape_predictor::read(std::istream& in)
	{
		int version = 0;
		in.read((char*)&version, sizeof(int));
		if (version != 1)
			throw std::runtime_error("Unexpected version found while cvsr::shape_predictor::read().");
		cv_util::readMat(initial_shape, in);
		cv_util::readVector<regression_tree>(forests, in, regression_tree::read);
		cv_util::readVector<unsigned long>(anchor_idx, in);
		cv_util::readVector<cv::Mat1f>(deltas, in, cv_util::readMat<float>);
	}


}
