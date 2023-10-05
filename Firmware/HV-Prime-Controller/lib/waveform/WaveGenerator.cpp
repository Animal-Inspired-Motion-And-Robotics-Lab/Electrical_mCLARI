/*------------------------------------------------------------------------------------------
*File:      template.cpp
*Author:    Parker McDonnell
*Lab:       AIM Robotics Lab
*Company:   University of Colorado Boulder
*
*-------------------------------------------------------------------------------------------*/

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------

#include "Arduino.h"
#include "WaveGenerator.h"

//-------------------------------------------------------------------------------------------
//                                        DEFINES
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//                              PRIVATE FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//                                  EXTERNAL VARIABLES
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//                                      CODE START
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


WaveGenerator::WaveGenerator(float freq=1, uint16_t amplitude=225, double offset=1, double phase=90, uint16_t ramp_length_ms=500) //ms)
{

// the active values for our waveform.
  _wave_type        = 's';
  _freq             = freq;
  _amplitude_lift   = amplitude;
  _amplitude_swing  = amplitude;
  _offset_lift      = offset;
  _offset_swing     = offset;
  _phase            = phase;
  _ramp_length_ms   = ramp_length_ms;
  _wave_index       = 0;
  _ramp_enabled     = true;
  _mode             = 3;    // inactive by default

// the initial settings for our waveform
  _wave_type_initial  = 's';
  _freq_initial       = freq;
  _amplitude_lift     = amplitude;
  _amplitude_swing    = amplitude;
  _offset_lift        = offset;
  _offset_swing       = offset;
  _phase              = phase;
  _mode               = 3;    // inactive by default

// the next settings for our waveform
  _freq_next             = freq;
  _amplitude_lift_next   = amplitude;
  _amplitude_swing_next  = amplitude;
  _offset_lift_next      = offset;
  _offset_swing_next     = offset;
  _phase_next            = phase;
  _mode_next             = 3;    // inactive by default

}

void WaveGenerator::begin()
{
  // reset the elapsed time timers initially
  _time_micros  = 0;
  _ramp_time_ms = 0;

  // calculate the update period
  _time_update_micros = 1e6 / ( _freq * WAVEFORM_LENGTH );

}

// call repeatedly to update the waveform generator state machine
void WaveGenerator::run(uint16_t *lift, uint16_t *swing, bool output_enable)
{


  // if we are currently at zero output and our waveform parameters have been updated
  if((_wave_lift_updated) || !output_enable)
  {
    // update all waveform parameters before starting the next period
    updateParameters(LIFT);
  }

  if((_wave_swing_updated) || !output_enable)
  {
    // update all waveform parameters before starting the next period
    updateParameters(SWING);
  }

  // run the desired waveform mode
  switch (_mode)
  {
    // lift and swing enabled
    case 0:
      // generate lift & swing waveforms
      generate(lift,swing, LIFT_SWING, output_enable);
      break;
    case 1:
      // generate lift only waveform
      generate(lift,swing, LIFT_ONLY, output_enable);
      break;
    case 2:
      // generate swing only waveform
      generate(lift,swing, SWING_ONLY, output_enable);
      break;
    default:
      // enter pause mode (application calls setWave to manually set lift/swing)
      // or user can enter 
      // no updates to waveform output
      break;
  }
  // Determine what mode we are in and whether we are ramping or not


}

void WaveGenerator::updateParameters(bool direction)
{

if(!direction)
{
  // current waveform parameters 
   _wave_type         = _wave_type_next;
   _freq              = _freq_next;
  _amplitude_lift     = _amplitude_lift_next;
  _offset_lift        = _offset_lift_next;
  _time_update_micros = _time_update_micros_next;
  _mode               = _mode_next;
  _wave_lift_updated  = false;
}
else
{
    // current waveform parameters 
   _wave_type         = _wave_type_next;
   _freq              = _freq_next;
  _amplitude_swing    = _amplitude_swing_next;
  _offset_swing       = _offset_swing_next;
  _time_update_micros = _time_update_micros_next;
  _phase              = _phase_next;
  _mode               = _mode_next;

  _wave_swing_updated = false;
}

}

