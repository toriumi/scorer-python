#include "LPRecog.h"
#include "Exception.h"
#include "common/RectFunctions.h"
#include <time.h>
#include <fstream>
#include <iostream>
#include <functional>
#include <opencv2/imgproc.hpp>

#define __DEBUG__ 0

#if __DEBUG__
#include <opencv2/highgui.hpp>
#include "util.h"
#endif

namespace lpr{

LPRecog::LPRecog(void)
{
	//_PlateSegmentation.SetPlateMarginScale(cv::Size2f(1.1, 1.2));
	_PlateSegmentation.SetProcPlateWidth(240);
	//_PlateSegmentation.SetProcPlateWidth(160);

	CharRecognition::init_hmax();
	_CarNumberRecog.init(cv::Size(12,20));
	_CarNumberRecog.EnableLCN(false);
	_HiraganaRecog.init(cv::Size(22,22));
	_ClassNumberRecog.init(cv::Size(16,22), cv::Size(13,13));
	_LocalNameRecog.init(cv::Size(42,20));
}


LPRecog::~LPRecog(void)
{
}


void LPRecog::RecogPlates(const cv::Mat& img, std::vector<LPContents>& detected_plates,
	int min_plate_width, int max_plate_width, const cv::Rect& roi, int num_plates, float threshold, bool invert_img) const
{
	std::vector<LPContents> all_plates;
	cv::Mat gray;
	if(img.type() == CV_8UC3){
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	}
	else if(img.type() == CV_8UC1){
		gray = img;
	}
	else{
		throw lpr::Exception(lpr::Exception::INVALID_DATA_TYPE, "Input image should be CV_8UC1 or CV_8UC3.", __FUNCTION__, __FILE__, __LINE__);
	}

	_RecogAllPlates(gray, all_plates, min_plate_width, max_plate_width, roi, threshold);
	_SetCommercialFlag(all_plates, false);
	if(invert_img){
		cv::Mat inv_img = -gray + 255;
		std::vector<LPContents> all_plates2;
		_RecogAllPlates(inv_img, all_plates2, min_plate_width, max_plate_width, roi, threshold);
		_SetCommercialFlag(all_plates2, true);
		all_plates.insert(all_plates.end(), all_plates2.begin(), all_plates2.end());
	}
	std::sort(all_plates.begin(), all_plates.end(), std::greater<LPContents>());

	if(num_plates < 0)
		num_plates = all_plates.size();

	detected_plates.clear();
	for(int i=0; i<num_plates && i<all_plates.size(); i++){
		detected_plates.push_back(all_plates[i]);
	}
}



void LPRecog::_RecogAllPlates(const cv::Mat& img, std::vector<LPContents>& detected_plates,
	int min_plate_width, int max_plate_width, const cv::Rect& roi, float threshold) const
{
	clock_t start_t, end_t;

	start_t = clock();
	std::vector<cv::Rect> plates;
	_PlateDetection.Detect(img, plates, min_plate_width, max_plate_width, roi);
	end_t = clock();
#if __DEBUG__
	std::cout << "detection time: " << (double)(end_t-start_t)/CLOCKS_PER_SEC << std::endl;
#endif

	if(plates.empty())
		return;

	std::vector<LPContents> all_plates;
	std::vector<cv::Rect>::iterator plts_i = plates.begin(), plts_e = plates.end();
	for(;plts_i != plts_e; plts_i++){
		LPContents plate;
		plate.Threshold = threshold;

		// 検出したプレートのコーナー検出
		plate.PlatePosition = *plts_i;
		_PlateDetection.AlignPlateCorners(img, plate.PlatePosition, plate.PlateCorners);
		plate.PlateImage = _PlateDetection.extractPlateImage(
			img, plate.PlatePosition.size(), plate.PlateCorners);

		// 文字領域の切り出し
		start_t = clock();
		_PlateSegmentation.ExtractCharRegions(plate.PlateImage,
			plate.LocalNamePosition, plate.ClassNumberPositions, 
			plate.HiraganaPosition, plate.CarNumberPositions, 
			plate.LargePlate);
		end_t = clock();
#if __DEBUG__
		std::cout << "score: " << ret << ", ";
		std::cout << "segmentation time: " << (double)(end_t-start_t)/CLOCKS_PER_SEC << std::endl;
#endif

		////// 文字認識 //////
		// 全ての領域の文字を認識してplateへ格納
		_RecognizeAllPlateChars(plate);

		if(plate.Confidence > plate.Threshold)
			all_plates.push_back(plate);
	}
	detected_plates = all_plates;
}


//! プレート内全ての文字領域を認識
/*!
認識に必要なplateのメンバ:
PlateImage, Threshold, 
CarNumberPositions, ClassNumberPositions, HiranagaPosition, LocalNamePosition

格納されるplateのメンバ：
Confidence,
CarNumberId, CarNumber,
ClassNumberId, ClassNumber,
HiraganaId, Hiragana,
LocalNameId, LocalName

修正される可能性のあるメンバ：
ClassNumberPositions, LocalNamePosition
*/
void LPRecog::_RecognizeAllPlateChars(LPContents& plate) const
{
	std::vector<float> char_recog_probs(4);
	std::vector<float> prob_weights(4);

	// 車両番号認識
	char_recog_probs[0] = _RecognizeChars(_CarNumberRecog, _CarNumberIds, plate.PlateImage,
		plate.CarNumberPositions, plate.CarNumberId, plate.CarNumber);
	if (!_ValidCarNumberId(plate.CarNumberId) ||
		!_ValidNumberString(plate.CarNumber)) {
		if (plate.Threshold <= 0)
			char_recog_probs[0] = 0.0;
		else {
			plate.Confidence = 0;
			return;
		}
	}
	prob_weights[0] = 1.0 / _CarNumberRecog.NumClasses();
	//plate.Confidence = char_recog_probs[0];

	// 分類番号認識
	char_recog_probs[1] = _RecognizeChars(_ClassNumberRecog, _ClassNumberIds, plate.PlateImage,
		plate.ClassNumberPositions, plate.ClassNumberId, plate.ClassNumber);
	prob_weights[1] = 1.0 / _ClassNumberRecog.NumClasses();
	if (!_ValidNumberString(plate.ClassNumber)) {
		if (plate.Threshold <= 0)
			char_recog_probs[1] = 0.0;
		else {
			plate.Confidence = 0;
			return;
		}
	}

	// ひらがな認識
	char_recog_probs[2] = _RecognizeChar(_HiraganaRecog, _HiraganaIds, plate.PlateImage,
		plate.HiraganaPosition, plate.HiraganaId, plate.Hiragana);
	prob_weights[2] = 1.0 / _HiraganaRecog.NumClasses();

	// 地名認識
	char_recog_probs[3] = _RecognizeChar(_LocalNameRecog, _LocalNameIds, plate.PlateImage,
		plate.LocalNamePosition, plate.LocalNameId, plate.LocalName);
	prob_weights[3] = 1.0 / _LocalNameRecog.NumClasses();

	// 尾張小牧＋２桁以下の特殊ケース
	_CheckOwariKomakiClassNumber(plate, char_recog_probs[3], char_recog_probs[1]);

	plate.Confidence = HarmonicMean(char_recog_probs, prob_weights);
}


//! 番号の認識
float LPRecog::_RecognizeChars(const CharRecognition& char_recog, const std::map<int, std::string>& char_map,
	const cv::Mat& img, const std::vector<cv::Rect>& char_positions,
	std::vector<int>& recog_ids, std::string& recog_string) 
{
	if (!char_recog.isReady())
		return 0;

	recog_string.clear();
	std::vector<float> probs;
	std::vector<cv::Rect>::const_iterator char_i, char_e = char_positions.end();
	for (char_i = char_positions.begin(); char_i != char_e; char_i++) {
		float prob;
		int recog_id = char_recog.Predict(img(*char_i), prob);
		probs.push_back(prob);
		//int recog_id = char_recog.Predict(img(*char_i));
		recog_ids.push_back(recog_id);
		std::map<int, std::string>::const_iterator it = char_map.find(recog_id);
		if (it != char_map.end())
			recog_string = recog_string + it->second;
	}
	return HarmonicMean(probs);
}


//! 番号の認識
float LPRecog::_RecognizeChar(const CharRecognition& char_recog, const std::map<int, std::string>& char_map,
	const cv::Mat& img, const cv::Rect& char_position,
	int& id, std::string& recog_string) 
{
	if (char_position.width <= 0 || char_position.height <= 0)
		return 0;

	float prob;
	id = char_recog.Predict(img(char_position), prob);
	recog_string = char_map.at(id);
	return prob;
}


bool LPRecog::_AddWordId(std::map<int, std::string>& id_name_map, int id, const std::string& name)
{
	std::pair<std::map<int, std::string>::iterator, bool> ret
		= id_name_map.insert(std::map<int, std::string>::value_type(id, name));

	return ret.second;
}


bool LPRecog::_LoadWordId(std::map<int, std::string>& id_name_map, const std::string& filename)
{
	std::ifstream ifs(filename);
	if(!ifs.is_open())
		return false;

	id_name_map.clear();

	int line = 1;
	std::string buf;
	while(ifs && std::getline(ifs, buf)){
		std::string::size_type	index = buf.find(",");
		if(index == std::string::npos)
			continue;
		int id = std::atoi(buf.substr(0, index).c_str());
		std::string::size_type name_size = buf.size() - index - 1;
		std::string name = buf.substr(index+1, name_size);
		if(!_AddWordId(id_name_map, id, name)){
			std::cerr << "Fail to read line " << line << "." << std::endl;
		};
		line++;
	}
	return true;
}


bool LPRecog::_SaveWordId(const std::map<int, std::string>& id_name_map, const std::string& filename)
{
	std::ofstream ofs(filename);
	if(!ofs.is_open())
		return false;

	std::map<int, std::string>::const_iterator it = id_name_map.begin();
	for(;it != id_name_map.end(); it++){
		ofs << it->first << "," << it->second << std::endl;
	}
	return true;
}

//! 車両番号の有効性チェック
/*!
dotは頭に連続する場合のみ許される
*/
bool LPRecog::_ValidCarNumberId(const std::vector<int>& car_number_ids, int max_valid_id)
{
	int prev_dot_id = -1;
	for (int i = 0;i < car_number_ids.size();i++) {
		if (car_number_ids[i] > max_valid_id) {
			if (i - 1 == prev_dot_id) {
				prev_dot_id = i;
			}
			else
				return false;
		}
	}

	if (prev_dot_id == car_number_ids.size() - 1)
		return false;

	return true;
}


//! 分類番号の有効性チェック
/*!
中板プレートで「尾張小牧33」のような４文字地名、２桁以下分類番号の時、「尾張小」「牧33」のように抽出される。
これを修正。
*/
bool LPRecog::_CheckOwariKomakiClassNumber(LPContents& plate, 
	float& local_name_confidence, float& class_number_confidence) const
{
	if (plate.ClassNumberId.size() < 2)
		return false;

	size_t pos = plate.ClassNumber.find("牧");
	if (pos == std::string::npos) {
		return false;
	}
	else if (pos != 0) {
		class_number_confidence = 0;
		return false;
	}

	// 地名認識
	cv::Rect local_name_rect = CombineRect(plate.LocalNamePosition, plate.ClassNumberPositions[0]);
	int local_name_id;
	std::string local_name;
	float local_name_prob = _RecognizeChar(_LocalNameRecog, _LocalNameIds, plate.PlateImage,
		local_name_rect, local_name_id, local_name);

	if (local_name != "尾張小牧") {
		class_number_confidence = 0;
		return false;
	}

	// 結果を格納しなおす
	std::vector<cv::Rect> class_number_rects(plate.ClassNumberPositions.begin() + 1, plate.ClassNumberPositions.end());
	std::vector<int> class_number_ids(plate.ClassNumberId.begin() + 1, plate.ClassNumberId.end());
	int s = plate.ClassNumber.size() - class_number_ids.size();
	std::string class_number(plate.ClassNumber, s, class_number_ids.size());

	plate.ClassNumber = class_number;
	plate.ClassNumberId = class_number_ids;
	plate.ClassNumberPositions = class_number_rects;

	plate.LocalName = local_name;
	plate.LocalNameId = local_name_id;
	plate.LocalNamePosition = local_name_rect;
	local_name_confidence = local_name_prob;
	return true;
}


void LPRecog::RecogPlatesWithoutDetector(const cv::Mat& img, LPContents& plate) const
{
	if (img.type() == CV_8UC3) {
		cv::cvtColor(img, plate.PlateImage, cv::COLOR_BGR2GRAY);
	}
	else if (img.type() == CV_8UC1) {
		plate.PlateImage = img;
	}
	else {
		throw lpr::Exception(lpr::Exception::INVALID_DATA_TYPE, "Input image should be CV_8UC1 or CV_8UC3.", __FUNCTION__, __FILE__, __LINE__);
	}

	// 文字領域の切り出し
	_PlateSegmentation.ExtractCharRegions(plate.PlateImage,
		plate.LocalNamePosition, plate.ClassNumberPositions,
		plate.HiraganaPosition, plate.CarNumberPositions,
		plate.LargePlate);

	////// 文字認識 //////
	// 全ての領域の文字を認識してplateへ格納
	_RecognizeAllPlateChars(plate);
}

}