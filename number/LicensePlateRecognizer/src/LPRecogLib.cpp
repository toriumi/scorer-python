#include "LPRecogLib.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>

//! 新しいインスタンスを生成
lpr::LPRecog* new_LPRecog() {
	return new lpr::LPRecog;
}

//! インスタンスを削除
void delete_LPRecog(lpr::LPRecog* h_LPRecog) {
	delete h_LPRecog;
}

//! cv::MatデータをMyClassへ渡す
int initialize(lpr::LPRecog* h_LPRecog,
	const char* detector_file, const char* shape_file,
	const char* local_name_filter, const char* local_name_id,
	const char* class_number_filter, const char* class_number_id,
	const char* hiragana_filter, const char* hiragana_id,
	const char* car_number_filter, const char* car_number_id
	) {

	if (!h_LPRecog->LoadDetector(detector_file, shape_file)) {
		std::cerr << "Fail to load " << detector_file << std::endl << " or "
			<< shape_file << std::endl;
		return -1;
	}
	if(!h_LPRecog->LoadCarNumberData(car_number_filter)){
		std::cerr << "Fail to load " << car_number_filter << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadCarNumberId(car_number_id)) {
		std::cerr << "Fail to load " << car_number_filter << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadClassNumberData(class_number_filter)) {
		std::cerr << "Fail to load " << class_number_filter << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadClassNumberId(class_number_id)) {
		std::cerr << "Fail to load " << class_number_id << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadHiraganaData(hiragana_filter)) {
		std::cerr << "Fail to load " << hiragana_filter << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadHiraganaId(hiragana_id)) {
		std::cerr << "Fail to load " << hiragana_id << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadLocalNameData(local_name_filter)) {
		std::cerr << "Fail to load " << local_name_filter << std::endl;
		return -1;
	}
	if (!h_LPRecog->LoadLocalNameId(local_name_id)) {
		std::cerr << "Fail to load " << local_name_id << std::endl;
		return -1;
	}
	return 0;
}


void* Recog(lpr::LPRecog* h_LPRecog, 
	void* image_data_ptr, int image_width, int image_height, int image_type,
	int min_plate_width, int max_plate_width, int num_plates, float threshold) {

	cv::Mat img(image_height, image_width, image_type, image_data_ptr);

	std::vector<lpr::LPRecog::LPContents>* results = new std::vector<lpr::LPRecog::LPContents>;
	h_LPRecog->RecogPlates(img, *results, min_plate_width, max_plate_width, 
		cv::Rect(), num_plates, threshold);

	return results;
}


//! 検出結果を削除
void delete_PlateInfos(void* h_plate_infos) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	delete results;
}


//! 検出したナンバープレートの数
int NumDetected(void* h_plate_infos) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	return results->size();
}


//! id番目の結果の地名を取得
const char* GetLocalName(void* h_plate_infos, int id) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	return results->at(id).LocalName.c_str();
}

//! id番目の結果の分類番号を取得
const char* GetClassNumber(void* h_plate_infos, int id) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	return results->at(id).ClassNumber.c_str();
}


//! id番目の結果のひらがなを取得
const char* GetHiragana(void* h_plate_infos, int id) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	return results->at(id).Hiragana.c_str();
}


//! id番目の結果の車両番号を取得
const char* GetCarNumber(void* h_plate_infos, int id) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	return results->at(id).CarNumber.c_str();
}


//! id番目の結果のプレートコーナーを取得
void GetPlateCorners(void* h_plate_infos, int id, float* dst_corners) {
	std::vector<lpr::LPRecog::LPContents>* results = (std::vector<lpr::LPRecog::LPContents>*)h_plate_infos;
	std::vector<cv::Point2f> corners = results->at(id).PlateCorners;
	
	//float dst_corners[8];
	for (int i = 0;i < 4;i++) {
		dst_corners[2 * i] = corners[i].x;
		dst_corners[2 * i + 1] = corners[i].y;
	}
	//return dst_corners;
}


