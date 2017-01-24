#include "RectFunctions.h"

//! はみ出る領域をカット
cv::Rect TruncateRect(const cv::Rect& obj_rect, const cv::Size& img_size)
{
	cv::Rect resize_rect = obj_rect;
	if(obj_rect.x < 0){
		resize_rect.x = 0;
		resize_rect.width += obj_rect.x;
	}
	if(obj_rect.y < 0){
		resize_rect.y = 0;
		resize_rect.height += obj_rect.y;
	}
	if(resize_rect.x + resize_rect.width > img_size.width){
		resize_rect.width = img_size.width - resize_rect.x;
		if (resize_rect.width < 0)
			resize_rect.width = 0;
	}
	if(resize_rect.y + resize_rect.height > img_size.height){
		resize_rect.height = img_size.height - resize_rect.y;
		if (resize_rect.height < 0)
			resize_rect.height = 0;
	}

	return resize_rect;
}


cv::Rect RescaleRectSize(const cv::Rect& src_rect, const cv::Size2f& rescale)
{
	float w = rescale.width * src_rect.width;
	float h = rescale.height * src_rect.height;
	float x = ((float)src_rect.width - w) / 2 + src_rect.x;
	float y = ((float)src_rect.height - h) / 2 + src_rect.y;
	return cv::Rect(x,y,w,h);
};