// set the waveform generator back to the initial values
void WaveGenerator::saveParameters()
{
  // the active values for our waveform.
// the initial settings for our waveform
    _wave_type_initial       = _wave_type;
    _freq_initial            = _freq;
    _amplitude_lift_initial  = _amplitude_lift;
    _amplitude_swing_initial = _amplitude_swing;
    _offset_lift_initial     = _offset_lift;
    _offset_swing_initial    = _offset_swing;
    _phase_initial           = _phase;
    _mode_initial            = _mode;    // inactive by default
}

void WaveGenerator::loadParameters()
{
  // load values into our active values for our waveform.
  _wave_type        = _wave_type_initial;
  _freq             = _freq_initial;
  _amplitude_lift   = _amplitude_lift_initial;
  _amplitude_swing  = _amplitude_swing_initial;
  _offset_lift      = _offset_lift_initial;
  _offset_swing     = _offset_swing_initial;
  _phase            = _phase_initial;
  _mode             = _mode_initial;    // inactive by default
}


void WaveGenerator::clearWaveform(uint16_t *lift, uint16_t *swing)
{
  // set all waveform parameters to zero
  _wave_type_next        = 's';
  _freq_next             = 1;
  _amplitude_lift_next   = 0;
  _amplitude_swing_next  = 0;
  _offset_lift_next      = 0;
  _offset_swing_next     = 0;
  //_phase_next            = 0;
  _mode_next             = 3;    // inactive by default
  _wave_index            = 0;

  // force the paremeters to update
  updateParameters(LIFT);
  updateParameters(SWING);

  // write zero to DAC
  *lift = 0;
  *swing = 0;

}

// set the specific lift and swing value manually
void WaveGenerator::setWave(uint16_t *lift, uint16_t *swing, uint16_t lift_value, uint16_t swing_value,bool output_enable)
{
  // update the lift swing values
  _lift = lift_value;
  _swing = swing_value;

  if(output_enable)
  {
    // write value to DAC
    *lift = lift_value;
    *swing = swing_value;
  }
}

void WaveGenerator::setWaveLift(uint16_t *lift, uint16_t *swing, uint16_t lift_value, bool output_enable)
{ 
  // update the swing value
  _lift = lift_value;

  // if we are generating an output
  if(output_enable)
  {
    // write value to DAC
    *lift = lift_value;
  }
}

void WaveGenerator::setWaveSwing(uint16_t *lift, uint16_t *swing, uint16_t swing_value, bool output_enable)
{
  // update the lift value
  _swing = swing_value;

  // if we are generating an output
  if(output_enable)
  {
    // write value to DAC
    *swing = swing_value;
  }
}

// call at high frequency to generate the waveforms
void WaveGenerator::generate(uint16_t *lift, uint16_t *swing, uint8_t mode, bool output_enable)
{
  uint16_t lift_amplitude_scaled, swing_amplitude_scaled, lift_offset_scaled, swing_offset_scaled;

  // if the time since the last index update is greater than the update period
  if(_time_micros > _time_update_micros)
  {
    // increment the wave index by one (wraps around at WAVEFORM_LENGTH)
    _wave_index = (_wave_index + 1) % WAVEFORM_LENGTH;
    // reset time since laste index update
    _time_micros = 0;

    // convert the voltage amplitude into DAC counts
    lift_amplitude_scaled = ( _amplitude_lift * DAC_MAX) / ( DAC_VREF * HV_AMPLIFIER_GAIN );
    swing_amplitude_scaled = ( _amplitude_swing * DAC_MAX ) / ( DAC_VREF * HV_AMPLIFIER_GAIN );
    // convert the offset value into DAC counts
    lift_offset_scaled= ( _offset_lift * DAC_MAX ) / ( DAC_VREF * HV_AMPLIFIER_GAIN );
    swing_offset_scaled= ( _offset_swing * DAC_MAX ) / ( DAC_VREF * HV_AMPLIFIER_GAIN );



    switch(mode)
    {
      // lift and swing enabled
      case LIFT_SWING:
        // calculate the next waveform value for 
        _lift  = (lift_amplitude_scaled/2) * ( sin(2 * PI * _wave_index / WAVEFORM_LENGTH) ) + lift_offset_scaled;
        _swing = (swing_amplitude_scaled/2) * ( sin(2 * PI * _wave_index / WAVEFORM_LENGTH + _phase*PI/180) ) + swing_offset_scaled;


        // limit maximum lift/swing values
        if(_lift >= DAC_MAX)  { _lift  = DAC_MAX; }
        if(_swing >= DAC_MAX) { _swing = DAC_MAX; }

        if(output_enable)
        {
          // write value to DAC
          *lift = _lift;
          *swing = _swing;
        }
      break;
      
      case LIFT_ONLY:
        _lift  = (lift_amplitude_scaled/2) * ( sin(2 * PI * _wave_index / WAVEFORM_LENGTH) ) + lift_offset_scaled;    
                // limit maximum lift/swing values
        if(_lift > DAC_MAX)  { _lift  = DAC_MAX; }
        if(output_enable) { *lift = _lift; }
      break;
      
      case SWING_ONLY:
        _swing = (swing_amplitude_scaled/2) * ( sin(2 * PI * _wave_index / WAVEFORM_LENGTH ) ) + swing_offset_scaled;
        if(_swing > DAC_MAX) { _swing = DAC_MAX; }
        if(output_enable) { *swing = _swing; }
      break;
      
      // manual or pause mode
      default:
      break;
    }

  }

}

