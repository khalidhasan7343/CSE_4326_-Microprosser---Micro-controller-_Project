#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/ledc.h"         
#include "esp_adc/adc_oneshot.h" 
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_rom_sys.h"
#include "esp_sntp.h"

/* ================= HARDWARE PIN MAP ================= */
#define SERVO_O2              GPIO_NUM_16 // PWM Channel 0 (MG996R)
#define SERVO_SALINE          GPIO_NUM_5  // PWM Channel 1 (MG996R)
#define SERVO_DOSE            GPIO_NUM_18 // PWM Channel 2 (MG996R)
#define BUZZER_PIN            GPIO_NUM_4
#define HX711_DT              GPIO_NUM_19
#define HX711_SCK             GPIO_NUM_23

// I2C & ECG Sensors
#define I2C_MASTER_SCL_IO     22
#define I2C_MASTER_SDA_IO     21
#define I2C_MASTER_NUM        I2C_NUM_0
#define I2C_MASTER_FREQ_HZ    100000
#define MAX30102_ADDR         0x57
#define ECG_ADC_UNIT          ADC_UNIT_1
#define ECG_ADC_CHANNEL       ADC_CHANNEL_6 
#define LO_PLUS_PIN           GPIO_NUM_25
#define LO_MINUS_PIN          GPIO_NUM_26

/* ================= WI-FI & FIREBASE CONFIG ================= */
#include "local_config.h"
#define MAXIMUM_RETRY 100 

/* ================= GLOBALS & THRESHOLDS ================= */
#define OXYGEN_DANGER_LEVEL   85.0   
#define FINGER_THRESHOLD      20000
#define MIN_BPM               50
#define MAX_BPM               130
#define BPM_BUFFER_SIZE       8

float saline_stop_ml = 0.0;     
float dose_trigger_ml = 0.0;    
int dose_trigger_hour = -1;     
int dose_trigger_minute = -1;   

float current_ml = 0; 
float smooth_spo2 = 0;
int current_ecg = 0; 
float display_bpm = 0;
static int wifi_retry_num = 0;

// Hardware State Globals
long raw_weight = 0;
int current_o2_angle = 0;
int current_saline_angle = 0;
int current_dose_angle = 0;
bool reset_dose_firebase = false;
char saline_status[16] = "Open";
char dose_status[16] = "Standby";

// Biometric Variables
float dc_ir = 0, dc_red = 0, ac_ir = 0, ac_red = 0;
float max_ac_ir = 0, min_ac_ir = 0, max_ac_red = 0, min_ac_red = 0, prev_ac_ir = 0;
bool beat_armed = false;
int64_t last_beat_time = 0;
float bpm_buffer[BPM_BUFFER_SIZE];
int bpm_index = 0;

/* ================= WI-FI EVENT HANDLER ================= */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (wifi_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            wifi_retry_num++;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        wifi_retry_num = 0;
    }
}

void init_wifi() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    
    esp_netif_init(); 
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 
    esp_wifi_init(&cfg);
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    wifi_config_t wifi_config = { .sta = { .ssid = WIFI_SSID, .password = WIFI_PASS } };
    esp_wifi_set_mode(WIFI_MODE_STA); 
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config); 
    esp_wifi_start();
}

void init_time() {
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
    setenv("TZ", "UTC-6", 1); 
    tzset();
}

