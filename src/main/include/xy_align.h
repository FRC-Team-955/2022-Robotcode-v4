#ifndef XYALIGH
#define XYALIGH

#include <frc/Joystick.h>
#include <iostream>
#include <AHRS.h>
#include <ctre/Phoenix.h>
#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h" 
//#include "ball_manager.h"

#include "drivebase.h"

using namespace frc;

class XYalign{
    public:
        XYalign(DriveBase *drivebase):drivebase(drivebase) {
            //change name of table later
            auto table = nt::NetworkTableInstance::GetDefault().GetTable("photonvision");
        };
        void Align(photonlib::PhotonPipelineResult limeresult);
        bool HasTargetLimeLight(photonlib::PhotonPipelineResult limeresult);

    private:
        DriveBase *drivebase; 
        frc::Joystick joystick{0};
        BallManager ball_manage;

        
};
#endif