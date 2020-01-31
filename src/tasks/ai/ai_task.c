#include "tasks/apps/common.h"

void ai_app_init();
void ai_app_process();

void ai_task(void){
    io_mux_init();
    io_set_power();

    ai_app_init();

    while(1){
      ai_app_process();
    }

}
