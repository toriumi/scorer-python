#include "Exception.h"

namespace lpr{

using namespace std;

Exception::Exception() { code = LPR_ERROR; line = 0; }

Exception::Exception(ErrCode _code, const string& _err, const string& _func, const string& _file, int _line)
{
	code = _code;
	err = _err;
	func = _func;
	file = getSourceFileName(_file);
	line = _line;

    formatMessage();
}


Exception::~Exception(void) throw() {}

const char* Exception::what() const throw() { return msg.c_str(); }

std::string Exception::getSourceFileName(const std::string& file)
{
	std::string::size_type t;
	t = file.find_last_of("/");
	if(t == std::string::npos){
		t = file.find_last_of("\\");
	}

	if(t == std::string::npos){
		t = 0;
	}
	else{
		t++;
	}
	return file.substr(t);
}

void Exception::formatMessage()
{
	char msg_char[256];
	sprintf(msg_char, "%s:%d: error: (%s) %s in function %s\n", file.c_str(), line, getErrString().c_str(), err.c_str(), func.c_str());

	msg = string(msg_char);
}


std::string Exception::getErrString()
{
	switch(code){
	case LPR_ERROR:
		return "ERROR";
		break;
	case INVALID_ARGUMENT:
		return "INVALID_ARGUMENT";
		break;
	case IO_ERROR:
		return "IO_ARGUMENT";
		break;
	case INVALID_DATA_TYPE:
		return "INVALID_DATA_TYPE";
		break;
	case INVALID_APP:
		return "INVALID_APP";
		break;
	case INVALID_VERSION:
		return "INVALID_VERSION";
		break;
	case INVALID_FORMAT:
		return "INVALID_FORMAT";
		break;
	default:
		return "";
	}
}

}