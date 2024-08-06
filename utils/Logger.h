#pragma once

#include <pch.h>

namespace utils
{
    class Logger {
    public:
        template<typename... Args>
        static void Trace(Args&&... args) {
            GetLogger()->trace(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Debug(Args&&... args) {
            GetLogger()->debug(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Info(Args&&... args) {
            GetLogger()->info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Warn(Args&&... args) {
            GetLogger()->warn(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Error(Args&&... args) {
            GetLogger()->error(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Critical(Args&&... args) {
            GetLogger()->critical(std::forward<Args>(args)...);
        }

    private:
        static std::shared_ptr<spdlog::logger> GetLogger() {
            static std::shared_ptr<spdlog::logger> logger = CreateLogger();
            return logger;
        }

        static std::shared_ptr<spdlog::logger> CreateLogger() {
            try {
                auto logger = spdlog::stdout_color_mt<spdlog::async_factory>("global_logger");
                logger->set_level(spdlog::level::trace);
                logger->set_pattern("[%^%H:%M:%S:%e] [%^%l%$] %v%$");

                return logger;
            }
            catch (const spdlog::spdlog_ex& ex) {
                std::cout << "Log initialization failed: " << ex.what() << std::endl;
                return nullptr;
            }
        }
    };
}

#ifdef ENABLE_LOGGING
#define TORCH_LOG_TRACE(...) utils::Logger::Trace(__VA_ARGS__)
#define TORCH_LOG_DEBUG(...) utils::Logger::Debug(__VA_ARGS__)
#define TORCH_LOG_INFO(...) utils::Logger::Info(__VA_ARGS__)
#define TORCH_LOG_WARN(...) utils::Logger::Warn(__VA_ARGS__)
#define TORCH_LOG_ERROR(...) utils::Logger::Error(__VA_ARGS__)
#define TORCH_LOG_CRITICAL(...) utils::Logger::Critical(__VA_ARGS__)
#else
#define TORCH_LOG_TRACE(...) do {} while (0)
#define TORCH_LOG_DEBUG(...) do {} while (0)
#define TORCH_LOG_INFO(...) do {} while (0)
#define TORCH_LOG_WARN(...) do {} while (0)
#define TORCH_LOG_ERROR(...) do {} while (0)
#define TORCH_LOG_CRITICAL(...) do {} while (0)
#endif
