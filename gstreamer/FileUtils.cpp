/*********************************************************************************
* Copyright(C),XXX
* FileName:    FileUtils.cpp
* Author:      YYY
* Date:        2024-08-01
* Description: The implementation of FileUtils
*
*********************************************************************************/

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/
#include <sys/stat.h>
#include <FileUtils.hpp>

namespace capture {
namespace video {

/*********************************************************************************
*                      Macro                                                     *
**********************************************************************************/


/*********************************************************************************
*                      Enum/Struct                                               *
**********************************************************************************/


/*********************************************************************************
*                      Variable                                                  *
**********************************************************************************/


/*********************************************************************************
*                      Function                                                  *
**********************************************************************************/
bool FileUtils::isDirExist(const std::string& dir) {
    struct stat st;
    if (stat(dir.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool FileUtils::createDir(const std::string& dir) {
    if (FileUtils::isDirExist(dir)) {
        return true;
    }
#ifdef __linux__
    return mkdir(dir.c_str(), 0750) == 0;
#else
    return mkdir(dir.c_str()) == 0;
#endif
}

bool FileUtils::isFileExist(const std::string& file) {
    struct stat st;
    if (stat(file.c_str(), &st) == 0) {
        return S_ISREG(st.st_mode);
    }
    return false;
}

} // namespace video
} // namespace capture
