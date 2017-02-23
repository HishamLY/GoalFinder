/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include <unistd.h>
#include <string.h>
#include <libgen.h>

//#include "Camera.h"
#include "Point.h"
#include "mjpg_streamer.h"
#include "minIni.h"
//#include "LinuxCamera.h"
//#include "ColorFinder.h"
#include "GoalFinder.h"
#include "Behavior.h"
#include "Serial.h"
#include "Compass.h"
#include "Imu.h"


int main () {


    Mat image;
    minIni* ini = new minIni(INI_FILE_PATH);
    GoalFinder::GetInstance()->LoadINISettings(ini);

    /* ================== Compass Initialize============ */

    // Open serial port
    Ret = serial->Open(U2D_DEV_NAME0,9600);// Open serial link at 57600 bauds 9600
    if (Ret!=1) {      // If an error occured...
        printf ("Error while opening port. Permission problem ?\n");        // ... display a message ...
         Ret = serial->Open(U2D_DEV_NAME1,9600);
	//return Ret;                                                         // ... quit the application
    }
    Compass::GetInstance()->LoadINISettings(ini);
    printf ("Serial port opened successfully!\n");
    int sp(0), kompensator, cmps_now, error, compassError;//variables for compass

/* =================End of Initialize ======================*/

    //CM Initializer
    Imu::GetInstance()->Initialize(&cm730);

    Walking::GetInstance()->LoadINISettings(ini);

    MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());

    LinuxMotionTimer *motion_timer = new LinuxMotionTimer(MotionManager::GetInstance());
    motion_timer->Start();
    /////////////////////////////////////////////////////////////////////

    MotionManager::GetInstance()->LoadINISettings(ini);

    int firm_ver = 0;
    if(cm730.ReadByte(JointData::ID_HEAD_PAN, MX28::P_VERSION, &firm_ver, 0)  != CM730::SUCCESS)
    {
        fprintf(stderr, "Can't read firmware version from Dynamixel ID %d!! \n\n", JointData::ID_HEAD_PAN);
        exit(0);
    }

    if(0 < firm_ver && firm_ver < 27)
    {
#ifdef MX28_1024
        Action::GetInstance()->LoadFile(MOTION_FILE_PATH);
#else
        fprintf(stderr, "MX-28's firmware is not support 4096 resolution!! \n");
        fprintf(stderr, "Upgrade MX-28's firmware to version 27(0x1B) or higher.\n\n");
        exit(0);
#endif
    }
    else if(27 <= firm_ver)
    {
#ifdef MX28_1024
        fprintf(stderr, "MX-28's firmware is not support 1024 resolution!! \n");
        fprintf(stderr, "Remove '#define MX28_1024' from 'MX28.h' file and rebuild.\n\n");
        exit(0);
#else
        Action::GetInstance()->LoadFile((char*)MOTION_FILE_PATH);
#endif
    }
    else
        exit(0);

    Action::GetInstance()->m_Joint.SetEnableBody(true, true);
    MotionManager::GetInstance()->SetEnable(true);

    cm730.WriteByte(CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);

/* ============ Imu Calibration ================== */

    Imu::GetInstance()->CalibrateIMU();
//action Initialization
//LinuxActionScript::PlayMP3("../../../Data/mp3/Demonstration ready mode.mp3");
    Action::GetInstance()->Start(9);
    while(Action::GetInstance()->IsRunning()) sleep(1);	
  //  Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);

  Status = NONE

  //GoalFinder
    while (1) {
        VideoCapture ;
        

        cap.read(image);
        Behavior::GetInstance()->checkPost2();
        GoalFinder::GetInstance()->Process(image, true)

    }

    return 0;
}