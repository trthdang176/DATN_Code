/* header file */
#include "app_eeprom.h"

/* app header */
#include "app_main.h"
#include "../Core/Inc/main.h"
#include "app_eeprom_data.h"


/* Define */
#define MAX_WRITE_EEPROM_BUF 10

/* the app task */
typedef struct {
    OS_task task;

    OS_TimeEvt time_wait; /* time wait write is complete */
    OS_TimeEvt time_check; /* time check write data in buf */
    data_eeprom_t data_eeprom;

    write_param_t **write_params_buf; /* buff store data write */
    uint8_t head_index;
    uint8_t tail_index;
    uint8_t nUsed;
    uint8_t end;
    bool b_is_writing; /* run or not run */
} app_eeprom;


void eeprom_ctor(app_eeprom *pAO);
static void eeprom_init(app_eeprom * const pOS_task, OS_event_t const * const pEvent);
static void eeprom_dispatch(app_eeprom * const pOS_task, OS_event_t const * const pEvent);
static void write_buffer_with_task(write_param_t *pOS_task);

void write_data_test_ic(const char *icName, const char *Data, uint16_t len);
 
static app_eeprom eeprom_instance;
OS_task * const AO_task_eeprom = &eeprom_instance.task;

AT24Cxx_t eeprom_ob;
static write_param_t *write_buf[MAX_WRITE_EEPROM_BUF]; /* buffer store the data write */

/* I2C port using for eeprom */
void eeprom_task_init(I2C_HandleTypeDef* i2c_port, uint8_t dev_address) {
//    eeprom_ob.i2c_port = i2c_port;
//    eeprom_ob.dev_address = dev_address;
	
    /* init eeprom */
    if (AT24Cxx_Init(&eeprom_ob,dev_address,i2c_port) == AT24Cxx_Init_OK) {
        /* init success */
        printf("Init eeprom success!!\n");
    } else {
        printf("Init eeprom fail!!\n");
    }

      uint8_t data_check = 0;
    // AT24Cxx_read_buffer(&eeprom_ob,MEM_ADDR_CHECK_UPDATA_DATA_TEST,&data_check,1);
    // if (data_check == 0) {
    // } 
    AT24Cxx_write_buffer_bloking(&eeprom_ob,START_MEM_ADDR_DIREC_USED,(uint8_t *)&data_check,sizeof(data_check));
    HAL_Delay(10);

    write_data_test_ic("744051",data_744051,sizeof(data_744051));
    write_data_test_ic("740000",data_7400,sizeof(data_7400));
    write_data_test_ic("74139",data_74139,sizeof(data_74139));
    write_data_test_ic("74164",data_74164,sizeof(data_74164));
    write_data_test_ic("7408",data_7408,sizeof(data_7408));
    write_data_test_ic("74157",data_74157,sizeof(data_74157));
    write_data_test_ic("40175",data_40175,sizeof(data_40175));
    write_data_test_ic("40174",data_40174,sizeof(data_40174));
    write_data_test_ic("4027",data_4027,sizeof(data_4027));
    write_data_test_ic("74165",data_74165,sizeof(data_74165));


    uint8_t num_direc;
    AT24Cxx_read_buffer(&eeprom_ob,START_MEM_ADDR_DIREC_USED,&num_direc,1);
    // uint8_t clear_buf[TOTAL_ALL_PROGRAM_TEST_LEN] = {0};
    // AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(0),(uint8_t *)clear_buf,TOTAL_ALL_PROGRAM_TEST_LEN);
    // AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(0),(uint8_t *)data_program1,strlen(data_program1));
    // AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(1),(uint8_t *)data_program2,strlen(data_program2));
    // AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(2),(uint8_t *)data_program3,strlen(data_program3));
    // AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(3),(uint8_t *)data_program4,strlen(data_program4));
    uint8_t clear_buf[60] = {0};
    AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_WIFI_INFO,clear_buf,DATA_LEN_WIFI_INFO);
    AT24Cxx_write_buffer_bloking(&eeprom_ob,START_ADDR_WIFI_INFO,wifi_info,strlen(wifi_info));
    uint8_t buffer[30];
//    AT24Cxx_read_buffer(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(0),&buffer,strlen(data_program1));
//    AT24Cxx_read_buffer(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(1),&buffer,strlen(data_program2));
//    AT24Cxx_read_buffer(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(2),&buffer,strlen(data_program3));
//    AT24Cxx_read_buffer(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(3),&buffer,strlen(data_program4));

    char text_program_1[] = "0";
    AT24Cxx_write_buffer(&eeprom_ob,0x02,(uint8_t *)text_program_1,strlen(text_program_1));

