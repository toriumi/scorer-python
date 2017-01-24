// Copyright (C) 2014  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
// This program was rewritten for replacing dlib to OpenCV by Minagagawa. T. (2016).

#ifndef __CV_SHAPE_PREDICTOR__
#define __CV_SHAPE_PREDICTOR__

#include <deque>
#include <opencv2/core.hpp>
#include <fstream>
#include "binary_func.h"

// ----------------------------------------------------------------------------------------

namespace cvsr
{
	typedef std::vector<cv::Point2f> shape_landmarks;


	struct split_feature
	{
		unsigned long idx1;
		unsigned long idx2;
		float thresh;

		static void write(const split_feature& item, std::ostream& out)
		{
			out.write((const char*)&item.idx1, sizeof(item.idx1));
			out.write((const char*)&item.idx2, sizeof(item.idx1));
			out.write((const char*)&item.thresh, sizeof(item.thresh));
		}

		static void read(split_feature& item, std::istream& in)
		{
			in.read((char*)&item.idx1, sizeof(item.idx1));
			in.read((char*)&item.idx2, sizeof(item.idx2));
			in.read((char*)&item.thresh, sizeof(item.thresh));
		}
	};


	// a tree is just a std::vector<cvsp::split_feature>.  We use this function to navigate the
	// tree nodes
	inline unsigned long left_child(unsigned long idx) { return 2 * idx + 1; }
	/*!
		ensures
			- returns the index of the left child of the binary tree node idx
	!*/
	inline unsigned long right_child(unsigned long idx) { return 2 * idx + 2; }
	/*!
		ensures
			- returns the index of the left child of the binary tree node idx
	!*/

	struct regression_tree
	{
		std::vector<split_feature> splits;
		std::vector<cv::Mat1f> leaf_values;

		unsigned long num_leaves() const { return leaf_values.size(); }

		/*!
		requires
		- All the index values in splits are less than feature_pixel_values.size()
		- leaf_values.size() is a power of 2.
		(i.e. we require a tree with all the levels fully filled out.
		- leaf_values.size() == splits.size()+1
		(i.e. there needs to be the right number of leaves given the number of splits in the tree)
		ensures
		- runs through the tree and returns the vector at the leaf we end up in.
		- #i == the selected leaf node index.
		!*/
		inline const cv::Mat1f& operator()(
			const std::vector<float>& feature_pixel_values,
			unsigned long& i
			) const
		{
			i = 0;
			while (i < splits.size())
			{
				if (feature_pixel_values[splits[i].idx1] - feature_pixel_values[splits[i].idx2] > splits[i].thresh)
					i = left_child(i);
				else
					i = right_child(i);
			}
			i = i - splits.size();
			return leaf_values[i];
		}

		static void write(const regression_tree& item, std::ostream& out) {
			cv_util::writeVector<cv::Mat1f>(item.leaf_values, out, cv_util::writeMat<float>);
			cv_util::writeVector(item.splits, out, split_feature::write);
		}

		static void read(regression_tree& item, std::istream& in) {
			cv_util::readVector(item.leaf_values, in, cv_util::readMat<float>);
			cv_util::readVector(item.splits, in, split_feature::read);
		}
	};

	// ------------------------------------------------------------------------------------

	/*!
	requires
	- idx < shape.size()/2
	- shape.size()%2 == 0
	ensures
	- returns the idx-th point from the shape vector.
	!*/
	inline cv::Mat1f location(
		const cv::Mat1f& shape,
		unsigned long idx
		)
	{
		return shape(cv::Rect(0, idx*2, 1, 2)).clone();
	}

	template <typename T>
	bool inside(const cv::Rect& rect, const cv::Point_<T>& pt) {
		return (pt.x >= rect.x && pt.x < rect.width && pt.y >= rect.y && pt.y < rect.height);
	}

	template <typename T>
	bool inside(const cv::Size& size, const cv::Point_<T>& pt) {
		return inside(cv::Rect(0, 0, size.width, size.height), pt);
	}
	// ------------------------------------------------------------------------------------

	inline unsigned long nearest_shape_point(
		const cv::Mat1f& shape,
		const cv::Mat1f& pt
		)
	{
		// find the nearest part of the shape to this pixel
		float best_dist = std::numeric_limits<float>::infinity();
		const unsigned long num_shape_parts = shape.rows / 2;
		//const unsigned long num_shape_parts = shape.size() / 2;
		unsigned long best_idx = 0;
		for (unsigned long j = 0; j < num_shape_parts; ++j)
		{
			const float dist = cv::norm(location(shape, j), pt);
			//const float dist = length_squared(location(shape, j) - pt);
			if (dist < best_dist)
			{
				best_dist = dist;
				best_idx = j;
			}
		}
		return best_idx;
	}

