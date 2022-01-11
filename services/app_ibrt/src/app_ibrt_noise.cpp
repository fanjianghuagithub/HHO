#if defined(IBRT)
#include "app_ibrt_if.h"
#include "app_tws_ctrl_thread.h"
#include "app_ibrt_noise.h"

static bool need_roleswitch_with_noise_bak = false;
extern osTimerId ibrt_ui_check_noise_roleswitch_timer_id;

// switch the app_ibrt_ui_tws based on noise_power and snr
void app_ibrt_ui_noise_reset(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    p_ibrt_ctrl->local_noise_power = 0.f;
    p_ibrt_ctrl->local_snr = 0.f;
    p_ibrt_ctrl->peer_noise_power = 0.f;
    p_ibrt_ctrl->peer_snr = 0.f;
}

void app_ibrt_ui_noise_process(float noise_power, float snr)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if(app_tws_ibrt_tws_link_connected()){
        p_ibrt_ctrl->local_noise_power = noise_power;
        p_ibrt_ctrl->local_snr = snr;
    }
}

bool app_ibrt_ui_tws_switch_according_noise_needed(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    app_ibrt_ui_t *p_ibrt_ui = app_ibrt_ui_get_ctx();

    if ((p_ibrt_ctrl->local_noise_power - p_ibrt_ctrl->peer_noise_power >= p_ibrt_ui->config.noise_power_threshold) \
        && (p_ibrt_ctrl->peer_snr - p_ibrt_ctrl->local_snr >= p_ibrt_ui->config.snr_threshold))
    {
        p_ibrt_ctrl->strong_noise_count += 1;
		if (p_ibrt_ctrl->strong_noise_count >= p_ibrt_ui->config.noise_count_threshold)
		{
            //local noise_power is stronger and snr is weaker than peer, local role is SLAVE
            return true;
		}
        else
        {
            return false;
        }
    }
    else
    {
        p_ibrt_ctrl->strong_noise_count = 0;
        return false;
    }
}

void app_ibrt_ui_check_noise_roleswitch_timer_cb(void const *current_evt)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    app_ibrt_ui_t *p_ibrt_ui = app_ibrt_ui_get_ctx();

    if(!(p_ibrt_ui->config.tws_switch_according_to_noise_value)){
        return ;
    }

    if (app_tws_ibrt_mobile_link_connected() &&
        app_ibrt_ui_is_profile_exchanged() &&
        (p_ibrt_ctrl->tws_mode  == BTIF_BLM_ACTIVE_MODE))
    {
        tws_ctrl_send_cmd(APP_TWS_CMD_GET_PEER_NOISE_INFO, NULL, 0);
    }

    osTimerStart(ibrt_ui_check_noise_roleswitch_timer_id, p_ibrt_ui->config.noise_monitor_timeout);
}

void app_ibrt_get_peer_noise_info(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_GET_PEER_NOISE_INFO, p_buff, length);
}

void app_ibrt_get_peer_noise_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (app_tws_ibrt_slave_ibrt_link_connected() &&
        app_ibrt_ui_is_profile_exchanged() &&
        (p_ibrt_ctrl->tws_mode  == BTIF_BLM_ACTIVE_MODE))
    {
        app_ui_noise_info_t  noise_info_buffer;
        noise_info_buffer.local_snr = p_ibrt_ctrl->local_snr;
        noise_info_buffer.local_noise_power = p_ibrt_ctrl->local_noise_power;

        tws_ctrl_send_rsp(APP_TWS_CMD_GET_PEER_NOISE_INFO, \
                          rsp_seq, \
                          (uint8_t *)&noise_info_buffer, \
                          sizeof(app_ui_noise_info_t));
    }
}

void app_ibrt_get_peer_noise_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if (app_tws_ibrt_mobile_link_connected() &&
        app_ibrt_ui_is_profile_exchanged() &&
        (p_ibrt_ctrl->tws_mode  == BTIF_BLM_ACTIVE_MODE))
    {
	    if(p_ibrt_ctrl->role_switch_debonce_time == 0)
	    {
		    app_ibrt_ui_t *p_ibrt_ui = app_ibrt_ui_get_ctx();
    		app_ui_noise_info_t noise_info = *(app_ui_noise_info_t *)p_buff;

		    p_ibrt_ctrl->peer_snr = noise_info.local_snr;
		    p_ibrt_ctrl->peer_noise_power = noise_info.local_noise_power;

	        if(p_ibrt_ui->config.tws_switch_according_to_noise_value)
	        {
	            bool need_roleswitch_with_noise = app_ibrt_ui_tws_switch_according_noise_needed();

	            if(need_roleswitch_with_noise != need_roleswitch_with_noise_bak){
	                if(need_roleswitch_with_noise){
	                    app_ibrt_ui_tws_switch();
	                    p_ibrt_ctrl->role_switch_debonce_time = p_ibrt_ui->config.role_switch_timer_threshold;
	                }
	                need_roleswitch_with_noise_bak = need_roleswitch_with_noise;
	            }
	        }
	    }
	    else
	    {
	        p_ibrt_ctrl->role_switch_debonce_time--;
	    }
	}
}
#endif