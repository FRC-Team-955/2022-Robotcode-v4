#ifndef COMPRESSOR
#define COMPRESSOR

#include <frc/Compressor.h>
#include <frc/Solenoid.h>

#include "settings.h"

using namespace frc;

class RobotCompressor {
  public:
    void TurnOnCompressor();
    void TurnOffCompressor();
    bool DetectPressure();    
  private:
    Compressor compressor{frc::PneumaticsModuleType::CTREPCM};
};
#endif