/* ================= HARDWARE HELPERS ================= */
void init_hardware() {
    ledc_timer_config_t servo_timer = { .speed_mode = LEDC_LOW_SPEED_MODE, .timer_num = LEDC_TIMER_0, .duty_resolution = LEDC_TIMER_13_BIT, .freq_hz = 50, .clk_cfg = LEDC_AUTO_CLK };
    ledc_timer_config(&servo_timer); 

    ledc_channel_config_t ch0 = { .channel=LEDC_CHANNEL_0, .timer_sel=LEDC_TIMER_0, .gpio_num=SERVO_O2, .speed_mode=LEDC_LOW_SPEED_MODE };
    ledc_channel_config_t ch1 = { .channel=LEDC_CHANNEL_1, .timer_sel=LEDC_TIMER_0, .gpio_num=SERVO_SALINE, .speed_mode=LEDC_LOW_SPEED_MODE };
    ledc_channel_config_t ch2 = { .channel=LEDC_CHANNEL_2, .timer_sel=LEDC_TIMER_0, .gpio_num=SERVO_DOSE, .speed_mode=LEDC_LOW_SPEED_MODE };
    ledc_channel_config(&ch0); ledc_channel_config(&ch1); ledc_channel_config(&ch2); 

    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(HX711_SCK, GPIO_MODE_OUTPUT); 
    gpio_set_direction(HX711_DT, GPIO_MODE_INPUT);
    gpio_set_level(HX711_SCK, 0);

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, .sda_io_num = I2C_MASTER_SDA_IO, .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE, .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void set_servo_angle(ledc_channel_t channel, int angle) {
    uint32_t duty = (500 + (((2500 - 500) * angle) / 180)) * 8192 / 20000;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

void max30102_write(uint8_t reg, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX30102_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
}

void max30102_read(uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX30102_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX30102_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
}

int compare_float(const void *a, const void *b) {
    float fa = *(float*)a; float fb = *(float*)b;
    return (fa > fb) - (fa < fb);
}

long read_scale() {
    long count = 0; int timeout = 10000;
    while(gpio_get_level(HX711_DT) == 1 && timeout > 0) { esp_rom_delay_us(1); timeout--; }
    if (timeout == 0) return 0; 
    for(int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK, 1); esp_rom_delay_us(1);
        count = count << 1;
        gpio_set_level(HX711_SCK, 0); esp_rom_delay_us(1);
        if(gpio_get_level(HX711_DT)) count++;
    }
    gpio_set_level(HX711_SCK, 1); count = count ^ 0x800000; esp_rom_delay_us(1); gpio_set_level(HX711_SCK, 0);
    return count;
}

/* ================= FREERTOS TASKS ================= */

char firebase_rx_buffer[512];
int firebase_rx_len = 0;

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        if (firebase_rx_len + evt->data_len < sizeof(firebase_rx_buffer) - 1) {
            memcpy(firebase_rx_buffer + firebase_rx_len, evt->data, evt->data_len);
            firebase_rx_len += evt->data_len;
            firebase_rx_buffer[firebase_rx_len] = '\0';
        }
    }
    return ESP_OK;
}

void firebase_sync_task(void *pvParameters) {
    char tx_buffer[512];
    
    while (1) {
        // --- 1. PULL SETTINGS FROM FIREBASE ---
        firebase_rx_len = 0; 
        memset(firebase_rx_buffer, 0, sizeof(firebase_rx_buffer));

        esp_http_client_config_t config_get = { 
            .url = FIREBASE_SETTINGS_URL, 
            .method = HTTP_METHOD_GET, 
            .event_handler = http_event_handler,
            .cert_pem = NULL, 
            .skip_cert_common_name_check = true 
        };
        esp_http_client_handle_t client_get = esp_http_client_init(&config_get);
        
        esp_err_t get_err = esp_http_client_perform(client_get);
        if (get_err == ESP_OK) {
            char *ptr;
            ptr = strstr(firebase_rx_buffer, "saline_stop_ml");
            if (ptr && (ptr = strchr(ptr, ':'))) { 
                ptr++; while(*ptr == ' ' || *ptr == '"' || *ptr == '\'') ptr++; 
                saline_stop_ml = atof(ptr); 
            }
            
            ptr = strstr(firebase_rx_buffer, "dose_trigger_ml");
            if (ptr && (ptr = strchr(ptr, ':'))) { 
                ptr++; while(*ptr == ' ' || *ptr == '"' || *ptr == '\'') ptr++; 
                dose_trigger_ml = atof(ptr); 
            }
            
            ptr = strstr(firebase_rx_buffer, "dose_time");
            if (ptr && (ptr = strchr(ptr, ':'))) { 
                ptr++; while(*ptr == ' ' || *ptr == '"' || *ptr == '\'') ptr++; 
                int hr, mn; 
                if (sscanf(ptr, "%d:%d", &hr, &mn) == 2) { dose_trigger_hour = hr; dose_trigger_minute = mn; }
            }
        } 
        esp_http_client_cleanup(client_get);

        // --- 1.5 AUTO-RESET DOSE RULE IN FIREBASE ---
        if (reset_dose_firebase) {
            char reset_buffer[64];
            snprintf(reset_buffer, sizeof(reset_buffer), "{\"dose_trigger_ml\": 0}");
            
            esp_http_client_config_t config_reset = { .url = FIREBASE_SETTINGS_URL, .method = HTTP_METHOD_PATCH, .cert_pem = NULL, .skip_cert_common_name_check = true };
            esp_http_client_handle_t client_reset = esp_http_client_init(&config_reset);
            esp_http_client_set_header(client_reset, "Content-Type", "application/json");
            esp_http_client_set_post_field(client_reset, reset_buffer, strlen(reset_buffer));
            esp_http_client_perform(client_reset);
            esp_http_client_cleanup(client_reset);
            reset_dose_firebase = false; 
        }

        // --- 2. PUSH VITALS & NEW SERVO STATUSES TO FIREBASE ---
        snprintf(tx_buffer, sizeof(tx_buffer), 
                 "{\"bpm\": %.1f, \"spo2\": %.1f, \"ecg\": %d, \"current_ml\": %.1f, \"saline_status\": \"%s\", \"dose_status\": \"%s\"}", 
                 display_bpm, smooth_spo2, current_ecg, current_ml, saline_status, dose_status);
                 
        esp_http_client_config_t config_patch = { .url = FIREBASE_VITALS_URL, .method = HTTP_METHOD_PATCH, .cert_pem = NULL, .skip_cert_common_name_check = true };
        esp_http_client_handle_t client_patch = esp_http_client_init(&config_patch);
        esp_http_client_set_header(client_patch, "Content-Type", "application/json");
        esp_http_client_set_post_field(client_patch, tx_buffer, strlen(tx_buffer));
        
        esp_http_client_perform(client_patch);
        esp_http_client_cleanup(client_patch);

        vTaskDelay(pdMS_TO_TICKS(2000)); 
    }
}

