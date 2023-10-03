/*------------------------------------------------------------------------------------------
*File:      template.h
*Author:    Parker McDonnell
*Lab:       AIM Robotics Lab
*Company:   University of Colorado Boulder
*
*-------------------------------------------------------------------------------------------*/

#ifndef TEMPLATE_H
#define TEMPLATE_H

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------

#include "Arduino.h"
#include <elapsedMillis.h>
//-------------------------------------------------------------------------------------------
//                                        DEFINES
//-------------------------------------------------------------------------------------------

#define   WAVEFORM_LENGTH     255
#define   DAC_MAX             4096
#define   DAC_VREF            3.3
#define   HV_AMPLIFIER_GAIN   80
#define   HV_MAX              200
//-------------------------------------------------------------------------------------------
//                                  EXTERNAL VARIABLES
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//                              PUBLIC FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////
//@brief:	
//
//
//@param:   
//@return:  
//@outcome: 
/////////////////////////////////////////////////////////////////////////////////////////////


class WaveGenerator
{
  public:
    explicit WaveGenerator(float freq, uint16_t amplitude, double offset, double phase, uint16_t ramp_length_ms);
    void begin();
    void run(uint16_t *lift, uint16_t *swing, bool output_enable);
    void updateParameters(bool direction);
    void saveParameters();
    void loadParameters();
    void setMode(uint8_t mode);
    void setWave(uint16_t *lift, uint16_t *swing, uint16_t lift_value, uint16_t swing_value,bool output_enable);
    void setWaveLift(uint16_t *lift, uint16_t *swing, uint16_t lift_value, bool output_enable);
    void setWaveSwing(uint16_t *lift, uint16_t *swing, uint16_t swing_value, bool output_enable);
    void generate(uint16_t *lift, uint16_t *swing, uint8_t mode, bool output_enable);
    void clearWaveform(uint16_t *lift, uint16_t *swing);
    void setFreq(float freq);
    void setAmplitude(uint8_t direction, uint16_t amplitude);
    void setPhase(double phase);
    void startRamp(bool direction,uint16_t time_ms);
    bool ramp();
    void setOffset(bool direction, double offset);
    uint16_t GetLift();
    uint16_t GetSwing();
    void printParameters();
    void printWaveform();
  private:

// 

    char      _wave_type_initial;
    float     _freq_initial;
    float     _amplitude_lift_initial;
    float     _amplitude_swing_initial;
    float     _offset_swing_initial;
    float     _offset_lift_initial;
    float     _phase_initial;
    uint8_t   _mode_initial;

// current waveform parameters 
    char      _wave_type;
    float     _freq;
    float     _amplitude_lift;
    float     _amplitude_swing;
    float     _offset_swing;
    float     _offset_lift;
    float     _phase;
    uint32_t  _time_update_micros;
    uint16_t  _wave_index;
    uint8_t   _mode;

// next waveform parameters (set when waveform index = 0)
    char      _wave_type_next;
    float     _freq_next;
    float     _amplitude_lift_next;
    float     _amplitude_swing_next;
    float     _offset_swing_next;
    float     _offset_lift_next;
    float     _phase_next;
    uint32_t  _time_update_micros_next;
    uint8_t   _mode_next;

// waveform parameters for ramping

    bool _wave_lift_updated;
    bool _wave_swing_updated;

    uint16_t  _lift;
    uint16_t  _swing;
    uint16_t  _lift_next;
    uint16_t  _swing_next;

    uint16_t  _ramp_length_ms;
    bool      _ramp_enabled;
    bool      _ramp_up;
    bool      _ramp_first_call;

    float _ramp_amplitude_lift_slew_rate = 0;
    float _ramp_offset_lift_slew_rate = 0;
    float _ramp_amplitude_swing_slew_rate = 0;
    float _ramp_offset_swing_slew_rate = 0;

    elapsedMicros _time_micros;
    elapsedMillis _ramp_time_ms;

    enum _WaveType
    {
      SINE,
      SQUARE,
      TRIANGLE,
      SAWTOOTH,
      RAMP,
      DC
    };
    
    enum _Direction
    {
      LIFT,
      SWING
    };

    enum _Mode
    {
      LIFT_SWING,
      LIFT_ONLY,
      SWING_ONLY,
      OFF
    };

};

#endif  //TEMPLATE_H