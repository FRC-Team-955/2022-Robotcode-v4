#ifndef XYALIGH
#define XYALIGH

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h" 
//#include "ball_manager.h"

#include "drivebase.h"

using namespace DriveConst;

class XYalign{
    public:
        XYalign(DriveBase *drivebase, Joystick *joystick):drivebase(drivebase),joystick(joystick){
            //change name of table later
            auto table = nt::NetworkTableInstance::GetDefault().GetTable("photonvision");
        };
        void Align(photonlib::PhotonPipelineResult limeresult);
        bool HasTargetLimeLight(photonlib::PhotonPipelineResult limeresult);

    private:
<<<<<<< Updated upstream
        DriveBase *drivebase; 

        frc::Joystick *joystick;        
=======
        DriveBase *drivebase;
>>>>>>> Stashed changes
};
#endif