# Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
# SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

SET(APP_BIN deepin-font-manager)

#libdeepin-font-manager
SET(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libdeepin-font-manager)
FILE(GLOB LIB_SRC_FILES ${LIB_DIR}/*.cpp ${LIB_DIR}/*.h)
#INSTALL(FILES ${LIB_DIR}/schemas/com.deepin.font-manager.gschema.xml
#        DESTINATION ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)
#用deepin-turbo对Dapplication进行加速
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-needed -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

#可执行文件
FILE(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/views/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/views/*.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/*.h)

SET(QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../deepin-font-manager-assets/deepin-font-manager.qrc)
include_directories(${CMAKE_BINARY_DIR}/deepin-font-manager)
CONFIGURE_FILE(environments.h.in environments.h @ONLY)

#编译翻译文件
file(GLOB TS LIST_DIRECTORIES false ../translations/*.ts)
set_source_files_properties(${TS} PROPERTIES OUTPUT_LOCATION ${PROJECT_SOURCE_DIR}/translations)
qt_create_translation(QM ${SRC_FILES} ${TS})
ADD_EXECUTABLE(${APP_BIN} ${QM})
#安装翻译文件
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../translations
    DESTINATION share/${APP_BIN}
    FILES_MATCHING PATTERN "*.qm")

TARGET_SOURCES(${APP_BIN} PRIVATE ${SRC_FILES} ${QRC_FILES} ${LIB_SRC_FILES})
TARGET_INCLUDE_DIRECTORIES(${APP_BIN} PRIVATE ${LIB_DIR})
TARGET_INCLUDE_DIRECTORIES(${APP_BIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_INCLUDE_DIRECTORIES(${APP_BIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/views)
TARGET_INCLUDE_DIRECTORIES(${APP_BIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)

TARGET_LINK_LIBRARIES(${APP_BIN} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Network
    Dtk${DTK_VERSION}::Core
    Dtk${DTK_VERSION}::Gui
    Dtk${DTK_VERSION}::Widget
    ${FREETYPE_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    PkgConfig::FontConfig)

INSTALL(TARGETS ${APP_BIN} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

SET(DESKTOP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deepin-font-manager-assets/deepin-font-manager.desktop)
INSTALL(FILES ${DESKTOP_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

SET(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deepin-font-manager-assets/deepin-font-manager.svg)
INSTALL(FILES ${ICON_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

SET(PREVIEW_CONTENT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deepin-font-manager-assets/contents.txt)
INSTALL(FILES ${PREVIEW_CONTENT_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APP_BIN}/)
