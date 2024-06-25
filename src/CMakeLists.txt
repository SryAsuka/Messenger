cmake_minimum_required(VERSION 3.29)

project(
  Messenger
  VERSION 0.1
  LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Core Network Core5Compat Gui
                                         Widgets)

qt_standard_project_setup(REQUIRES 6.7)

qt_add_executable(appMessenger main.cpp)

target_sources(
  appMessenger
  PRIVATE ipmsg/filetransworker.cpp
          ipmsg/filetransworker.h
          ipmsg/ipmsgcommon.h
          ipmsg/ipmsgfileclient.h
          ipmsg/ipmsgfileclient.cpp
          ipmsg/ipmsgfilerecv.cpp
          ipmsg/ipmsgfilerecv.h
          ipmsg/ipmsgfileserver.cpp
          ipmsg/ipmsgfileserver.h
          ipmsg/ipmsgudpsession.cpp
          ipmsg/ipmsgudpsession.h
          ipmsg/ipmsguser.cpp
          ipmsg/ipmsguser.h
          ipmsg/ipmsgtcpsession.cpp
          ipmsg/ipmsgtcpsession.h
          ipmsg/ipmsgtcpserver.cpp
          ipmsg/ipmsgtcpserver.h
  PUBLIC)

qt_add_qml_module(
  appMessenger
  URI
  Messenger
  VERSION
  1.0
  QML_FILES
  Main.qml
  UserList.qml
  ChatForm.qml
  SOURCES
  messenger.h
  messenger.cpp
  SOURCES
  userlistmodel.h
  userlistmodel.cpp)

target_compile_features(appMessenger PRIVATE cxx_std_20)

set_target_properties(
  appMessenger
  PROPERTIES AUTORCC TRUE
             MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_link_libraries(
  appMessenger PRIVATE Qt6::Quick Qt6::Core Qt6::Core5Compat Qt6::Network
                       Qt6::Widgets)

include(GNUInstallDirs)
install(
  TARGETS appMessenger
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
