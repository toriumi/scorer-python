#ifndef __LPR_EXCEPTION__
#define __LPR_EXCEPTION__

#include <exception>
#include <string>

namespace lpr
{

//! Open Multi View Framework用例外定義
class Exception : public std::exception
{
public:
	//! エラーコード
	typedef enum ErrCode{
		LPR_ERROR = 0,	//!< エラー全般
		INVALID_ARGUMENT = 1,	//!< 引数の不正
		IO_ERROR = 2,	//!< ファイル入出力エラー
		INVALID_DATA_TYPE = 3,	//!< データの型不正
		INVALID_APP = 11,	//!< アプリケーション不正
		INVALID_VERSION = 12,	//!< バージョン不正
		INVALID_FORMAT = 13,	//!< フォーマット不正
	}ErrCode;

	//! コンストラクタ
	Exception(void);

	//! コンストラクタ
	/*!
	\param[in] _code エラーコード
	\param[in] _err エラーメッセージ
	\param[in] _func エラー発生の関数名
	\param[in] _file エラー発生ソースファイル名
	\param[in] _line エラーが発生した行
	*/
    Exception(ErrCode _code, const std::string& _err, const std::string& _func, const std::string& _file, int _line);

	//! デストラクタ
	virtual ~Exception(void) throw();

	//! エラーメッセージ出力
    virtual const char *what() const throw();
    
    std::string msg; //!< エラーメッセージ

    ErrCode code; //!< エラーコード（ErrType）
    std::string err; //!< エラーメッセージ
    std::string func; //!< 関数名
    std::string file; //!< ファイル名
    int line; //!< エラーの発生した行数 

protected:
	void formatMessage();	//!< エラーメッセージのフォーマット
	
	//! エラーコードを文字列へ変換
	/*!
	\return エラーコードを変換した文字列
	*/
	std::string getErrString();

private:
	//! ファイルパスから、ファイル名のみ取得
	/*!
	\param[in] file ファイルパス
	\return ファイル名
	*/
	static std::string getSourceFileName(const std::string& file);
};

};

#endif
