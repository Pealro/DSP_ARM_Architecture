#include "stm32f4xx.h"
#include "signals.h"
#include "uart.h"
#include "stdio.h"
#include "arm_math.h"
#include "systick.h"

extern float _640_points_ecg_[HZ_5_SIG_LEN];
extern float32_t input_signal_f32_1kHz_15kHz[KHZ1_15_SIG_LEN];
extern float32_t  impulse_response[IMP_RSP_LEN];
float32_t output_signal[KHZ1_15_SIG_LEN + IMP_RSP_LEN - 1];

float g_in_sig_sample;
float g_imp_rsp_sample;
int iplot;

float32_t signal_2;
float32_t impulse;
float32_t output;

static void plot_input_signal(void);
static void pseudo_dly(int dly);
static void fpu_enable(void);
static void plot_impulse_response(void);
void plot_all_signals(void);
void convolution(float32_t *sig_src_arr,
				 float32_t *imp_rsp_arr,
				 float32_t *sig_dst_arr,
				 uint32_t	sig_src_lgt,
				 uint32_t	imp_rsp_lgt);

uint32_t g_before, g_after, g_time_taken;
float32_t g_seconds, g_miliseconds;

int main(){
	/*System Control Block points to Coprocessor Access Control Register,
	  habiliting the Floating Point Unit of CP10 and CP11 */
	fpu_enable();

	//Initialize the UART
	uart2_tx_init();

	//Initialize Systick
	systick_counter_init();


	//plot_impulse_response();
	//plot_input_signal();

	/*convolution((float32_t *)input_signal_f32_1kHz_15kHz,
				(float32_t *)impulse_response,
				(float32_t *)output_signal,
				(uint32_t)KHZ1_15_SIG_LEN,
				(uint32_t)IMP_RSP_LEN);*/

	g_before = SysTick->VAL;
	arm_conv_f32((float32_t *)input_signal_f32_1kHz_15kHz,
				 (uint32_t)KHZ1_15_SIG_LEN,
				 (float32_t *)impulse_response,
				 (uint32_t)IMP_RSP_LEN,
				 (float32_t *)output_signal
				);
	g_after = SysTick->VAL;
	g_time_taken = g_before - g_after;

	while(1){
		//plot_all_signals();
	}
}

void plot_all_signals(void){
	uint32_t i, j, k;
	i = j = 0;

	for(k = 0; k < (KHZ1_15_SIG_LEN + IMP_RSP_LEN - 1); k++){
		i++;
		j++;
		if(i == KHZ1_15_SIG_LEN){i = 0;}
		if(j == IMP_RSP_LEN){j = 0;}
		impulse = impulse_response[j];
		signal_2 = input_signal_f32_1kHz_15kHz[i];
		output = output_signal[k];
		pseudo_dly(9000);
	}
}
void convolution(float32_t *sig_src_arr,
				 float32_t *imp_rsp_arr,
				 float32_t *sig_otp_arr,
				 uint32_t	sig_src_lgt,
				 uint32_t	imp_rsp_lgt){
	uint32_t i, j;
	float32_t sig_total_lenght = sig_src_lgt + imp_rsp_lgt - 1;

	/*Clear output signal buffer*/
	for(i = 0; i < sig_total_lenght; i++){
		sig_otp_arr[i] = 0;
	}

	for(i = 0; i < sig_src_lgt; i++){
		for(j = 0; j < imp_rsp_lgt; j++){
			sig_otp_arr[i + j] = sig_otp_arr[i + j] + (sig_src_arr[i] * imp_rsp_arr[j]);
			g_imp_rsp_sample = sig_otp_arr[i + j];
		}

	}
}

static void plot_impulse_response(void){
	for(int i = 0; i < IMP_RSP_LEN; i++){
		g_imp_rsp_sample = impulse_response[i];
		pseudo_dly(22500);
	}
}

static void plot_input_signal(void){
	for(iplot = 0;iplot < KHZ1_15_SIG_LEN;iplot++){
		g_imp_rsp_sample = input_signal_f32_1kHz_15kHz[iplot];
		pseudo_dly(1000);
	}
}

static void pseudo_dly(int dly){
	for(int i = 0;i < dly;i++){

	}
}

static void fpu_enable(void){
	SCB->CPACR |=  ((1UL << 20)|
					(1UL << 21)|
					(1UL << 22)|
					(1UL << 23));
}