	// ------------------------------------------------------------------------------------

	/*!
	requires
	- shape.size()%2 == 0
	- shape.size() > 0
	ensures
	- #anchor_idx.size() == pixel_coordinates.size()
	- #deltas.size()     == pixel_coordinates.size()
	- for all valid i:
	- pixel_coordinates[i] == location(shape,#anchor_idx[i]) + #deltas[i]
	!*/
	inline void create_shape_relative_encoding(
		const cv::Mat1f& shape,
		const std::vector<cv::Mat1f >& pixel_coordinates,
		std::vector<unsigned long>& anchor_idx,
		std::vector<cv::Mat1f >& deltas
		)
	{
		anchor_idx.resize(pixel_coordinates.size());
		deltas.resize(pixel_coordinates.size());


		for (unsigned long i = 0; i < pixel_coordinates.size(); ++i)
		{
			anchor_idx[i] = nearest_shape_point(shape, pixel_coordinates[i]);
			deltas[i] = pixel_coordinates[i] - location(shape, anchor_idx[i]);
		}
	}

	// ------------------------------------------------------------------------------------
	class point_affine_transformer
	{
	private:
		cv::Mat1d _m;
		cv::Mat1d _b;

	public:
		point_affine_transformer() {
			_m = cv::Mat1d::eye(2, 2);
			_b = cv::Mat1d::zeros(2, 1);
		}

		point_affine_transformer(const cv::Mat1f& m, const cv::Mat1f& b)
		{m.convertTo(_m, CV_64FC1), b.convertTo(_b, CV_64FC1);}

		point_affine_transformer(const cv::Mat1d& m, const cv::Mat1d& b) :
			_m(m), _b(b) { }

		point_affine_transformer(const cv::Matx33d& a);

		const cv::Mat1d& get_m() {
			return _m;
		}

		cv::Point2f operator()(const cv::Mat1d& src) const {
			cv::Mat1d dst = _m * src + _b;
			return cv::Point2f(dst(0, 0), dst(1, 0));
		}

		cv::Point2f operator()(const cv::Point2d& src) const {
			return operator()(cv::Mat1d(src));
		}

		cv::Point2f operator()(const cv::Mat1f& src) const {
			cv::Mat1d src2;
			src.convertTo(src2, CV_64FC1);
			return operator()(src2);
		}

		static point_affine_transformer find_tform_between_shapes(
			const cv::Mat1f& from_shape, const cv::Mat1f& to_shape);

		/*!
		\param[in] rect input rectangle
		\return a transform that maps top left corner to (0,0) and bottom right corner to (1,1).
		*/
		static point_affine_transformer normalizing_tform(const cv::Rect& rect);

		/*!
		\param[in] rect input rectangle
		\return a transform that maps (0,0) to top left corner and (1,1) to bottom right corner.
		*/
		static point_affine_transformer unnormalizing_tform(const cv::Rect& rect);
	};

	// ------------------------------------------------------------------------------------

	/*!
	requires
	- image_type == an image object that implements the interface defined in
	dlib/image_processing/generic_image.h
	- reference_pixel_anchor_idx.size() == reference_pixel_deltas.size()
	- current_shape.size() == reference_shape.size()
	- reference_shape.size()%2 == 0
	- max(mat(reference_pixel_anchor_idx)) < reference_shape.size()/2
	ensures
	- #feature_pixel_values.size() == reference_pixel_deltas.size()
	- for all valid i:
	- #feature_pixel_values[i] == the value of the pixel in img_ that
	corresponds to the pixel identified by reference_pixel_anchor_idx[i]
	and reference_pixel_deltas[i] when the pixel is located relative to
	current_shape rather than reference_shape.
	!*/
	void extract_feature_pixel_values(
		const cv::Mat1b& img_,
		const point_affine_transformer& tform_to_img,
		const cv::Mat1f& current_shape,
		const cv::Mat1f& reference_shape,
		const std::vector<unsigned long>& reference_pixel_anchor_idx,
		const std::vector<cv::Mat1f >& reference_pixel_deltas,
		std::vector<float>& feature_pixel_values
		);

// ----------------------------------------------------------------------------------------

class shape_predictor
{
public:

