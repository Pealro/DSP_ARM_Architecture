#include "stm32f4xx.h"
#include "signals.h"
#include "uart.h"
#include "stdio.h"
#include "arm_math.h"
#include "systick.h"

extern float32_t _640_points_ecg_[ECG_SIG_LEN];
extern float32_t input_signal_f32_1kHz_15kHz[KHZ1_15_SIG_LEN];
extern float32_t  impulse_response[IMP_RSP_LEN];
extern float32_t _5hz_signal[SIG_5_HZ_LEN];
float32_t output_signal[KHZ1_15_SIG_LEN + IMP_RSP_LEN - 1];

float g_in_sig_sample;
float g_imp_rsp_sample;
int iplot;

float32_t signal_2;
float32_t impulse;
float32_t output;
float32_t signal_re;
float32_t signal_img;

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
static void calc_sig_dft(float32_t *sig_src_arr,
		 	 	  float32_t *sig_otp_rex_arr,
				  float32_t *sig_otp_imx_arr,
				  uint32_t	src_lgt
				  );
static void calc_sig_idft(float32_t *sig_src_rex_arr,
	  		 	 	 	  float32_t *sig_src_imx_arr,
						  float32_t *sig_otp_arr,
						  uint32_t	idft_lgt);

uint32_t g_before, g_after, g_time_taken;
float32_t g_seconds, g_miliseconds;
float32_t REX[KHZ1_15_SIG_LEN/2];
float32_t IMX[KHZ1_15_SIG_LEN/2];
float32_t IDFT[ECG_SIG_LEN];

int main(){
	/*System Control Block points to Coprocessor Access Control Register,
	  habiliting the Floating Point Unit of CP10 and CP11 */
	fpu_enable();

	//Initialize the UART
	uart2_tx_init();

	//Initialize Systick
	systick_counter_init();

	calc_sig_dft((float32_t *) _640_points_ecg_,
			 	 (float32_t *) REX,
				 (float32_t *) IMX,
				 (uint32_t)	ECG_SIG_LEN
				 );

	g_before = SysTick->VAL;

	calc_sig_idft((float32_t *)REX,
		  		  (float32_t *)IMX,
				  (float32_t *)IDFT,
				  (uint32_t)   ECG_SIG_LEN);

	g_after = SysTick->VAL;
	g_time_taken = g_before - g_after;

	while(1){
		//plot_input_signal();
		plot_all_signals();
	}
}

static void calc_sig_idft(float32_t *sig_src_rex_arr,
	  		 	 	 	  float32_t *sig_src_imx_arr,
						  float32_t *sig_otp_arr,
						  uint32_t	idft_lgt){
	uint32_t i,k;

	//Normalize amplitudes
	for(i = 0; i < (idft_lgt / 2); i++){
		sig_src_rex_arr[i] = sig_src_rex_arr[i] / (idft_lgt / 2);
		sig_src_imx_arr[i] = - sig_src_imx_arr[i] / (idft_lgt / 2);
	}

	//Deal with idx 0
	sig_src_rex_arr[0] = sig_src_rex_arr[0] / (idft_lgt);
	sig_src_imx_arr[0] = - sig_src_imx_arr[0] / (idft_lgt);

	//Deal with idx n/2
	sig_src_rex_arr[(idft_lgt / 2)] =   sig_src_rex_arr[(idft_lgt / 2)] / (idft_lgt);
	sig_src_imx_arr[(idft_lgt / 2)] = - sig_src_imx_arr[(idft_lgt / 2)] / (idft_lgt);

	/*Clear output signal buffer*/
	for(i = 0; i < idft_lgt; i++){
		sig_otp_arr[i] = 0;
	}

	//Perform IDFT
	for(k = 0; k < (idft_lgt / 2); k++){
		for(i = 0; i < idft_lgt; i++){
			sig_otp_arr[i] += sig_src_rex_arr[k] * cos(2*PI*i*k/idft_lgt) +
							  sig_src_imx_arr[k] * sin(2*PI*i*k/idft_lgt);
		}
	}
}

static void calc_sig_dft(float32_t *sig_src_arr,
		 	 	  		 float32_t *sig_otp_rex_arr,
						 float32_t *sig_otp_imx_arr,
						 uint32_t	src_lgt
				  	 	 ){
	uint32_t i, j, k;

	/*Clear output signal buffer*/
	for(i = 0; i < (src_lgt/2); i++){
		sig_otp_rex_arr[i] = 0;
		sig_otp_imx_arr[i] = 0;
	}

	for(j = 0; j < (src_lgt/2); j++){
		for(k = 0; k < src_lgt; k++){
			sig_otp_rex_arr[j] = sig_otp_rex_arr[j] + sig_src_arr[k] * cos(2*PI*j*k/src_lgt);
			//sig_otp_rex_arr[j] = fabs(sig_otp_rex_arr[j]);
			sig_otp_imx_arr[j] = sig_otp_imx_arr[j] + sig_src_arr[k] * sin(2*PI*j*k/src_lgt);
			//sig_otp_imx_arr[j] = fabs(sig_otp_imx_arr[j]);

		}
	}
}

void plot_all_signals(void){
	uint32_t i, j, k;
	i = j = 0;

	for(k = 0; k < ECG_SIG_LEN; k++){
		/*i++;
		j++;
		if(i == KHZ1_15_SIG_LEN){i = 0;}
		if(j == IMP_RSP_LEN){j = 0;}
		impulse = impulse_response[j];
		signal_2 = input_signal_f32_1kHz_15kHz[i];
		output = output_signal[k];*/
		output = IDFT[ECG_SIG_LEN - k];
		signal_2 = _640_points_ecg_[k];
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
	for(iplot = 0;iplot < ECG_SIG_LEN;iplot++){
		g_imp_rsp_sample = _640_points_ecg_[iplot];
		pseudo_dly(9000);
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
