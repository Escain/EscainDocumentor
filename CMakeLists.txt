#
# This file is part of Escain Documentor tool
# 
# Escain Documentor is free software: you can redistribute it and/or modify 
# it under ther terms of the GNU General Public License as published by 
# the Free Software Foundation, either version 3 of the License, or 
# (at your option) any later version.
# 
# Escain Documentor is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License 
# along with Escain Documentor. If not, see <https://www.gnu.org/licenses/>.
# 
# Author: Adrian Maire escain (at) gmail.com
#

cmake_minimum_required(VERSION 3.13)
cmake_policy( SET CMP0076 NEW )

project("EscainDocumentor")

macro( "SetupQtForProject" )
	set( CMAKE_AUTOMOC ON)
	set( CMAKE_AUTORCC ON)
	set( CMAKE_AUTOUIC ON)
	set( CMAKE_INCLUDE_CURRENT_DIR ON)
	set( QT_USE_QMAIN TRUE) #Avoid console, need WIN32 in the add_executable on Windows
	
	find_package(Qt5Widgets REQUIRED)
	add_definitions(${Qt5Widgets_DEFINITIONS})
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	include_directories(${Qt5Widgets_INCLUDE_DIRS})
	
	find_package(Qt5Xml REQUIRED)
	find_package(Qt5WebEngineWidgets REQUIRED)
	find_package(Qt5XmlPatterns REQUIRED)
endmacro()

function( "SetWarningLevel" TARGET)
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options( ${TARGET} PRIVATE -Wall -pedantic)
	endif()
	
	if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
		target_compile_options( ${TARGET} PRIVATE /W2)
	endif()
endfunction()

SetupQtForProject() #Before add_executable

set(TARGET "EscainDocumentor")
add_executable(${TARGET})

target_compile_features( ${TARGET} PUBLIC cxx_std_17)
SetWarningLevel(${TARGET})

# Add UIS
qt5_wrap_ui( UI_HEADERS "src/center.ui" )
qt5_add_resources( UI_RESOURCES "src/resources.qrc" )

#Build this project
set ( SOURCES 
	src/main.cpp 
	src/documentor.cpp 
	src/file_manager.cpp
	src/setup_documentor.cpp
	${UI_RESOURCES}
	src/resources.qrc
	src/center.ui 
	)

set ( HEADERS 
	src/documentor.hpp 
	src/escain_web_engine.hpp
	src/file_manager.hpp
	${UI_HEADERS}
	)

set ( LIBS  
	Qt5::Widgets
	Qt5::Xml
	Qt5::WebEngineWidgets
	Qt5::XmlPatterns
	)

# Project name
target_sources( ${TARGET} PRIVATE ${SOURCES} PUBLIC ${HEADERS})
target_link_libraries(${TARGET} ${LIBS})

if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OS X deployment version")
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES MACOSX_BUNDLE TRUE)
endif()

