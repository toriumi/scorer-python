#ifndef __LPRECOG__
#define __LPRECOG__

#include <opencv2/core.hpp>
#include <map>
#include "PlateDetection.h"
#include "PlateSegmentation.h"
#include "CharRecognition.h"

namespace lpr{

class LPRecog
{
public:
	//! ナンバープレート認識結果
	typedef struct _LPContents{
		float Confidence;	//!< 確度[0-1]
		float Threshold; //!< しきい値
		cv::Rect PlatePosition;	//!< プレートの場所
		std::vector<cv::Point2f> PlateCorners; //! プレートの詳細位置（四隅のコーナー：左上から時計回り）
		cv::Rect LocalNamePosition;	//!< 地名の場所
		cv::Rect HiraganaPosition;	//!< ひらがなの場所
		std::vector<cv::Rect> ClassNumberPositions;	//!< 分類番号の場所
		std::vector<cv::Rect> CarNumberPositions;	//!< 車両番号の場所
		std::vector<int> CarNumberId;	//!< 車両番号ID
		std::string CarNumber;	//!< 車両番号
		std::vector<int> ClassNumberId;	//!< 分類番号ID
		std::string ClassNumber;	//!< 分類番号
		int LocalNameId;	//!< 地名ID
		std::string LocalName;	//!< 地名
		int HiraganaId;	//!< ひらがなID
		std::string Hiragana;	//!< ひらがな
		cv::Mat PlateImage;	//!< 切り取って変形したプレート画像
		bool Commercial;	//!< true=営業車, false=自家用車
		bool LargePlate;	//!< true=大板プレート, false=中板プレート

		bool operator<( const struct _LPContents& right ) const{
			return Confidence < right.Confidence;
		}
		bool operator>( const struct _LPContents& right ) const{
			return Confidence > right.Confidence;
		}
	}LPContents;

private:
	PlateDetection _PlateDetection;	//!< ナンバープレート検出器
	PlateSegmentation _PlateSegmentation;	//!< ナンバープレートから文字を分離
	CharRecognition _CarNumberRecog;	//!< 車両番号認識
	CharRecognition _ClassNumberRecog;	//!< 分類番号認識
	CharRecognition _HiraganaRecog;	//!< ひらがな認識
	CharRecognition _LocalNameRecog;	//!< 地名認識

	std::map<int, std::string> _LocalNameIds;	//!< クラスIDと地名のヒモ付
	std::map<int, std::string> _ClassNumberIds;	//!< 分類番号IDと分類番号のヒモ付
	std::map<int, std::string> _HiraganaIds;	//!< ひらがなIDと地名のヒモ付
	std::map<int, std::string> _CarNumberIds;	//!< 車両番号IDと車両番号のヒモ付
	
public:
	LPRecog(void);
	~LPRecog(void);

	//! ナンバープレート認識器の初期化
	/*!
	\param[in] cascade_file 検出用の学習済みパラメータファイル
	*/
	bool LoadDetector(const std::string& cascade_file){
		return _PlateDetection.Init(cascade_file);
	};

	//! ナンバープレート認識器の初期化
	/*!
	\param[in] cascade_file 検出用の学習済みパラメータファイル
	\param[in] shape_predictor_file 形状推定用の学習済みパラメータファイル
	*/
	bool LoadDetector(const std::string& cascade_file, const std::string& shape_predictor_file) {
		return _PlateDetection.Init(cascade_file, shape_predictor_file);
	};


	const PlateDetection* GetDetector() const{
		return  &_PlateDetection;
	};

	//! ナンバープレート認識
	/*!
	\param[in] img 入力画像
	\param[out] detected_plates 検出されたプレート情報
	\param[in] min_plate_width 最小プレートサイズ
	\param[in] max_plate_width 最大プレートサイズ
	\param[in] roi 処理領域
	\param[in] num_plates 検出するナンバープレートの最大数
	\param[in] threshold しきい値(0-1までの値)
	\param[in] ivert_img 例えば緑地に白のような反転パターンを検出するかどうか
	*/
	void RecogPlates(const cv::Mat& img, std::vector<LPContents>& detected_plates,
		int min_plate_width = 0, int max_plate_width = 0, const cv::Rect& roi = cv::Rect(), 
		int num_plates = -1, float threshold = -1.0, bool invert_img = true) const;


	//! 各OCRクラスを取得
	const CharRecognition& GetCarNumberRecog() const{
		return _CarNumberRecog;
	}

	const CharRecognition& GetClassNumberRecog() const{
		return _ClassNumberRecog;
	}

	const CharRecognition& GetHiraganaRecog() const{
		return _HiraganaRecog;
	}

	const CharRecognition& GetLocalNameRecog() const{
		return _LocalNameRecog;
	}
	///////////// OCR ///////////////
	//! 車両番号
	bool LoadCarNumberData(const std::string& car_num_file){
		return _CarNumberRecog.Load(car_num_file);
	};

	bool SaveCarNumberData(const std::string& save_file) const{
		return _CarNumberRecog.Save(save_file);
	}

	//! 分類番号
	bool LoadClassNumberData(const std::string& class_num_file){
		return _ClassNumberRecog.Load(class_num_file);
	};

	bool SaveClassNumberData(const std::string& save_file) const{
		return _ClassNumberRecog.Save(save_file);
	}

	//! ひらがな
	bool LoadHiraganaData(const std::string& hiragana_file){
		return _HiraganaRecog.Load(hiragana_file);
	};

	bool SaveHiraganaData(const std::string& save_file) const{
		return _HiraganaRecog.Save(save_file);
	};

	//! 地名
	bool LoadLocalNameData(const std::string& local_name_file){
		return _LocalNameRecog.Load(local_name_file);
	};

