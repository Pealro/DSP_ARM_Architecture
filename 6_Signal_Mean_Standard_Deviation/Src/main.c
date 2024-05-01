#include "stm32f4xx.h"
#include "signals.h"
#include "uart.h"
#include "stdio.h"
#include "arm_math.h"

extern float _640_points_ecg_[HZ_5_SIG_LEN];
extern float32_t input_signal_f32_1kHz_15kHz[KHZ1_15_SIG_LEN];
float g_in_sig_sample;
int iplot;

static void plot_input_signal(void);
static void pseudo_dly(int dly);
static void fpu_enable(void);
static float32_t signal_mean(float32_t *sig_src_arr, uint32_t sig_lenght);
static float32_t signal_variance(float32_t *sig_src_arr, float32_t signal_mean, uint32_t sig_lenght);
static float32_t signal_std_deviation(float32_t variance);

float32_t g_mean_value;
float32_t g_variance_value;
float32_t g_std_deviation_value;
float32_t g_std_deviation_value_cmsis;

int main(){
	/*System Control Block points to Coprocessor Access Control Register,
	  habiliting the Floating Point Unit of CP10 and CP11 */
	fpu_enable();

	//Initialize the UART
	uart2_tx_init();

	g_mean_value = signal_mean((float32_t *)input_signal_f32_1kHz_15kHz,(uint32_t) KHZ1_15_SIG_LEN);
	g_variance_value = signal_variance((float32_t *)input_signal_f32_1kHz_15kHz,(float32_t) g_mean_value,(uint32_t) KHZ1_15_SIG_LEN);
	g_std_deviation_value = signal_std_deviation((float32_t) g_variance_value);
	arm_std_f32(&input_signal_f32_1kHz_15kHz[0], KHZ1_15_SIG_LEN, &g_std_deviation_value_cmsis);

	while(1){
		//plot_input_signal();
	}
}

static float32_t signal_variance(float32_t *sig_src_arr, float32_t signal_mean, uint32_t sig_lenght){
	float32_t variance = 0.0;
	uint32_t i;

	for(i = 0;i < sig_lenght; i++){
		variance = variance + powf((sig_src_arr[i] - signal_mean), 2);
	}

	variance = variance / (float32_t)(sig_lenght - 1);

	return variance;
}

static float32_t signal_std_deviation(float32_t variance){
	float32_t std_deviation;

	std_deviation = sqrt(variance);

	return std_deviation;
}

static float32_t signal_mean(float32_t *sig_src_arr, uint32_t sig_lenght){
	float32_t mean = 0.0;
	uint32_t i;

	for(i = 0;i < sig_lenght; i++){
		mean = mean + sig_src_arr[i];
	}

	mean = mean /(float32_t)sig_lenght;

	return mean;
}

static void plot_input_signal(void){
	for(iplot = 0;iplot < KHZ1_15_SIG_LEN;iplot++){
		g_in_sig_sample = input_signal_f32_1kHz_15kHz[iplot];
		pseudo_dly(1e4);
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
