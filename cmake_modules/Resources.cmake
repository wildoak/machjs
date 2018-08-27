# https://beesbuzz.biz/code/4399-Embedding-binary-resources-with-CMake-and-C-11
function(add_resources out_var)
  set(result)
  foreach(in_file_path ${ARGN})
    file(RELATIVE_PATH in_file_path_relative ${CMAKE_SOURCE_DIR} ${in_file_path})
    
    set(ld_out ${PROJECT_BINARY_DIR}/${in_file_path_relative}.o)
    set(ld_in ${in_file_path_relative})

    get_filename_component(ld_out_directory ${ld_out} DIRECTORY)
    file(MAKE_DIRECTORY ${ld_out_directory})

    add_custom_command(
      OUTPUT ${ld_out}
      COMMAND ${CMAKE_LINKER} -r -b binary -o ${ld_out} ${ld_in}
      DEPENDS ${in_file_path}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "pack resource ${in_file_path}"
      VERBATIM
    )
    list(APPEND result ${ld_out})
  endforeach()
  set(${out_var} "${result}" PARENT_SCOPE)
endfunction()
