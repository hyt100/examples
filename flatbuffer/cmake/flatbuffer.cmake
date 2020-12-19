function(COMPILE_FLATBUFFERS_SCHEMA_TO_CPP NEW_TARGET_NAME SRC_FBS)
  # get dir
  get_filename_component(SRC_FBS_DIR ${SRC_FBS} DIRECTORY)
  # get output name
  string(REGEX REPLACE "\\.fbs$" "_generated.h" GENERATE_FILENAME ${SRC_FBS})
  # run command
  add_custom_command(
    OUTPUT ${GENERATE_FILENAME}
    COMMAND flatc -c --no-includes --gen-mutable --gen-object-api --gen-compare 
          --no-prefix -o "${SRC_FBS_DIR}" 
          ${SRC_FBS}
  )
  # your target depends on xxx_generated.h, so that runs this command automatically.
  add_custom_target(${NEW_TARGET_NAME} ALL DEPENDS ${GENERATE_FILENAME})
endfunction()