/* ================= MAIN LOOP ================= */
void app_main(void) {
    printf("\n\n--- MEDISMART SYSTEM BOOTING (MEDICAL TASKS ONLY) ---\n");

    init_hardware();
    init_wifi();
    init_time();

    max30102_write(0x09, 0x40);  vTaskDelay(pdMS_TO_TICKS(100));
    max30102_write(0x08, 0x4F);  max30102_write(0x09, 0x03);  
    max30102_write(0x0A, 0x27);  max30102_write(0x0C, 0x24);  max30102_write(0x0D, 0x24);

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = { .unit_id = ECG_ADC_UNIT };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);
    adc_oneshot_chan_cfg_t adc_config = { .bitwidth = ADC_BITWIDTH_DEFAULT, .atten = ADC_ATTEN_DB_12 };
    adc_oneshot_config_channel(adc1_handle, ECG_ADC_CHANNEL, &adc_config);
    gpio_set_direction(LO_PLUS_PIN, GPIO_MODE_INPUT); 
    gpio_set_direction(LO_MINUS_PIN, GPIO_MODE_INPUT);

    xTaskCreate(firebase_sync_task, "firebase", 4096, NULL, 5, NULL);

    int serial_print_counter = 0; 
    static bool dose_cycle_active = false;
    static bool dose_returning = false;

    while (1) {
        // --- 1. READ SENSORS ---
        raw_weight = read_scale();

        if (raw_weight != 0) {
            float new_reading = (raw_weight - 8388608.0) / 1000.0; 
            if (new_reading < 0) new_reading = 0; 
            if (current_ml == 0) {
                current_ml = new_reading; 
            } else {
                current_ml = (0.85 * current_ml) + (0.15 * new_reading); 
            }
        }

        if (gpio_get_level(LO_PLUS_PIN) == 1 || gpio_get_level(LO_MINUS_PIN) == 1) {
            current_ecg = 0; 
        } else {
            adc_oneshot_read(adc1_handle, ECG_ADC_CHANNEL, &current_ecg);
        }

        uint8_t fifo[6];
        max30102_read(0x07, fifo, 6);
        uint32_t red = ((fifo[0] & 0x03) << 16) | (fifo[1] << 8) | fifo[2];
        uint32_t ir  = ((fifo[3] & 0x03) << 16) | (fifo[4] << 8) | fifo[5];

        if (ir < FINGER_THRESHOLD) {
            display_bpm = 0; smooth_spo2 = 0; dc_ir = 0; dc_red = 0; last_beat_time = 0; bpm_index = 0;
            memset(bpm_buffer, 0, sizeof(bpm_buffer));
        } else {
            if (dc_ir == 0) { dc_ir = ir; dc_red = red; }
            dc_ir = 0.95 * dc_ir + 0.05 * ir; dc_red = 0.95 * dc_red + 0.05 * red;
            ac_ir = ir - dc_ir; ac_red = red - dc_red;

            if (ac_ir > max_ac_ir) max_ac_ir = ac_ir; 
            if (ac_ir < min_ac_ir) min_ac_ir = ac_ir;
            
            if (ac_red > max_ac_red) max_ac_red = ac_red; 
            if (ac_red < min_ac_red) min_ac_red = ac_red;

            int64_t now_timer = esp_timer_get_time() / 1000;
            float dynamic_threshold = 0.5 * max_ac_ir;

            if (ac_ir > dynamic_threshold && prev_ac_ir <= dynamic_threshold) beat_armed = true;
            if (beat_armed && ac_ir < prev_ac_ir) {
                if (now_timer - last_beat_time > 400) {   
                    if (last_beat_time != 0) {
                        float bpm = 60000.0 / (now_timer - last_beat_time);
                        if (bpm > MIN_BPM && bpm < MAX_BPM) {
                            bpm_buffer[bpm_index++] = bpm;
                            if (bpm_index >= BPM_BUFFER_SIZE) bpm_index = 0;
                        }
                    }
                    last_beat_time = now_timer;
                }
                beat_armed = false; max_ac_ir = 0; min_ac_ir = 0; max_ac_red = 0; min_ac_red = 0;
            }
            prev_ac_ir = ac_ir;

            float temp[BPM_BUFFER_SIZE]; int count = 0;
            for (int i = 0; i < BPM_BUFFER_SIZE; i++) if (bpm_buffer[i] > 0) temp[count++] = bpm_buffer[i];
            
            if (count >= 5) {
                qsort(temp, count, sizeof(float), compare_float);
                float sum = 0; for (int i = 1; i < count - 1; i++) sum += temp[i];
                display_bpm = (display_bpm == 0) ? (sum / (count - 2)) : (0.97 * display_bpm + 0.03 * (sum / (count - 2)));
            }

            float ir_amp = max_ac_ir - min_ac_ir; float red_amp = max_ac_red - min_ac_red;
            if (ir_amp > 0 && red_amp > 0) {
                float spo2 = 110 - 25 * ((red_amp / dc_red) / (ir_amp / dc_ir));
                if (spo2 > 100) spo2 = 100; 
                if (spo2 < 80) spo2 = 80;
                smooth_spo2 = (smooth_spo2 == 0) ? spo2 : (0.9 * smooth_spo2 + 0.1 * spo2);
            }
        }

        // --- EVALUATE LOGIC CONDITIONS (Strictly Lower Than) ---
        time_t now; struct tm timeinfo;
        time(&now); localtime_r(&now, &timeinfo);

        bool saline_stop_active = (saline_stop_ml > 0 && current_ml < saline_stop_ml);
        bool weight_trigger = (dose_trigger_ml > 0 && current_ml < dose_trigger_ml);
        bool time_trigger = (dose_trigger_hour != -1 && timeinfo.tm_hour == dose_trigger_hour && timeinfo.tm_min == dose_trigger_minute);
        bool o2_emergency = (smooth_spo2 > 0 && smooth_spo2 < OXYGEN_DANGER_LEVEL);

        // --- TASK 1: SALINE VALVE (MG996R) ---
        if (saline_stop_active) {
            if (current_saline_angle < 180) current_saline_angle += 2; 
            strcpy(saline_status, (current_saline_angle >= 180) ? "Stopped" : "Closing");
        } else {
            if (current_saline_angle > 0) current_saline_angle -= 2;   
            strcpy(saline_status, (current_saline_angle <= 0) ? "Open" : "Opening");
        }
        set_servo_angle(LEDC_CHANNEL_1, current_saline_angle);

        // --- TASK 2: AUTO-DOSE SYRINGE (MG996R) ---
        if ((weight_trigger || time_trigger) && !dose_cycle_active) {
            dose_cycle_active = true;
            dose_returning = false;
        }

        if (dose_cycle_active) {
            if (!dose_returning) {
                if (current_dose_angle < 180) {
                    current_dose_angle += 1; 
                    strcpy(dose_status, "Pushing");
                } else {
                    dose_returning = true;
                    reset_dose_firebase = true; 
                    dose_trigger_ml = 0.0;      
                    strcpy(dose_status, "Pushed");
                }
            } else {
                if (current_dose_angle > 0) {
                    current_dose_angle -= 2; 
                } else {
                    dose_cycle_active = false;
                    strcpy(dose_status, "Standby");
                }
            }
        }
        set_servo_angle(LEDC_CHANNEL_2, current_dose_angle);

        // --- TASK 3: OXYGEN EMERGENCY VALVE (MG996R) ---
        if (o2_emergency) {
            gpio_set_level(BUZZER_PIN, 1); 
            if (current_o2_angle < 180) current_o2_angle += 2; 
            set_servo_angle(LEDC_CHANNEL_0, current_o2_angle); 
        } else {
            gpio_set_level(BUZZER_PIN, 0); 
            if (current_o2_angle > 0) current_o2_angle -= 2;   
            set_servo_angle(LEDC_CHANNEL_0, current_o2_angle); 
        }

        // --- LIVE DIAGNOSTIC DASHBOARD ---
        serial_print_counter++;
        if (serial_print_counter >= 50) { 
            printf("\n========== MEDISMART LIVE DASHBOARD ==========\n");
            printf("VITALS  : SpO2: %.1f%% | BPM: %.1f | ECG: %d\n", smooth_spo2, display_bpm, current_ecg);
            printf("SALINE  : %.1f ml | Stop Rule: %.1f ml | Status: %s (%d deg)\n", current_ml, saline_stop_ml, saline_status, current_saline_angle);
            printf("DOSE    : Rule: %.1f ml | Status: %s (%d deg)\n", dose_trigger_ml, dose_status, current_dose_angle);
            printf("==============================================\n");
            serial_print_counter = 0; 
        }

        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
}