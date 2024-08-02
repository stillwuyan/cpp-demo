/*********************************************************************************
* Copyright(C),XXX
* FileName:    Log.hpp
* Author:      YYY
* Date:        2024-07-30
* Description: The declaration of Log
*
*********************************************************************************/
#ifndef __COMMON_LOG_HPP__
#define __COMMON_LOG_HPP__

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>

namespace capture {
namespace video {

/*********************************************************************************
*                      Class                                                     *
**********************************************************************************/
class Log
{
public:
    template<typename ... Args>
    static std::string format( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ) {
            throw std::runtime_error( "Error during formatting." );
        }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }
};

/*********************************************************************************
*                      Macro                                                     *
**********************************************************************************/
#define LOG_DEBUG(...) \
    std::cout << "[DBG] " << capture::video::Log::format(__VA_ARGS__) << std::endl;
#define LOG_INFO(...) \
    std::cout << "[INF] " << capture::video::Log::format(__VA_ARGS__) << std::endl;
#define LOG_ERR(...) \
    std::cout << "[ERR] " << capture::video::Log::format(__VA_ARGS__) << std::endl;
#define LOG_WARN(...) \
    std::cout << "[WRN] " << capture::video::Log::format(__VA_ARGS__) << std::endl;

} // namespace video
} // namespace capture
#endif // __COMMON_LOG_HPP__
