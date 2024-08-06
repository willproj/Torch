set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/thirdParty/spdlog)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/thirdParty/glfw)
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/thirdParty/glad)
add_subdirectory(${SPDLOG_DIR})
add_subdirectory(${GLFW_DIR})
add_subdirectory(${GLAD_DIR})

