#include "stm32f4xx.h"
#include "signals.h"

extern float _640_points_ecg_[HZ_5_SIG_LEN];
float g_in_sig_sample;
int iplot;

static void plot_input_signal(void);
static void pseudo_dly(int dly);

int main(){
	/*System Control Block points to Coprocessor Access Control Register,
	  habiliting the Floating Point Unit of CP10 and CP11 */
	SCB->CPACR |=  ((1UL << 20)|
					(1UL << 21)|
					(1UL << 22)|
					(1UL << 23));

	while(1){
		plot_input_signal();
	}
}

static void plot_input_signal(void){
	for(iplot = 0;iplot < HZ_5_SIG_LEN;iplot++){
		g_in_sig_sample = _640_points_ecg_[iplot];
		pseudo_dly(1e4);
	}
}

static void pseudo_dly(int dly){
	for(int i = 0;i < dly;i++){

	}
}