//        char *text_program = "Program test2,74hc560,20";
    /* check write data test */


    // AT24Cxx_write_buffer(&eeprom_ob,MEM_ADDR_CHECK_UPDATA_DATA_TEST,&data_check,1);

    eeprom_instance.b_is_writing = false;
    eeprom_instance.data_eeprom.mem_addr = 0x00;
    eeprom_instance.data_eeprom.data = (uint8_t *)0;
    eeprom_instance.data_eeprom.data_len = 0;

    eeprom_instance.write_params_buf = write_buf;
    eeprom_instance.head_index = 0;
    eeprom_instance.tail_index = 0;
    eeprom_instance.end = MAX_WRITE_EEPROM_BUF - 1;
    eeprom_instance.nUsed = 0;

    eeprom_ctor(&eeprom_instance);
//    printf("Setting info eeprom success\n");
}

void eeprom_ctor(app_eeprom *pAO) {
    /* init task */
    OS_task_init(AO_task_eeprom,(OS_handler)&eeprom_init,(OS_handler)&eeprom_dispatch);

    /* init time */
    OS_TimeEvt_init(&eeprom_instance.time_wait,FINISH_WRITE,&eeprom_instance.task);
    OS_TimeEvt_init(&eeprom_instance.time_check,CHECK_WRITE_BUF,&eeprom_instance.task);
}

static void eeprom_init(app_eeprom * const pOS_task, OS_event_t const * const pEvent) {
    /* init time */
    OS_TimeEvt_Stop(&eeprom_instance.time_check); /* don't using time when init */
    OS_TimeEvt_Stop(&eeprom_instance.time_wait); 

    /* read data program run */
    eeprom_instance.data_eeprom.data = malloc(TOTAL_ALL_PROGRAM_TEST_LEN);
    eeprom_instance.data_eeprom.data_len = TOTAL_ALL_PROGRAM_TEST_LEN;
    AT24Cxx_read_buffer(&eeprom_ob,START_ADDR_PROGRAM_TEST_X(0),
                        eeprom_instance.data_eeprom.data,TOTAL_ALL_PROGRAM_TEST_LEN);

    data_eeprom_t *data_eeprom_read = (data_eeprom_t *)malloc(sizeof(data_eeprom_t));
    data_eeprom_read->data_len = eeprom_instance.data_eeprom.data_len;
    data_eeprom_read->data = (uint8_t *)malloc(eeprom_instance.data_eeprom.data_len);
    memcpy(data_eeprom_read->data,eeprom_instance.data_eeprom.data,eeprom_instance.data_eeprom.data_len);
    


    // OS_task_post_event(AO_BlinkyTest,READ_ALL_PROGRAM_TEST,(uint8_t *)&data_eeprom_read,sizeof(data_eeprom_t *));

    free(eeprom_instance.data_eeprom.data);

}

void eeprom_dispatch(app_eeprom * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case READ_EEPROM : {
            data_eeprom_t *data_read = (data_eeprom_t *)(*(uint32_t *)get_data_dynamic_event(pEvent));
            eeprom_instance.data_eeprom.data = malloc(data_read->data_len);
            AT24Cxx_read_buffer(&eeprom_ob,data_read->mem_addr,
                                eeprom_instance.data_eeprom.data,eeprom_instance.data_eeprom.data_len);
            // printf("Data read eeprom : %s\n",eeprom_instance.data_eeprom.data);
            free(eeprom_instance.data_eeprom.data);
            free(data_read);
            
        } break;
        case WRITE_EEPROM : {
            printf("Write eeprom\n");
            data_eeprom_t *data_receive = (data_eeprom_t *)(*(uint32_t *)get_data_dynamic_event(pEvent));
            write_param_t *e_write = (write_param_t *)malloc(sizeof(write_param_t));
            // e_write->byte_first_page = AT24C256_PG_SIZE - data_receive->mem_addr % AT24C256_PG_SIZE;
            // e_write->page_write = (data_receive->data_len - e_write->byte_first_page) / AT24C256_PG_SIZE;
            // e_write->byte_remain = (data_receive->data_len - e_write->byte_first_page) % AT24C256_PG_SIZE;
            e_write->data_written = 0;
            e_write->data_lastlen = 0;
            e_write->mem_addr = data_receive->mem_addr;
            e_write->data_len = data_receive->data_len;
            e_write->data = (uint8_t *)malloc(data_receive->data_len);
            memcpy(e_write->data,data_receive->data,data_receive->data_len);

            pOS_task->write_params_buf[pOS_task->head_index] = e_write; /* insert to buffer */
            if (pOS_task->head_index == 0) {
                pOS_task->head_index = pOS_task->end;
            } else {
                --pOS_task->head_index;
            }
            ++pOS_task->nUsed;
            
            if (data_receive->data != NULL) free(data_receive->data);
            free(data_receive);

            if (pOS_task->b_is_writing == false) { /* if the eeprom is free, writing doesn't need to wait */
                pOS_task->b_is_writing = true;
                write_buffer_with_task(pOS_task->write_params_buf[pOS_task->tail_index]);
            }
        } break;
        case WAIT_WRITE_TIMEOUT : {
            /* wait ~10ms for eeprom write complete */
            pOS_task->write_params_buf[pOS_task->tail_index]->data_written += pOS_task->write_params_buf[pOS_task->tail_index]->data_lastlen;

            OS_TimeEvt_Set(&pOS_task->time_wait,11,0); /* run only 1 time */
        } break;
        case FINISH_WRITE : {
            if (pOS_task->write_params_buf[pOS_task->tail_index]->data_written < pOS_task->write_params_buf[pOS_task->tail_index]->data_len) {
                write_buffer_with_task(pOS_task->write_params_buf[pOS_task->tail_index]);
            } else {
                /* finish write */
                // printf("Write data to eeprom finish\n");
                if (pOS_task->write_params_buf[pOS_task->tail_index]->data != NULL) {
                    free(pOS_task->write_params_buf[pOS_task->tail_index]->data);
                }
                if (pOS_task->write_params_buf[pOS_task->tail_index] != NULL) {
                    free(pOS_task->write_params_buf[pOS_task->tail_index]);
                }

                if (pOS_task->tail_index == 0) {
                    pOS_task->tail_index = pOS_task->end;
                } else {
                    --pOS_task->tail_index;
                } 
                --pOS_task->nUsed;
                pOS_task->b_is_writing = false;
                /* Check buf */
                if (pOS_task->nUsed > 0) {
                    OS_task_post_event(AO_task_eeprom,WRITE_EEPROM_IN_BUF,(uint8_t *)0,0);
                }
            }
        } break; 
        case WRITE_EEPROM_IN_BUF : {
            if (pOS_task->b_is_writing == false) { /* if the eeprom is free, writing doesn't need to wait */
                pOS_task->b_is_writing = true;
                write_buffer_with_task(pOS_task->write_params_buf[pOS_task->tail_index]);
            }
        }
        default : break;
    }
}

