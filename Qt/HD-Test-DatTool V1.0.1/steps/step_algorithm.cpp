#include "step_algorithm.h"
#include <QDebug>

Step_algorithm::Step_algorithm(QWidget *parent) : QMainWindow(parent)
{

}

static int Step_ZAccPeakValue_cnt = 0;
Steps_GlobalVari_Stru Steps_GlobalVari_Stru_temp;
short int ZAccData_buf[ZAccData_len];
uint8_t ZAccData_Flag = 0;
void Step_algorithm::ZAccData_FilterHandle()
{
    Step_ZAccPeakValue_Stru Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_len];
    //float Normal_ZAccData = 1000;
    short int ZAccData_temp[ZAccData_len];
    short int AData_temp = 0, BData_temp, CData_temp = 0, DData_temp = 0;
    short int EData_temp = 0, FData_temp, GData_temp = 0, HData_temp = 0;
    short int ZAccData_Diff_temp = 0;
    int ATimeDelay = 0, BTimeDelay = 0;
    int Step_ZAccPeakValue_FlagSub = 0;
    Steps_GlobalVari_Stru_temp.Step_ZAccAdd_Max = 0;
    //Steps_GlobalVari_Stru_temp.Step_SubCount = 0;
    memset(reinterpret_cast<uint8_t *>(Step_ZAccPeakValue_Stru_temp), 0, sizeof(Step_ZAccPeakValue_Stru)*Step_ZAccPeakValue_len);
    if(ZAccData_Flag == 1)
    {
        memset(reinterpret_cast<short int *>(ZAccData_temp), 0, sizeof(short int)*ZAccData_len);
        memcpy(reinterpret_cast<short int *>(ZAccData_temp), reinterpret_cast<short int *>(ZAccData_buf), sizeof(short int)*ZAccData_len);
        memset(reinterpret_cast<short int *>(ZAccData_buf), 0, sizeof(short int)*ZAccData_len);
        /* 起脚状态判定 */
        /* 轮询5个数据以获得加速度上升峰值 */
        for(int i = 0; i < ZAccData_len; i++)
        {
            AData_temp = ZAccData_temp[i - 1] - ZAccData_temp[i - 2];
            BData_temp = ZAccData_temp[i] - ZAccData_temp[i - 1];
            CData_temp = ZAccData_temp[i] - ZAccData_temp[i + 1];
            DData_temp = ZAccData_temp[i + 1] - ZAccData_temp[i + 2];

            EData_temp = ZAccData_temp[i - 2] - ZAccData_temp[i - 1];
            FData_temp = ZAccData_temp[i - 1] - ZAccData_temp[i];
            GData_temp = ZAccData_temp[i + 1] - ZAccData_temp[i];
            HData_temp = ZAccData_temp[i + 2] - ZAccData_temp[i + 1];
            if((AData_temp > ZAccData_DiffValue) && (BData_temp > ZAccData_DiffValue)
                 && (CData_temp > ZAccData_DiffValue) && (DData_temp > ZAccData_DiffValue)
                 && (ZAccData_temp[i - 1] > 0) && (ZAccData_temp[i] > ZAccData_MaxThreshold)
                 && (ZAccData_temp[i + 1] > 0))
            {
                #if 1
                Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue_Flag = 1;
                Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue_Positoin = i;
                Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue = ZAccData_temp[i];
                Step_ZAccPeakValue_cnt ++;
                //qDebug() << "step";
                AData_temp = 0;
                BData_temp = 0;
                CData_temp = 0;
                DData_temp = 0;
                //qDebug() << "step" << Step_ZAccPeakValue_cnt << Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue_Positoin << Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue;
                //qDebug("step[%d] = %d, %d\r\n", Step_ZAccPeakValue_cnt,Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue_Positoin, Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue);
                #endif
            }

            else if(((EData_temp) > ZAccData_DiffValue) && ((FData_temp) > ZAccData_DiffValue)
                 && ((GData_temp) > ZAccData_DiffValue) && ((HData_temp) > ZAccData_DiffValue)
                 && (ZAccData_temp[i - 1] < ZAccData_MinThreshold) && (ZAccData_temp[i] < ZAccData_MinThreshold)
                 && (ZAccData_temp[i + 1] < ZAccData_MinThreshold))
            {
                #if 1
                Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue_Flag = 2;
                Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue_Positoin = i;
                Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt].Step_ZAccPeakValue = ZAccData_temp[i];
                Step_ZAccPeakValue_cnt ++;
                #endif
            }
            else;
        }
        memset((short int *)ZAccData_temp, 0, sizeof(short int)*ZAccData_len);

        /* 数据有效性判断 */
        for(int i = 0; i < Step_ZAccPeakValue_cnt - 2; i++)
        {
            /* 检测到加速度增加状态 */
            if(Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue_Flag == 1)
            {
                /* 判定下一状态是否加速度减少状态 */
                if(Step_ZAccPeakValue_Stru_temp[i +1].Step_ZAccPeakValue_Flag == 2)
                {
                    /* 峰值时间窗判定(100ms-1s)之间，峰值之间的差是否大于预设值 */
                    ATimeDelay = Step_ZAccPeakValue_Stru_temp[i + 1].Step_ZAccPeakValue_Positoin - Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue_Positoin;

                    ZAccData_Diff_temp = Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue - Step_ZAccPeakValue_Stru_temp[i + 1].Step_ZAccPeakValue;
                    if((ATimeDelay > 5)&&(ATimeDelay < 50) && (ZAccData_Diff_temp > ZAccData_DiffThreshold))
                    {
                        Steps_GlobalVari_Stru_temp.Step_SubCount = Steps_GlobalVari_Stru_temp.Step_SubCount + 2;
                    }
                }
                else;

            }
            else
            {
                Step_ZAccPeakValue_FlagSub = Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue_Flag + Step_ZAccPeakValue_Stru_temp[i +1].Step_ZAccPeakValue_Flag;
                BTimeDelay = Step_ZAccPeakValue_Stru_temp[i + 1].Step_ZAccPeakValue_Positoin - Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue_Positoin;
                if(((Step_ZAccPeakValue_FlagSub == 2) ||(Step_ZAccPeakValue_FlagSub == 4)) && (BTimeDelay > 5)&&(BTimeDelay < 50))
                {
                    Steps_GlobalVari_Stru_temp.Step_SubCount = Steps_GlobalVari_Stru_temp.Step_SubCount + 1;
                }
                //continue;
            }
            qDebug("step[%d] = %d, %d\r\n", i,Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue_Positoin, Step_ZAccPeakValue_Stru_temp[i].Step_ZAccPeakValue);

        }

        qDebug("P_S = %d, %d\r\n", Step_ZAccPeakValue_cnt, Steps_GlobalVari_Stru_temp.Step_SubCount);
        /* 如果最后一包为加速度增加状态，则步数加1 */
        if(Step_ZAccPeakValue_Stru_temp[Step_ZAccPeakValue_cnt -1].Step_ZAccPeakValue_Flag == 1)
        {
            Steps_GlobalVari_Stru_temp.Step_SubCount ++;
        }
        ZAccData_Flag = 0;
        Step_ZAccPeakValue_cnt = 0;
   }

}
