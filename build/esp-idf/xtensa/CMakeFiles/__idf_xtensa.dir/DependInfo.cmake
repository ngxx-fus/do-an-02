
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/codespace/esp-idf/components/xtensa/xtensa_context.S" "/workspaces/do-an-02/build/esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_context.S.obj"
  "/home/codespace/esp-idf/components/xtensa/xtensa_intr_asm.S" "/workspaces/do-an-02/build/esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_intr_asm.S.obj"
  "/home/codespace/esp-idf/components/xtensa/xtensa_vectors.S" "/workspaces/do-an-02/build/esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_vectors.S.obj"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "ESP_PLATFORM"
  "IDF_VER=\"v5.5\""
  "SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE"
  "SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ"
  "_GLIBCXX_HAVE_POSIX_SEMAPHORE"
  "_GLIBCXX_USE_POSIX_SEMAPHORE"
  "_GNU_SOURCE"
  "_POSIX_READER_WRITER_LOCKS"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "config"
  "/home/codespace/esp-idf/components/xtensa/esp32s3/include"
  "/home/codespace/esp-idf/components/xtensa/include"
  "/home/codespace/esp-idf/components/xtensa/deprecated_include"
  "/home/codespace/esp-idf/components/newlib/platform_include"
  "/home/codespace/esp-idf/components/freertos/config/include"
  "/home/codespace/esp-idf/components/freertos/config/include/freertos"
  "/home/codespace/esp-idf/components/freertos/config/xtensa/include"
  "/home/codespace/esp-idf/components/freertos/FreeRTOS-Kernel/include"
  "/home/codespace/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include"
  "/home/codespace/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include/freertos"
  "/home/codespace/esp-idf/components/freertos/esp_additions/include"
  "/home/codespace/esp-idf/components/esp_hw_support/include"
  "/home/codespace/esp-idf/components/esp_hw_support/include/soc"
  "/home/codespace/esp-idf/components/esp_hw_support/include/soc/esp32s3"
  "/home/codespace/esp-idf/components/esp_hw_support/dma/include"
  "/home/codespace/esp-idf/components/esp_hw_support/ldo/include"
  "/home/codespace/esp-idf/components/esp_hw_support/debug_probe/include"
  "/home/codespace/esp-idf/components/esp_hw_support/mspi_timing_tuning/include"
  "/home/codespace/esp-idf/components/esp_hw_support/mspi_timing_tuning/tuning_scheme_impl/include"
  "/home/codespace/esp-idf/components/esp_hw_support/power_supply/include"
  "/home/codespace/esp-idf/components/esp_hw_support/port/esp32s3/."
  "/home/codespace/esp-idf/components/esp_hw_support/port/esp32s3/include"
  "/home/codespace/esp-idf/components/esp_hw_support/mspi_timing_tuning/port/esp32s3/."
  "/home/codespace/esp-idf/components/esp_hw_support/mspi_timing_tuning/port/esp32s3/include"
  "/home/codespace/esp-idf/components/heap/include"
  "/home/codespace/esp-idf/components/heap/tlsf"
  "/home/codespace/esp-idf/components/log/include"
  "/home/codespace/esp-idf/components/soc/include"
  "/home/codespace/esp-idf/components/soc/esp32s3"
  "/home/codespace/esp-idf/components/soc/esp32s3/include"
  "/home/codespace/esp-idf/components/soc/esp32s3/register"
  "/home/codespace/esp-idf/components/hal/platform_port/include"
  "/home/codespace/esp-idf/components/hal/esp32s3/include"
  "/home/codespace/esp-idf/components/hal/include"
  "/home/codespace/esp-idf/components/esp_rom/include"
  "/home/codespace/esp-idf/components/esp_rom/esp32s3/include"
  "/home/codespace/esp-idf/components/esp_rom/esp32s3/include/esp32s3"
  "/home/codespace/esp-idf/components/esp_rom/esp32s3"
  "/home/codespace/esp-idf/components/esp_common/include"
  "/home/codespace/esp-idf/components/esp_system/include"
  "/home/codespace/esp-idf/components/esp_system/port/soc"
  "/home/codespace/esp-idf/components/esp_system/port/include/private"
  "/home/codespace/esp-idf/components/lwip/include"
  "/home/codespace/esp-idf/components/lwip/include/apps"
  "/home/codespace/esp-idf/components/lwip/include/apps/sntp"
  "/home/codespace/esp-idf/components/lwip/lwip/src/include"
  "/home/codespace/esp-idf/components/lwip/port/include"
  "/home/codespace/esp-idf/components/lwip/port/freertos/include"
  "/home/codespace/esp-idf/components/lwip/port/esp32xx/include"
  "/home/codespace/esp-idf/components/lwip/port/esp32xx/include/arch"
  "/home/codespace/esp-idf/components/lwip/port/esp32xx/include/sys"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/home/codespace/esp-idf/components/xtensa/eri.c" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/eri.c.obj" "gcc" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/eri.c.obj.d"
  "/home/codespace/esp-idf/components/xtensa/xt_trax.c" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xt_trax.c.obj" "gcc" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xt_trax.c.obj.d"
  "/home/codespace/esp-idf/components/xtensa/xtensa_intr.c" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_intr.c.obj" "gcc" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_intr.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