	shape_predictor()
	{}

	/*!
	requires
	- initial_shape.size()%2 == 0
	- forests.size() == pixel_coordinates.size() == the number of cascades
	- for all valid i:
	- all the index values in forests[i] are less than pixel_coordinates[i].size()
	- for all valid i and j:
	- forests[i][j].leaf_values.size() is a power of 2.
	(i.e. we require a tree with all the levels fully filled out.
	- forests[i][j].leaf_values.size() == forests[i][j].splits.size()+1
	(i.e. there need to be the right number of leaves given the number of splits in the tree)
	!*/
	shape_predictor(
		const cv::Mat1f& initial_shape_,
		//const dlib::matrix<float, 0, 1>& initial_shape_,
		const std::vector<std::vector<cvsr::regression_tree> >& forests_,
		const std::vector<std::vector<cv::Mat1f> >& pixel_coordinates
		) : initial_shape(initial_shape_), forests(forests_)

	{
		anchor_idx.resize(pixel_coordinates.size());
		deltas.resize(pixel_coordinates.size());
		// Each cascade uses a different set of pixels for its features.  We compute
		// their representations relative to the initial shape now and save it.
		for (unsigned long i = 0; i < pixel_coordinates.size(); ++i) {
			cvsr::create_shape_relative_encoding(initial_shape, pixel_coordinates[i], anchor_idx[i], deltas[i]);
		}
	}

	unsigned long num_parts(
		) const
	{
		//return initial_shape.size() / 2;
		return initial_shape.rows / 2;
	}

	unsigned long num_features(
		) const
	{
		unsigned long num = 0;
		for (unsigned long iter = 0; iter < forests.size(); ++iter)
			for (unsigned long i = 0; i < forests[iter].size(); ++i)
				num += forests[iter][i].num_leaves();
		return num;
	}

	//template <typename image_type>
	void operator()(
		const cv::Mat1b& img,
		const cv::Rect& rect,
		shape_landmarks& parts
		) const;


	template <typename T, typename U>
	void operator()(
		const cv::Mat1b& img,
		const cv::Rect& rect,
		std::vector<std::pair<T, U> >& feats,
		shape_landmarks& parts
		) const
	{
		feats.clear();
		cv::Mat1f current_shape = initial_shape.clone();
		std::vector<float> feature_pixel_values;
		unsigned long feat_offset = 0;
		//const point_transform_affine tform_to_img = unnormalizing_tform(rect);
		point_affine_transformer tform_to_img = point_affine_transformer::unnormalizing_tform(rect);
		for (unsigned long iter = 0; iter < forests.size(); ++iter)
		{
			cvsr::extract_feature_pixel_values(img, tform_to_img, current_shape, initial_shape,
				anchor_idx[iter], deltas[iter], feature_pixel_values);
			// evaluate all the trees at this level of the cascade.
			for (unsigned long i = 0; i < forests[iter].size(); ++i)
			{
				unsigned long leaf_idx;
				current_shape += forests[iter][i](feature_pixel_values, leaf_idx);

				feats.push_back(std::make_pair(feat_offset + leaf_idx, 1));
				feat_offset += forests[iter][i].num_leaves();
			}
		}

		// convert the current_shape into a full_object_detection
		parts.resize(num_parts());
		for (unsigned long i = 0; i < parts.size(); ++i) {
			parts[i] = tform_to_img(location(current_shape, i));
		}
	}

	bool isReady() const {
		return !initial_shape.empty() & !forests.empty() & !anchor_idx.empty() & !deltas.empty();
	}

	void write(std::ostream& out) const;

	void read(std::istream& in);

	bool Save(const std::string& filename) const{
		std::ofstream out(filename, std::ios::binary);
		if (!out.is_open())
			return false;
		write(out);
		return true;
	}

	bool Load(const std::string& filename) {
		std::ifstream in(filename, std::ios::binary);
		if (!in.is_open())
			return false;
		read(in);
		return true;
	}


private:
	cv::Mat1f initial_shape;
	std::vector<std::vector<cvsr::regression_tree> > forests;
	std::vector<std::vector<unsigned long> > anchor_idx;
	std::vector<std::vector<cv::Mat1f > > deltas;
};

// ----------------------------------------------------------------------------------------


} // end namespace cvsp


#endif

