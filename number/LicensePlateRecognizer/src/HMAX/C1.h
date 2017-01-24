#ifndef __C1__
#define __C1__

#include <opencv2/core/core.hpp>

class C1
{
public:
	C1(void);
	virtual ~C1(void);

	void init();
	void init(const std::vector<int>& max_filter_size, int max_scale_layer, float overlap);
	void filter(const std::vector<std::vector<cv::Mat>>& input, std::vector<std::vector<cv::Mat>>& output, const std::pair<int,int>& layer_mask = std::pair<int,int>(-1,-1)) const;

	void Save(const std::string& filename) const;
	void Load(const std::string& filename);

	void Write(cv::FileStorage& file_storage, const std::string& node_name) const; 
	void Read(const cv::FileNode& file_node);

	int GetNumOfLayers() const{return NumOfLayers;};
	int GetMaxFilterScale() const{return MaxFilterScale;};
	float GetOverlapRatio() const{return OverlapRatio;};
	const std::vector<int>& GetFilterSizes() const{return MaxFilterSize;};

	// C1上のサイズをS1上のものに変換する。
	// 四捨五入の必要はなく，単に小数点以下を切り捨てれば良い。
	float calcSizeC1toS1(float C1_size, int layer_idx) const
	{
		return MaxFilterSize[layer_idx] * (C1_size + (1.0 - C1_size) * OverlapRatio);
	}

	float calcSizeC1toS1(float C1_size) const
	{
		return calcSizeC1toS1(C1_size, NumOfLayers-1);
	}

	// S1上のサイズをC1上のサイズへ変換
	float calcSizeS1toC1(float S1_size, int layer_idx) const
	{
		return (S1_size - OverlapRatio * MaxFilterSize[layer_idx]) / ((1.0 - OverlapRatio) * MaxFilterSize[layer_idx]);
	}

	float calcSizeS1toC1(float S1_size) const
	{
		return calcSizeS1toC1(S1_size, NumOfLayers - 1);
	}

	// S1上のサイズをC1上のサイズへ変換
	cv::Size2f calcSizeS1toC1(const cv::Size& S1_size, int layer_idx) const
	{
		float width = calcSizeS1toC1(S1_size.width, layer_idx);
		float height = calcSizeS1toC1(S1_size.height, layer_idx);
		return cv::Size2f(width, height);
	}

	cv::Size2f calcSizeS1toC1(const cv::Size& S1_size) const
	{
		return calcSizeS1toC1(S1_size, NumOfLayers - 1);
	}

	// S1上の座標をC1上のものに変換する。
	// 返り値がC1の幅以上の時は，幅-1に修正する必要
	float calcCoordS1toC1(float S1_coord, int layer_idx) const
	{
		return (S1_coord / MaxFilterSize[layer_idx] - 0.5) / (1.0 - OverlapRatio);
	}

	float calcCoordS1toC1(float S1_coord) const
	{
		return calcCoordS1toC1(S1_coord, NumOfLayers-1);
	}

	// C1上の座標をS1上のものに変換する。
	// 返り値が入力画像の幅以上の時は，幅-1に修正する必要
	float calcCoordC1toS1(int C1_coord, int layer_idx) const
	{
		return MaxFilterSize[layer_idx] * ((1.0 - OverlapRatio) * C1_coord + 0.5);
	}

	float calcCoordC1toS1(int C1_coord) const
	{
		return calcCoordC1toS1(C1_coord, NumOfLayers-1);
	}

	// C1上の座標をS1状のものに変換する。
	// C1上の座標はS1上ではある程度の幅を持つが、その端点の座標を返す。
	int calcCoordC1toS1Edge(int C1_coord, int layer_idx) const
	{
		return MaxFilterSize[layer_idx] * ((1.0 - OverlapRatio) * C1_coord);
	}

	int calcCoordC1toS1Edge(int C1_coord) const
	{
		calcCoordC1toS1Edge(C1_coord, NumOfLayers-1);
	}

	// C1上の矩形をS1上のものに変換する。
	cv::Rect calcRectC1toS1(const cv::Rect&  c1_rect, int layer_idx) const
	{
		cv::Rect s1_rect;
		s1_rect.width = calcSizeC1toS1(c1_rect.width, layer_idx);
		s1_rect.height = calcSizeC1toS1(c1_rect.height, layer_idx);
		s1_rect.x = calcCoordC1toS1Edge(c1_rect.x, layer_idx); 
		s1_rect.y = calcCoordC1toS1Edge(c1_rect.y, layer_idx); 
		return s1_rect;
	}

	cv::Rect calcRectC1toS1(const cv::Rect&  c1_rect) const
	{
		return calcRectC1toS1(c1_rect, NumOfLayers-1);
	}

protected:
	int NumOfLayers;

private:
	std::vector<int> MaxFilterSize;
	int MaxFilterScale;
	float OverlapRatio;
};

#endif