// set the mode the waveform generator state machine is running in
void WaveGenerator::setMode(uint8_t mode)
{
  // update the mode variable
  _mode_next = mode;
  // set waveform updated flag
  _wave_lift_updated = true;
  _wave_swing_updated = true;
}

void WaveGenerator::setOffset(bool direction , double offset)
{

  if(!direction)
  {
    _offset_lift_next = offset;
    _wave_lift_updated = true;
  }
  else
  {
    _offset_swing_next = offset;
    _wave_swing_updated = true;
  }
}

// set the amplitude of the current waveform
void WaveGenerator::setAmplitude(uint8_t direction, uint16_t amplitude)
{
    switch (direction)
    {
      // update the lift amplitude
      case LIFT:
        _amplitude_lift_next = amplitude;
        _wave_lift_updated = true;
        break;
      // update the swing amplitude
      case SWING:
        _amplitude_swing_next = amplitude;
        _wave_swing_updated = true;
        break;
      // set all amplitudes to the same level
      default:
          _amplitude_lift_next = amplitude;
          _amplitude_swing_next = amplitude;
          _wave_lift_updated = true;
          _wave_swing_updated = true;
        break;  
    } 
}

void WaveGenerator::setPhase(double phase)
{
  // update the phase variable
  _phase_next = phase;
  // set waveform updated flag
  _wave_swing_updated = true;
}

void WaveGenerator::setFreq(float freq)
{
  _freq_next = freq;
  // update takes place immediately
  _time_update_micros_next = 1e6 / ( freq * WAVEFORM_LENGTH );
  Serial.print("MSG  >> Waveform update time: ");
  Serial.print(_time_update_micros_next);
  Serial.println(" us!");
  // set waveform updated flag
  _wave_lift_updated = true;
  _wave_swing_updated = true;
}

