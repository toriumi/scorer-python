#ifndef __HMAX__
#define __HMAX__

#include "S1.h"
#include "C1.h"

class HMAX
{
public:
	HMAX(void);
	~HMAX(void);

	S1* hmax_s1;
	C1* hmax_c1;

	bool Save(const std::string& filename) const;
	bool Load(const std::string& filename);
	bool Write(cv::FileStorage& fs, const std::string& node_name = std::string()) const;
	bool Read(const cv::FileNode& fn);

	//! 処理対象画像をセット
	void SetProcImage(const cv::Mat& img){
		this->hmax_s1->SetProcImage(img);
	};

	//! 出力ベクトル群から訓練用データを生成
	static void GenerateQuery(const std::vector<std::vector<cv::Mat>>& outputs, cv::Mat& query_data);

	//! 出力ベクトル群を訓練用データへコピー
	static void CopyToTrainDataRow(const std::vector<std::vector<cv::Mat>>& outputs, cv::Mat& train_data, int idx);

	//! 出力ベクトル群から訓練用データを生成
	static void GenerateTrainData(const std::vector<std::vector<std::vector<cv::Mat>>>& outputs, cv::Mat& train_data);

	//! 信号をファイルに保存（主にデバッグ用）
	static void WriteSignal(cv::FileStorage& fs, const std::string& nodename, const std::vector<std::vector<cv::Mat>>& hmax_signal);
	static void SaveSignal(const std::string& filename, const std::vector<std::vector<cv::Mat>>& hmax_signal){
		cv::FileStorage fs(filename, cv::FileStorage::WRITE);
		WriteSignal(fs, "HMAX_Signal", hmax_signal);
	};

	//! 信号をファイルから読み込み（主にデバッグ用）
	static void ReadSignal(const cv::FileNode& file_node, std::vector<std::vector<cv::Mat>>& hmax_signal);
	static void LoadSignal(const std::string& filename, std::vector<std::vector<cv::Mat>>& hmax_signal){
		cv::FileStorage fs(filename, cv::FileStorage::READ);
		ReadSignal(fs["HMAX_Signal"], hmax_signal);
	};

	//! 信号をファイルにバイナリ形式で保存
	static bool WriteSignalBinary(std::ofstream& ofs, const std::vector<std::vector<cv::Mat>>& outputs);
	static bool SaveSignalBinary(const std::string& filename, const std::vector<std::vector<cv::Mat>>& outputs);

	//! 信号のバイナリファイルを読み込み
	static bool ReadSignalBinary(std::ifstream& ifs, std::vector<std::vector<cv::Mat>>& outputs);
	static bool LoadSignalBinary(const std::string& filename, std::vector<std::vector<cv::Mat>>& outputs);

};

#endif