#ifndef __LPRECOG_LIB__
#define __LPRECOG_LIB__

#if WIN32
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define DLLEXPORT extern "C"
#endif

#include "LPRecog.h"

//! 新しいインスタンスを生成
DLLEXPORT lpr::LPRecog* new_LPRecog();

//! インスタンスを削除
DLLEXPORT void delete_LPRecog(lpr::LPRecog* h_LPRecog);

//! 設定ファイルの読み込み
DLLEXPORT int initialize(lpr::LPRecog* h_LPRecog, 
	const char* detector_file, const char* shape_file,
	const char* local_name_filter, const char* local_name_id,
	const char* class_number_filter, const char* class_number_id,
	const char* hiragana_filter, const char* hiragana_id,
	const char* car_number_filter, const char* car_number_id
	);


//! 認識
/*!
\param[in] h_LPRecog ハンドル
\param[in] image_data_ptr 画像ポインタ
\param[in] image_width 画像幅
\param[in] image_height 画像高さ
\param[in] image_type 画像型
\param[in] min_plate_width 検出最小幅(pixel)
\param[in] max_plate_width 検出最大幅(pixel)
\param[in] num_plates 最大検出数
\param[in] threshold 閾値
\return 結果クラスへのハンドル
*/
DLLEXPORT void* Recog(lpr::LPRecog* h_LPRecog, 
	void* image_data_ptr, int image_width, int image_height, int image_type,
	int min_plate_width, int max_plate_width, int num_plates, float threshold);


//! 検出結果を削除
DLLEXPORT void delete_PlateInfos(void* h_plate_infos);

//! 検出したナンバープレートの数
DLLEXPORT int NumDetected(void* h_plate_infos);

//! id番目の結果の地名を取得
DLLEXPORT const char* GetLocalName(void* h_plate_infos, int id);

//! id番目の結果の分類番号を取得
DLLEXPORT const char* GetClassNumber(void* h_plate_infos, int id);

//! id番目の結果のひらがなを取得
DLLEXPORT const char* GetHiragana(void* h_plate_infos, int id);

//! id番目の結果の車両番号を取得
DLLEXPORT const char* GetCarNumber(void* h_plate_infos, int id);

//! id番目の結果のプレートコーナーを取得
//DLLEXPORT const float* GetPlateCorners(void* h_plate_infos, int id);
DLLEXPORT void GetPlateCorners(void* h_plate_infos, int id, float* dst_corners);

#endif