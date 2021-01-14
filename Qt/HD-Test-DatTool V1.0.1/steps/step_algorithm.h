#ifndef STEP_ALGORITHM_H
#define STEP_ALGORITHM_H

#include <QMainWindow>

#define ZAccData_DiffValue 5
#define ZAccData_MaxThreshold 250
#define ZAccData_MinThreshold -250
#define ZAccData_DiffThreshold 500
#define Step_ZAccPeakValue_len ZAccData_len/5
#define ZAcc_FilterThreadValue_Max 30000
#define ZAcc_FilterThreadValue_Min -30000
#define ZAccData_len 250

#pragma pack(1)
typedef struct
{
    uint8_t Step_ZAccAdd_Flag;
    int Step_ZAccAdd_Max;
    int Step_ZAccReduce_Min;
    int Step_SubCount;
}Steps_GlobalVari_Stru;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t Step_ZAccPeakValue_Flag;   /* 1表示最大值，2表示最小值 */
    int Step_ZAccPeakValue_Positoin;
    short int Step_ZAccPeakValue;
}Step_ZAccPeakValue_Stru;
#pragma pack()

extern short int ZAccData_buf[ZAccData_len];
extern Steps_GlobalVari_Stru Steps_GlobalVari_Stru_temp;
extern uint8_t ZAccData_Flag;

class Step_algorithm : public QMainWindow
{
    Q_OBJECT
public:
    explicit Step_algorithm(QWidget *parent = nullptr);
    void ZAccData_FilterHandle(void);

signals:

public slots:
};

#endif // STEP_ALGORITHM_H
