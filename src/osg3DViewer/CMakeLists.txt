SET(EXE_NAME osg3DViewer)


SET(MYAPP_VERSION "1.0.0")

FILE(GLOB include_file_list "./*.h" "./CollisionDetect/*.h" "./3rdparty/qtpropertybrowser/*.h" "./3rdparty/osgWorks/osgwTools/*.h")
FILE(GLOB cpp_file_list "./*.cpp" "./CollisionDetect/*.cpp" "./3rdparty/qtpropertybrowser/*.cpp" "./3rdparty/osgWorks/osgwTools/*.cpp")

# MainWindow2.h is an unused backup copy of MainWindow.h - must stay out of moc
LIST(FILTER include_file_list EXCLUDE REGEX "MainWindow2\\.h$")
FILE(GLOB resource_file_list "../../res/*.qrc")
FILE(GLOB ui_file_list "../../ui/*.ui")
FILE(GLOB trans_file_list "../../lang/*.ts")


SET(UIS
    ${ui_file_list}
)
SET(HEADERS
    ${include_file_list}
)
SET(SOURCES
    ${cpp_file_list}
)
SET(TRANSLATIONS
    ${trans_file_list}
)
SET(RESOURCES
    ${resource_file_list}
)


# qt4 definitinos
QT4_WRAP_UI         (APP_UIS_H  ${UIS})
QT4_WRAP_CPP        (APP_MOC_SRCS ${HEADERS})
QT4_ADD_RESOURCES   (APP_RESOURCES ${RESOURCES})
#QT4_AUTOMOC         (${SOURCES})
QT4_ADD_TRANSLATION (APP_QM ${TRANSLATIONS})

INCLUDE_DIRECTORIES(include "./" "./CollisionDetect" "./3rdparty/qtpropertybrowser" "./3rdparty/osgWorks" ${PROJECT_BINARY_DIR}/src/${PROJECT_NAME})

# Prebuilt custom SDK: osgDragger / osgVirtualAssembly / osgCollisionDetection / Utils (Win32)
SET(OSGVA_SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/osgVirtualAssembly)
# Bullet Physics 2.88 headers (required by osgCollisionDetection headers)
# - src/            : headers from bullet3 tag 2.88
# - <root>          : provides Demos/OpenGL/GLDebugDrawer.h (from tag 2.82, needed by DebugDraw)
SET(BULLET_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/bullet-2.88/src
                        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/bullet-2.88)
INCLUDE_DIRECTORIES(${OSGVA_SDK_DIR}/include ${BULLET_INCLUDE_DIRS})

# resource compilation for MinGW
IF(MINGW)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}ico.o 
        COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../res/${EXE_NAME}.rc -o ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}ico.o)
    SET(APP_RESOURCES ${APP_RESOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}ico.o)
ELSE(MINGW)
    SET(APP_RESOURCES ${APP_RESOURCES} ../../res/${EXE_NAME}.rc)
ENDIF(MINGW)

# Set some Win32 Specific Settings
IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)

IF (MSVC)
    # Turn off deprecation warnings
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNINGS)
    # Keep debug info in Release builds (PDB) for crash diagnosis
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
    # Statically link the CRT to avoid loader conflicts with the prebuilt
    # VS2010-era runtime DLLs (Qt4 / OSG 3.4.1 / custom SDK) in the same folder
    STRING(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    STRING(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
ENDIF(MSVC)

ADD_EXECUTABLE(${EXE_NAME} WIN32 ${SOURCES} ${HEADERS} ${APP_UIS_H} ${APP_MOC_SRCS} ${APP_RESOURCES} ${APP_QM})

TARGET_LINK_LIBRARIES(${EXE_NAME} ${QT_LIBRARIES}
	debug OpenThreads${OSG_DEBUG_POSTFIX} optimized OpenThreads
    debug osg${OSG_DEBUG_POSTFIX}         optimized osg
	debug osgGA${OSG_DEBUG_POSTFIX}       optimized osgGA
	debug osgViewer${OSG_DEBUG_POSTFIX}   optimized osgViewer
	debug osgShadow${OSG_DEBUG_POSTFIX}   optimized osgShadow
	debug osgFX${OSG_DEBUG_POSTFIX}       optimized osgFX
	debug osgQt${OSG_DEBUG_POSTFIX}       optimized osgQt
    debug osgDB${OSG_DEBUG_POSTFIX}       optimized osgDB
    debug osgUtil${OSG_DEBUG_POSTFIX}     optimized osgUtil
    debug osgText${OSG_DEBUG_POSTFIX}     optimized osgText
    debug osgManipulator${OSG_DEBUG_POSTFIX} optimized osgManipulator
    # prebuilt custom SDK (Win32 x86)
    debug ${OSGVA_SDK_DIR}/lib/osgDraggerd.lib            optimized ${OSGVA_SDK_DIR}/lib/osgDragger.lib
    debug ${OSGVA_SDK_DIR}/lib/osgVirtualAssemblyd.lib    optimized ${OSGVA_SDK_DIR}/lib/osgVirtualAssembly.lib
    debug ${OSGVA_SDK_DIR}/lib/osgCollisionDetectiond.lib optimized ${OSGVA_SDK_DIR}/lib/osgCollisionDetection.lib
    winmm)


IF(WIN32)
    SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/ENTRY:\"mainCRTStartup\" /SUBSYSTEM:WINDOWS"
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
ENDIF(WIN32)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#include_directories(${PROJECT_SOURCE_DIR}/libhello)
link_directories(${PROJECT_BINARY_DIR}/lib)

SET(INSTALL_INCDIR include)
SET(INSTALL_BINDIR bin)
IF(WIN32)
    SET(INSTALL_LIBDIR bin)
    SET(INSTALL_ARCHIVEDIR lib)
ELSE(WIN32)
    SET(INSTALL_LIBDIR lib${LIB_POSTFIX})
    SET(INSTALL_ARCHIVEDIR lib${LIB_POSTFIX})
ENDIF(WIN32)

INSTALL(
    TARGETS ${EXE_NAME}
    RUNTIME DESTINATION ${INSTALL_BINDIR}
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR}
)

INSTALL(
    FILES       ${HEADERS}
    DESTINATION ${INSTALL_INCDIR}/${EXE_NAME}
)

# Copy custom SDK DLLs next to the executable (both configs, harmless)
IF(WIN32)
    FOREACH(_dll osgDragger osgDraggerd osgVirtualAssembly osgVirtualAssemblyd osgCollisionDetection osgCollisionDetectiond)
        ADD_CUSTOM_COMMAND(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OSGVA_SDK_DIR}/lib/${_dll}.dll"
                "$<TARGET_FILE_DIR:${EXE_NAME}>")
    ENDFOREACH()
ENDIF(WIN32)