	bool SaveLocalNameData(const std::string& save_file) const{
		return _LocalNameRecog.Save(save_file);
	};

	//! IDと地名の対応付け
	bool AddLocalNameId(int id, const std::string& name){
		return _AddWordId(_LocalNameIds, id, name);
	};

	bool LoadLocalNameId(const std::string& load_file){
		return _LoadWordId(_LocalNameIds, load_file);
	};

	bool SaveLocalNameId(const std::string& save_file) const{
		return _SaveWordId(_LocalNameIds, save_file);
	};

	//! IDと分類番号の対応付け
	bool AddClassNumberId(int id, const std::string& name) {
		return _AddWordId(_ClassNumberIds, id, name);
	};

	bool LoadClassNumberId(const std::string& load_file) {
		return _LoadWordId(_ClassNumberIds, load_file);
	};

	bool SaveClassNumberId(const std::string& save_file) const {
		return _SaveWordId(_ClassNumberIds, save_file);
	};

	//! IDとひらがなの対応付け
	bool AddHiraganaId(int id, const std::string& name){
		return _AddWordId(_HiraganaIds, id, name);
	};

	bool LoadHiraganaId(const std::string& load_file){
		return _LoadWordId(_HiraganaIds, load_file);
	};

	bool SaveHiraganaId(const std::string& save_file) const{
		return _SaveWordId(_HiraganaIds, save_file);
	};

	//! IDと車両番号の対応付け
	bool AddCarNumberId(int id, const std::string& name) {
		return _AddWordId(_CarNumberIds, id, name);
	};

	bool LoadCarNumberId(const std::string& load_file) {
		return _LoadWordId(_CarNumberIds, load_file);
	};

	bool SaveCarNumberId(const std::string& save_file) const {
		return _SaveWordId(_CarNumberIds, save_file);
	};


	//! 検出器およびコーナー検出を使用せずナンバープレート認識
	/*!
	テスト用。
	*/
	void RecogPlatesWithoutDetector(const cv::Mat& img, LPContents& result) const;

private:
	//! ナンバープレート認識
	/*!
	全てのナンバープレート候補が抽出される
	\param[in] img 入力画像
	\param[out] detected_plates 検出されたプレート情報
	\param[in] min_plate_width 最小プレートサイズ
	\param[in] max_plate_width 最大プレートサイズ
	\param[in] roi 処理領域
	\param[in] threshold しきい値(0-1までの値)
	*/
	void _RecogAllPlates(const cv::Mat& img, std::vector<LPContents>& detected_plates,
		int min_plate_width = 0, int max_plate_width = 0, const cv::Rect& roi = cv::Rect(), 
		float threshold = -1) const;

	//! 営業車フラグの設定
	inline static void _SetCommercialFlag(std::vector<LPContents>& detected_plates, bool flag) {
		for(std::vector<LPContents>::iterator it = detected_plates.begin(); it != detected_plates.end(); it++)
			it->Commercial = flag;
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
	void _RecognizeAllPlateChars(LPContents& plate) const;
	
	//! 番号の認識
	/*!
	\param[in] char_recog 文字認識器
	\param[in] img 入力濃淡画像
	\param[in] char_positions 文字位置
	\param[out] ids 認識された文字ID
	\param[out] recog_string 認識された文字列
	\return スコア（0-1）
	*/
	static float _RecognizeChars(const CharRecognition& char_recog, const std::map<int, std::string>& char_map,
		const cv::Mat& img, const std::vector<cv::Rect>& char_positions, 
		std::vector<int>& ids, std::string& recog_string);

	//! 番号の認識
	/*!
	\param[in] char_recog 文字認識器
	\param[in] img 入力濃淡画像
	\param[in] char_position 文字位置
	\param[out] id 認識された文字ID
	\param[out] recog_string 認識された文字列
	\return スコア（0-1）
	*/
	static float _RecognizeChar(const CharRecognition& char_recog, const std::map<int, std::string>& char_map,
		const cv::Mat& img, const cv::Rect& char_position,
		int& id, std::string& recog_string);


	//! 認識IDと単語のヒモ付
	static bool _AddWordId(std::map<int, std::string>& word_map, int id, const std::string& word);

	static bool _LoadWordId(std::map<int, std::string>& word_map, const std::string& filename);

	static bool _SaveWordId(const std::map<int, std::string>& word_map, const std::string& filename);

	//! 車両番号の有効性チェック
	static bool _ValidCarNumberId(const std::vector<int>& car_number_ids, int max_valid_id = 9);

	static bool _ValidNumberString(const std::string& car_number) {
		return !(car_number[0] == '0' || car_number == "牧");
	}

	//! 分類番号の有効性チェック
	/*!
	中板プレートで「尾張小牧33」のような４文字地名、２桁以下分類番号の時、「尾張小」「牧33」のように抽出される。
	これを修正。
	*/
	bool _CheckOwariKomakiClassNumber(LPContents& plate,
		float& local_name_confidence, float& class_number_confidence) const;

	//! 調和平均
	template <typename T>
	static double HarmonicMean(const std::vector<T>& values, 
		const std::vector<float>& weights = std::vector<float>())
	{
		std::vector<float> w = weights.empty() ? std::vector<float>(values.size(), 1.0) : weights;
		assert(w.size() == values.size());

		double num_values = 0;
		for (int i = 0;i < w.size();i++)
			num_values += w[i];
		double sum = 0;
		for (int i = 0;i < values.size();i++) {
			if (values[i] == 0)
				return 0;
			sum += w[i] / values[i];
		}
		return num_values / sum;
	}
};

}

#endif