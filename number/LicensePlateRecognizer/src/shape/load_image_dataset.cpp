#include "load_image_dataset.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <set>
#include <map>
//#include <boost/algorithm/string/trim.hpp>

namespace cvsr {
	
/*	void dump(const boost::property_tree::ptree& pt, const std::string& indent = "") {
		using namespace boost::property_tree;
		for (auto& node : pt) {
			std::cout << indent << node.first;
			auto value = boost::trim_copy(node.second.get_value(""));
			if (!value.empty())
				std::cout << ": '" << value << "'";
			std::cout << "\n";
			dump(node.second, indent + "    ");
		}
	}
	*/


	bool load_image_dataset(const std::string& filename,
		std::vector<std::string>& image_list,
		std::vector<std::vector<cv::Rect> >& rectangles,
		std::vector<std::vector<cvsr::shape_landmarks> >& landmarks,
		bool relative_path_from_file)
	{
		using namespace boost::property_tree;
		//using namespace boost::property_tree::xml_parser;

		ptree pt;
		try {
			read_xml(filename, pt);
			//dump(pt);
			//const int indent = 2;
			//write_xml("test.xml", pt, std::locale(),
			//	xml_writer_make_settings(' ', indent, widen<std::string>("utf-8")));

			boost::filesystem::path work_dir;
			if(relative_path_from_file){
				work_dir = boost::filesystem::path(filename).parent_path();
			}

			std::set<std::string> part_name_list;
			std::vector<std::vector<std::map<std::string, cv::Point2f> > > all_landmark_info_list;
			BOOST_FOREACH(const ptree::value_type &v, pt.get_child("dataset.images")) {
				if (v.first != "image")
					continue;

				const ptree& img_pt = v.second;
				boost::optional<std::string> file_attr = img_pt.get_optional<std::string>("<xmlattr>.file");
				boost::filesystem::path img_path(file_attr.get());
				std::string img_file_name = img_path.string();
				if (!img_path.is_absolute() && relative_path_from_file) {
					img_file_name = (work_dir / img_path).string();
				}

				//// debug
				//std::cout << file_attr.get() << std::endl;

				std::vector<cv::Rect> rect_list;
				std::vector<std::map<std::string, cv::Point2f> > landmark_info_list;
				BOOST_FOREACH(const ptree::value_type &img_v, img_pt) {
					if (img_v.first != "box")
						continue;

					boost::optional<int> attr_top = img_v.second.get_optional<int>("<xmlattr>.top");
					boost::optional<int> attr_left = img_v.second.get_optional<int>("<xmlattr>.left");
					boost::optional<int> attr_width = img_v.second.get_optional<int>("<xmlattr>.width");
					boost::optional<int> attr_height = img_v.second.get_optional<int>("<xmlattr>.height");

					cv::Rect rect(attr_left.get(), attr_top.get(), attr_width.get(), attr_height.get());
					rect_list.push_back(rect);

					// debug
					//std::cout << "box: " << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << std::endl;
					//std::cout << "part: ";

					std::map<std::string, cv::Point2f> landmark_info;
					const ptree& part_pt = img_v.second;
					BOOST_FOREACH(const ptree::value_type &part_v, part_pt) {
						if (part_v.first != "part")
							continue;
						boost::optional<std::string> attr_name
							= part_v.second.get_optional<std::string>("<xmlattr>.name");
						boost::optional<int> attr_x
							= part_v.second.get_optional<int>("<xmlattr>.x");
						boost::optional<int> attr_y
							= part_v.second.get_optional<int>("<xmlattr>.y");

						cv::Point2f pt(attr_x.get(), attr_y.get());
						std::string part_name = attr_name.get();
						landmark_info.insert(std::pair<std::string, cv::Point2f>(part_name, pt));
						part_name_list.insert(part_name);

						//debug
						//std::cout << "(" << pt.x << ", " << pt.y << ")";
					}
					landmark_info_list.push_back(landmark_info);

					//debug
					//std::cout << std::endl << std::endl;
				}

				image_list.push_back(img_file_name);
				all_landmark_info_list.push_back(landmark_info_list);
				rectangles.push_back(rect_list);
			}

			// プロットされていないLandmarkを確認しつつ格納
			int num_parts = part_name_list.size();
			int num_images = image_list.size();
			std::map<std::string, cv::Point2f>::const_iterator parts_it;
			landmarks.resize(num_images);
			for (int i = 0;i < num_images;i++) {
				int num_objs = rectangles[i].size();
				landmarks[i].resize(num_objs);
				for (int j = 0;j < num_objs;j++) {
					shape_landmarks part_points(num_parts, cv::Point2f(-1, -1));
					std::set<std::string>::const_iterator part_name_it = part_name_list.begin();
					const std::map<std::string, cv::Point2f>& parts = all_landmark_info_list[i][j];
					for (;part_name_it != part_name_list.end(); part_name_it++) {
						parts_it = parts.find(*part_name_it);
						if (parts_it == parts.end())
							continue;

						part_points[std::distance(parts.begin(), parts_it)] = parts.at(*part_name_it);
					}
					landmarks[i][j] = part_points;
				}
			}
			return true;
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return false;
		}
	}

	
	bool save_image_dataset(const std::string& filename,
		const std::string& title,
		const std::vector<std::string>& image_list,
		const std::vector<std::vector<cv::Rect> >& rectangles,
		const std::vector<std::vector<cvsr::shape_landmarks> >& landmarks,
		const std::string& comment)
	{
		assert(image_list.size() == rectangles.size());
		assert(image_list.size() == landmarks.size());

		using namespace boost::property_tree;
		//using namespace boost::property_tree::xml_parser;

		try {
			ptree root_pt;
			ptree& dataset_pt = root_pt.add("dataset", "");
			dataset_pt.add("name", title);
			if (!comment.empty())
				dataset_pt.add("comment", comment);

			ptree& images_pt = dataset_pt.add("images", "");
			for (int i = 0;i < image_list.size();i++) {
				ptree& image_pt = images_pt.add("image", "");
				image_pt.put("<xmlattr>.file", image_list[i]);
				assert(rectangles[i].size() == landmarks[i].size());
				for (int j = 0;j < rectangles[i].size();j++) {
					ptree& box_pt = image_pt.add("box", "");
					box_pt.put("<xmlattr>.top", rectangles[i][j].y);
					box_pt.put("<xmlattr>.left", rectangles[i][j].x);
					box_pt.put("<xmlattr>.width", rectangles[i][j].width);
					box_pt.put("<xmlattr>.height", rectangles[i][j].height);
					for (int k = 0;k < landmarks[i][j].size();k++) {
						if (landmarks[i][j][k].x < 0 && landmarks[i][j][k].y)
							continue;
						ptree& part_pt = box_pt.add("part", "");
						part_pt.put("<xmlattr>.name", k + 1);
						part_pt.put("<xmlattr>.x", (int)(landmarks[i][j][k].x + 0.5));
						part_pt.put("<xmlattr>.y", (int)(landmarks[i][j][k].y + 0.5));
					}
				}
			}
			write_xml(filename, root_pt, std::locale(), 
				xml_parser::xml_writer_make_settings<std::string>('\t', 1));
			return true;
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return false;
		}
	}
}