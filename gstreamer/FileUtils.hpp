/*********************************************************************************
* Copyright(C),XXX
* FileName:    FileUtils.hpp
* Author:      YYY
* Date:        2024-08-01
* Description: The declaration of FileUtils
*
*********************************************************************************/
#ifndef __COMMON_FILEUTILS_HPP__
#define __COMMON_FILEUTILS_HPP__

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/
#include <string>

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
*                      Class                                                     *
**********************************************************************************/

class FileUtils
{
public:
    FileUtils() = default;
    ~FileUtils() = default;

    static bool isDirExist(const std::string& path);
    static bool createDir(const std::string& path);
    static bool isFileExist(const std::string& path);
};

} // namespace video
} // namespace capture
#endif // __COMMON_FILEUTILS_HPP__