void WaveGenerator::startRamp(bool direction,uint16_t time_ms)
{
  // update our ramp parameters
  _ramp_enabled = true;
  _ramp_length_ms = time_ms;
  _ramp_up = direction;
  _ramp_first_call = true;
}
//TODO: Add swing ramping as well
bool WaveGenerator::ramp()
{
  
  static float _amplitude_lift_final, _offset_lift_final, _amplitude_swing_final, _offset_swing_final;
  float ramp_length_float = _ramp_length_ms;
  // slew rate lift and swing variables

  // final desired value
  //static uint16_t amplitude_lift_desired, offset_lift_desired, amplitude_swing_desired, offset_swing_desired = 0;
  // time remaining for down ramp variable
  //uint32_t ramp_time_remaining_ms = 0;

  if(_ramp_first_call)
  {
    _ramp_offset_lift_slew_rate = _offset_lift / ramp_length_float;
    _ramp_amplitude_swing_slew_rate = _amplitude_swing / ramp_length_float;
    _ramp_offset_swing_slew_rate = _offset_swing / ramp_length_float;
    _ramp_amplitude_lift_slew_rate = _amplitude_lift / ramp_length_float;

    // record the final values
    if(_ramp_up)
    {
      // set the final values
      _amplitude_lift_final  = _amplitude_lift;
      _offset_lift_final     = _offset_lift;
      _amplitude_swing_final = _amplitude_swing;
      _offset_swing_final    = _offset_swing;
    }
    else
    {
      // set the final values
      _amplitude_lift_final  = 0;
      _offset_lift_final     = 0;
      _amplitude_swing_final = 0;
      _offset_swing_final    = 0;
    }

    // reset the ramp time counter
    _ramp_time_ms = 0;
    // set the first call flag to false
    _ramp_first_call = false;
  }

  if(_ramp_time_ms < _ramp_length_ms)
  {
    // we are ramping up
    if(_ramp_up)
    {
      _amplitude_lift_next  = _ramp_time_ms * _ramp_amplitude_lift_slew_rate;
      _offset_lift_next     = _ramp_time_ms * _ramp_offset_lift_slew_rate;
      _amplitude_swing_next = _ramp_time_ms * _ramp_amplitude_swing_slew_rate;
      _offset_swing_next    = _ramp_time_ms * _ramp_offset_swing_slew_rate;
      
      // Serial.print("MSG  >> Ramp time: ");
      // Serial.print(_ramp_time_ms);
      // Serial.print(" Amplitude lift: ");
      // Serial.println(_offset_lift);

    }
    else
    {
      // we are ramping down
      _amplitude_lift_next  = (_ramp_length_ms - _ramp_time_ms) * _ramp_amplitude_lift_slew_rate;
      _offset_lift_next     = (_ramp_length_ms - _ramp_time_ms) * _ramp_offset_lift_slew_rate;
      _amplitude_swing_next = (_ramp_length_ms - _ramp_time_ms) * _ramp_amplitude_swing_slew_rate;
      _offset_swing_next    = (_ramp_length_ms - _ramp_time_ms) * _ramp_offset_swing_slew_rate;
    }

  }
  else
  {
    Serial.println(_amplitude_lift_final);
    _amplitude_lift_next  = _amplitude_lift_final;
    _offset_lift_next     = _offset_lift_final;
    _amplitude_swing_next = _amplitude_swing_final;
    _offset_swing_next    = _offset_swing_final;
    // we are no longer ramping
    _ramp_enabled = false;
  }
// force the paremeters to update
updateParameters(LIFT);
updateParameters(SWING);

return _ramp_enabled;

}

uint16_t WaveGenerator::GetLift()
{
  return _lift;
}

uint16_t WaveGenerator::GetSwing()
{
  return _swing;
}

void WaveGenerator::printParameters()
{
  Serial.println("MSG  >> Printing waveform values:");
  Serial.println("==============================================================");
  Serial.print("MSG  >> Waveform frequency: ");
  Serial.print(_freq);
  Serial.println(" Hz!");
  Serial.print("MSG  >> Waveform lift offset: ");
  Serial.print(_offset_lift);
  Serial.println(" V!");
  Serial.print("MSG  >> Waveform swing offset: ");
  Serial.print(_offset_swing);
  Serial.println(" V!");
  Serial.print("MSG  >> Waveform Amplitude Lift value: ");
  Serial.print(_amplitude_lift);
  Serial.println("!");
  Serial.print("MSG  >> Waveform Amplitude Swing value: ");
  Serial.print(_amplitude_swing);
  Serial.println("!");
  Serial.print("MSG  >> Waveform phase: ");
  Serial.print(_phase);
  Serial.println(" deg!");
  Serial.print("MSG  >> Waveform update time: ");
  Serial.print(_time_update_micros);
  Serial.println(" us!");
  Serial.print("MSG  >> Waveform index: ");
  Serial.print(_wave_index);
  Serial.println("!");
  Serial.println("==============================================================");
  Serial.println("MSG  >> End of waveform values");
}

// print out our waveforms to the serial port
void WaveGenerator::printWaveform()
{
  for(uint8_t i=0;i<WAVEFORM_LENGTH;i++)
  {
    _lift  = (_amplitude_lift/2) * ( sin(2 * PI * i / WAVEFORM_LENGTH) ) + _offset_lift;
    _swing = (_amplitude_swing/2) * ( sin(2 * PI * i / WAVEFORM_LENGTH + _phase*PI/180) ) + _offset_swing;
    Serial.print(">lift:");
    Serial.println(_lift);
    Serial.print(">swing:");
    Serial.println(_swing);
  }
}