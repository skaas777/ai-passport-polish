idf_component_register(
    SRCS "main.c" "polish_course.c" "polish_study.c" "polish_progress.c"
         "polish_learn.c" "polish_audio.c"
         "../assets/fonts/font_cn_16.c"
         "../assets/fonts/font_polish_20.c"
         "../assets/fonts/font_ipa_16.c"
    INCLUDE_DIRS "."
    REQUIRES bsp esp_hw_support esp_timer nvs_flash
)

# Only the WAV sources are tracked. Pack once in course order, in flash.
set(audio_dir "${CMAKE_CURRENT_LIST_DIR}/../assets/audio")
file(GLOB recordings CONFIGURE_DEPENDS "${audio_dir}/raw/*.wav")
idf_build_get_property(python PYTHON)
set(bank "${CMAKE_CURRENT_BINARY_DIR}/polish_bank.bin")
add_custom_command(OUTPUT "${bank}"
    COMMAND "${python}" "${audio_dir}/pack_audio.py" "${bank}"
    DEPENDS "${audio_dir}/pack_audio.py" "${audio_dir}/words.txt"
            "${CMAKE_CURRENT_LIST_DIR}/polish_course.c" ${recordings}
    VERBATIM)
target_add_binary_data(${COMPONENT_LIB} "${bank}" BINARY)
