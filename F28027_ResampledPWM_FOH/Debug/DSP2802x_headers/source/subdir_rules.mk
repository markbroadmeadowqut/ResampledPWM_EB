################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP2802x_headers/source/DSP2802x_GlobalVariableDefs.obj: ../DSP2802x_headers/source/DSP2802x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/bin/cl2000" -v28 -ml -mt --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_FOH/DSP2802x_common/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_FOH/DSP2802x_headers/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_FOH/DSP2802x_common/source" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/include" -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="DSP2802x_headers/source/DSP2802x_GlobalVariableDefs.d" --obj_directory="DSP2802x_headers/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


