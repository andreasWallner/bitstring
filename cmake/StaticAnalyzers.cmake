# SPDX-Identifier: MIT
# inspired by ...

function(target_enable_clang_tidy TARGET ANALYZER_VAR_NAME)
  if(NOT TARGET ${TARGET})
    message(SEND_ERROR "target_enable_clang_tidy must be passed a known target")
  endif()
  if(DEFINED ${ANALYZER_VAR_NAME})
    find_program(CLANGTIDY "${${ANALYZER_VAR_NAME}}")
    if(CLANGTIDY)
      set(CXX_CLANG_TIDY "${CLANGTIDY};-extra-arg=-Wno-unknown-warning-option")
      get_target_property(CXX_STANDARD ${TARGET} CXX_STANDARD)
      if(NOT "${CXX_STANDARD}" STREQUAL "CXX_STANDARD-NOTFOUND")
        if("${CMAKE_CXX_CLANG_TIDY_DRIVER_MODE}" STREQUAL "cl")
          set_target_properties(${TARGET} PROPERTIES C_CLANG_TIDY "${CXX_CLANG_TIDY}" CXX_CLANG_TIDY "${CXX_CLANG_TIDY};-extra-arg=/std:c++${CXX_STANDARD}")
        else()
          set_target_properties(${TARGET} PROPERTIES C_CLANG_TIDY "${CXX_CLANG_TIDY}" CXX_CLANG_TIDY "${CXX_CLANG_TIDY};-extra-arg=-std=c++${CXX_STANDARD}")
        endif()
        message(STATUS "Analyzer '${${ANALYZER_VAR_NAME}}' enabled for '${TARGET}'")
      else()
        set_target_properties(${TARGET} PROPERTIES C_CLANG_TIDY "${CXX_CLANG_TIDY}" CXX_CLANG_TIDY "${CXX_CLANG_TIDY}")
        # should anyhow be handled via compile_commands.json
        message(STATUS "Analyzer '${${ANALYZER_VAR_NAME}}' enabled for '${TARGET}' w/o explicit CXX standard")
      endif()
    else()
      message(WARNING "tried to enable clang-tidy, but '${${ANALYZER_VAR_NAME}}' could not be found")
    endif()
  else()
    message(STATUS  "analyzer not enabled")
    message(STATUS "analyzer: ${${ANALYZER_VAR_NAME}}")
  endif()
endfunction()
