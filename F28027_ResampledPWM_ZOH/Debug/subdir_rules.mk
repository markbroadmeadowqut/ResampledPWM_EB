################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
CMPA_calc.obj: ../CMPA_calc.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/bin/cl2000" -v28 -ml -mt --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_common/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_headers/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_common/source" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="CMPA_calc.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/bin/cl2000" -v28 -ml -mt --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_common/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_headers/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_common/source" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwm.obj: ../pwm.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/bin/cl2000" -v28 -ml -mt --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_common/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_headers/include" --include_path="H:/Git/ResampledPWM_EB/F28027_ResampledPWM_ZOH/DSP2802x_common/source" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.3.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="pwm.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


