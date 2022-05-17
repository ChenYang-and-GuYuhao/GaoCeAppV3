#
# 应用程序主体代码
#
add_library(APP OBJECT
  # 主函数
  gao_ce_ui.hpp gao_ce_ui.cpp
  GaoCeRunningUi.hpp GaoCeRunningUi.cpp GaoCeRunningUi.ui
  wrapper.hpp wrapper.cpp
  Worker.hpp Worker.cpp

  StatisReport.hpp StatisReport.cpp
)

target_link_libraries(APP PUBLIC _PCH
  Eyestack::Framework Eyestack::Gencom Eyestack::Com_MVS
  GaoCe::ALGO Qt5::PrintSupport Qt5::Charts
)

target_include_directories(APP INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_precompile_headers(APP REUSE_FROM _PCH)

#
# 应用程序可执行文件
#
add_executable(${PROJECT_NAME}
  main.cpp

  # 添加程序图标
  favicon.rc favicon.ico
)

target_link_libraries(${PROJECT_NAME} PRIVATE APP)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM _PCH)

# 部署规则
install(TARGETS ${PROJECT_NAME})

install_eyestack_runtime(${PROJECT_NAME} MODULE Gencom Com_MVS)
