set(SRCS
  src/formatters/default_formatter.cpp
  src/formatters/null_formatter.cpp
  src/loggers/file/file_logger.cpp
  src/loggers/stdout/stdout_logger.cpp
  src/core/async_consumer.cpp
  src/core/config.cpp
  src/core/config_parser.cpp
  src/core/formatter.cpp
  src/core/logger.cpp
  src/core/logger_engine.cpp
  src/core/sync_consumer.cpp
)

include_directories(
  include/simplelog
  include/simplelog/private
  src/core
)