static void write_buffer_with_task(write_param_t *pWrite_params) {
    uint16_t byte_remain = pWrite_params->data_len - pWrite_params->data_written;
    uint8_t written_in_page = (pWrite_params->mem_addr + pWrite_params->data_written) % AT24C256_PG_SIZE;
    uint8_t byte_in_page = AT24C256_PG_SIZE - written_in_page;
    uint16_t addr_write = pWrite_params->mem_addr + pWrite_params->data_written;
    uint8_t len_write;
    if (byte_remain < byte_in_page) {
        len_write = byte_remain;
    } else {
        len_write = byte_in_page;
    }
    pWrite_params->data_lastlen = len_write;
    AT24Cxx_write_buffer(&eeprom_ob,addr_write,pWrite_params->data + pWrite_params->data_written,len_write);
}

void write_data_test_ic(const char *icName, const char *Data, uint16_t len) {
    uint8_t num_direc = 0;
    uint16_t dir_addr, data_addr;
    uint16_t mem_pre, len_pre;
    direc_EEPROM_t *Directory_pre;
    AT24Cxx_read_buffer(&eeprom_ob,START_MEM_ADDR_DIREC_USED,&num_direc,1);
    dir_addr = START_MEM_ADDR_DATA_DIREC + (num_direc * sizeof(direc_EEPROM_t));
    if (num_direc == 0) { /* first */ 
        data_addr = START_MEM_ADDR_DATA_TEST ;
    } else {
        uint8_t *temp_buf = (uint8_t *)malloc(sizeof(direc_EEPROM_t));
        AT24Cxx_read_buffer(&eeprom_ob,START_MEM_ADDR_DATA_DIREC + ((num_direc - 1) * sizeof(direc_EEPROM_t)),temp_buf,sizeof(direc_EEPROM_t));
        Directory_pre = (direc_EEPROM_t *)temp_buf;
        mem_pre = (uint16_t)((Directory_pre->addr[0] << 8) | Directory_pre->addr[1]);
        len_pre = (uint16_t)((Directory_pre->length[0] << 8) | Directory_pre->length[1]);
        data_addr = mem_pre + len_pre;
        free(temp_buf);
    }
    /* write data */

    AT24Cxx_write_buffer_bloking(&eeprom_ob,data_addr,Data,len);

    /* get the directory data */
    direc_EEPROM_t direc;
    memset(direc.nameIC,0,sizeof(direc.nameIC));
    memcpy(direc.nameIC,icName,strlen(icName));
    direc.addr[0] = ((data_addr >> 8) & 0xFF);
    direc.addr[1] = ((data_addr) & 0xFF);
    direc.length[0] = ((len >> 8) & 0xFF);
    direc.length[1] = ((len) & 0xFF);
    AT24Cxx_write_buffer_bloking(&eeprom_ob,dir_addr,(uint8_t *)&direc,sizeof(direc_EEPROM_t));
    ++num_direc;
    AT24Cxx_write_buffer_bloking(&eeprom_ob,START_MEM_ADDR_DIREC_USED,&num_direc,1);
}


