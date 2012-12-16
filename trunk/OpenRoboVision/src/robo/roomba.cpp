//
// OpenRoboVision
//
// iRobot Roomba control via serial port on 
// DIN socket on the side.
//
//
// robocraft.ru
//

// Roomba.cpp
//
// Copyright (C) 2010 Mike McCauley
// $Id: Roomba.cpp,v 1.1 2010/09/27 21:58:32 mikem Exp mikem $

#include "orv/robo/roomba.h"

Roomba::Roomba(Serial* serial, Baud baud):
del_serial(false)
{
  _serial = serial;
  _baud = baudCodeToBaudRate(baud);
  _pollState = PollStateIdle;
}

Roomba::Roomba(const char* name, Baud baud):
del_serial(true)
{
	_baud = baudCodeToBaudRate(baud);
	_serial = new Serial(name, _baud);
	_pollState = PollStateIdle;
}

Roomba::Roomba(const char* name, int rate):
del_serial(true)
{
	_serial = new Serial(name, rate);
	_pollState = PollStateIdle;
}

Roomba::~Roomba()
{
	reset();
	//power();
	if(del_serial)
	{
		delete _serial;
		_serial = NULL;
	}
}

// Resets the Roomba
void Roomba::reset()
{
    _serial->write(7);
}

// wake up robot (via Device Detect line)
void Roomba::wakeUp()
{
	// wake up the robot
	_serial->setControl(-1, 1);
	orv::time::sleep(100);
	_serial->setControl(-1, 0);
	orv::time::sleep(500);
	_serial->setControl(-1, 1);
	orv::time::sleep(2000);
}

// Start OI
// Changes mode to passive
void Roomba::start()
{
    _serial->setBaud(_baud);
    
	// set up ROI to receive commands
	_serial->write(128); // START 
	// Starts the ROI. The Start command must be sent before any
	// other ROI commands. This command puts the ROI in passive
	// mode.
	orv::time::sleep(50);
}

// Enables user control of Roomba.
void Roomba::enableControl()
{
	_serial->write(130); // CONTROL 
	orv::time::sleep(50);
}

uint32_t Roomba::baudCodeToBaudRate(Baud baud)
{
    switch (baud)
    {
	case Baud300:
	    return 300;
	case Baud600:
	    return 600;
	case Baud1200:
	    return 1200;
	case Baud2400:
	    return 2400;
	case Baud4800:
	    return 4800;
	case Baud9600:
	    return 9600;
	case Baud14400:
	    return 14400;
	case Baud19200:
	    return 19200;
	case Baud28800:
	    return 28800;
	case Baud38400:
	    return 38400;
	case Baud57600:
	    return 57600;
	case Baud115200:
	    return 115200;
	default:
	    return 57600;
    }
}

// Sets the baud rate in bits per second (bps) at which ROI
// commands and data are sent according to the baud code sent
// in the data byte. The default baud rate at power up is 57600
// bps.  Once the baud rate is
// changed, it will persist until Roomba is power cycled by removing
// the battery (or until the battery voltage falls below the minimum
// required for processor operation). You must wait 100ms after
// sending this command before sending additional commands
// at the new baud rate. The ROI must be in passive, safe, or full
// mode to accept this command. This command puts the ROI in
// passive mode.
void Roomba::baud(Baud baud)
{
    _serial->write(129);
    _serial->write(baud);

    _baud = baudCodeToBaudRate(baud);
    _serial->setBaud(_baud);
}

// This command puts the ROI in safe mode. The ROI must be in
// full mode to accept this command.
void Roomba::safeMode()
{
  _serial->write(131);
}

// Enables unrestricted control of Roomba through the ROI and
// turns off the safety features. The ROI must be in safe mode to
// accept this command. This command puts the ROI in full mode.
void Roomba::fullMode()
{
  _serial->write(132);
}

// Puts Roomba to sleep, the same as a normal “power” button
// press. The Device Detect line must be held low for 500 ms to
// wake up Roomba from sleep. The ROI must be in safe or full
// mode to accept this command. This command puts the ROI in
// passive mode.
void Roomba::power()
{
  _serial->write(133);
}

// Turns on force-seeking-dock mode, which causes the robot
// to immediately attempt to dock during its cleaning cycle if it
// encounters the docking beams from the Home Base. (Note,
// however, that if the robot was not active in a clean, spot or max
// cycle it will not attempt to execute the docking.) Normally the
// robot attempts to dock only if the cleaning cycle has completed
// or the battery is nearing depletion. This command can be sent
// anytime, but the mode will be cancelled if the robot turns off,
// begins charging, or is commanded into ROI safe or full modes.
void Roomba::dock()
{
  _serial->write(143);
}

// Starts a maximum time cleaning cycle, the same as a normal
// “max” button press. The ROI must be in safe or full mode to
// accept this command. This command puts the ROI in passive
// mode.
void Roomba::demo(Demo demo)
{
  _serial->write(136);
  _serial->write(demo);
}

// Starts a normal cleaning cycle, the same as a normal “clean”
// button press. The ROI must be in safe or full mode to accept this
// command. This command puts the ROI in passive mode.
void Roomba::cover()
{
  _serial->write(135);
}

void Roomba::coverAndDock()
{
  _serial->write(135);
  orv::time::sleep(50);
  _serial->write(143);
}

// Starts a spot cleaning cycle, the same as a normal “spot”
// button press. The ROI must be in safe or full mode to accept this
// command. This command puts the ROI in passive mode.
void Roomba::spot()
{
  _serial->write(134);
}

// Controls Roomba’s drive wheels. The command takes four data
// bytes, which are interpreted as two 16 bit signed values using
// twos-complement. The first two bytes specify the average velocity
// of the drive wheels in millimeters per second (mm/s), with the
// high byte sent first. The next two bytes specify the radius, in
// millimeters, at which Roomba should turn. The longer radii make
// Roomba drive straighter; shorter radii make it turn more. A Drive
// command with a positive velocity and a positive radius will make
// Roomba drive forward while turning toward the left. A negative
// radius will make it turn toward the right. Special cases for the
// radius make Roomba turn in place or drive straight, as specified
// below. The ROI must be in safe or full mode to accept this
// command. This command does change the mode.
void Roomba::drive(int16_t velocity, int16_t radius)
{
  _serial->write(137);
  _serial->write((velocity & 0xff00) >> 8);
  _serial->write(velocity & 0xff);
  _serial->write((radius & 0xff00) >> 8);
  _serial->write(radius & 0xff);
}

// = Create only =
// This command lets you control the forward and backward
// motion of Create’s drive wheels independently. It takes
// four data bytes, which are interpreted as two 16-bit signed
// values using two’s complement. The first two bytes specify
// the velocity of the right wheel in millimeters per second
// (mm/s), with the high byte sent first. The next two bytes
// specify the velocity of the left wheel, in the same format.
// A positive velocity makes that wheel drive forward, while a
// negative velocity makes it drive backward.
void Roomba::driveDirect(int16_t leftVelocity, int16_t rightVelocity)
{
  _serial->write(145);
  _serial->write((rightVelocity & 0xff00) >> 8);
  _serial->write(rightVelocity & 0xff);
  _serial->write((leftVelocity & 0xff00) >> 8);
  _serial->write(leftVelocity & 0xff);
}

// Controls Roomba’s LEDs. The state of each of the spot, clean,
// max, and dirt detect LEDs is specified by one bit in the first data
// byte. The color of the status LED is specified by two bits in the
// first data byte. The power LED is specified by two data bytes, one
// for the color and one for the intensity. The ROI must be in safe
// or full mode to accept this command. This command does not
// change the mode.
void Roomba::leds(uint8_t leds, uint8_t powerColour, uint8_t powerIntensity)
{
  _serial->write(139);
  _serial->write(leds);
  _serial->write(powerColour);
  _serial->write(powerIntensity);
}

// = Create only =
// This command controls the state of the 3 digital output
// pins on the 25 pin Cargo Bay Connector. The digital outputs
// can provide up to 20 mA of current.
void Roomba::digitalOut(uint8_t out)
{
  _serial->write(147);
  _serial->write(out);
}

// = Create only =
// This command lets you control the three low side drivers
// with variable power. With each data byte, you specify the
// PWM duty cycle for the low side driver (max 128). For
// example, if you want to control a driver with 25% of battery
// voltage, choose a duty cycle of 128 * 25% = 32.

// Sets PWM duty cycles on low side drivers
void Roomba::pwmDrivers(uint8_t dutyCycle0, uint8_t dutyCycle1, uint8_t dutyCycle2)
{
  _serial->write(144);
  _serial->write(dutyCycle2);
  _serial->write(dutyCycle1);
  _serial->write(dutyCycle0);
}

// Controls Roomba’s cleaning motors. The state of each motor is
// specified by one bit in the data byte. The ROI must be in safe
// or full mode to accept this command. This command does not
// change the mode.

// Sets low side driver outputs on or off
void Roomba::drivers(uint8_t out)
{
  _serial->write(138);
  _serial->write(out);
}

// = Create only =
// This command sends the requested byte out of low side
// driver 1 (pin 23 on the Cargo Bay Connector), using the
// format expected by iRobot Create’s IR receiver. You must
// use a preload resistor (suggested value: 100 ohms) in
// parallel with the IR LED and its resistor in order turn it on.

// Modulates low side driver 1 (pin 23 on Cargo Bay Connector)
// with the given IR command
void Roomba::sendIR(uint8_t data)
{
  _serial->write(151);
  _serial->write(data);
}

// Specifies a song to the ROI to be played later. Each song is
// associated with a song number which the Play command uses
// to select the song to play. Users can specify up to 16 songs
// with up to 16 notes per song. Each note is specified by a note
// number using MIDI note definitions and a duration specified
// in fractions of a second. The number of data bytes varies
// depending on the length of the song specified. A one note song
// is specified by four data bytes. For each additional note, two data
// bytes must be added. The ROI must be in passive, safe, or full
// mode to accept this command. This command does not change
// the mode.

// Define a song
// Data is 2 bytes per note
void Roomba::song(uint8_t songNumber, const uint8_t* data, int len)
{
    _serial->write(140);
    _serial->write(songNumber);
    _serial->write(len >> 1); // 2 bytes per note
    _serial->write(data, len);
}

// Plays one of 16 songs, as specified by an earlier Song
// command. If the requested song has not been specified yet,
// the Play command does nothing. The ROI must be in safe or full
// mode to accept this command. This command does not change
// the mode.
void Roomba::playSong(uint8_t songNumber)
{
  _serial->write(141);
  _serial->write(songNumber); 
}

// = Create only =
// This command starts a continuous stream of data packets.
// The list of packets requested is sent every 15 ms, which is
// the rate iRobot Create uses to update data.
// This is the best method of requesting sensor data if you
// are controlling Create over a wireless network (which has
// poor real-time characteristics) with software running on a
// desktop computer.

// Start a stream of sensor data with the specified packet IDs in it
void Roomba::stream(const uint8_t* packetIDs, int len)
{
  _serial->write(148);
  _serial->write(packetIDs, len);
}

// = Create only =
// This command lets you stop and restart the steam without
// clearing the list of requested packets.

// An argument of 0 stops the stream without clearing the list
// of requested packets. An argument of 1 starts the stream
// using the list of packets last requested.

// One of StreamCommand*
void Roomba::streamCommand(StreamCommand command)
{
  _serial->write(150);
  _serial->write(command);
}

// = Create only =
// This command specifies a script to be played later. A script
// consists of OI commands and can be up to 100 bytes long.
// There is no flow control, but “wait” commands (see below)
// cause Create to hold its current state until the specified
// event is detected.

// Use len=0 to clear the script
void Roomba::script(const uint8_t* script, uint8_t len)
{
  _serial->write(152);
  _serial->write(len);
  _serial->write(script, len);
}

// = Create only =
// This command loads a previously defined OI script into the
// serial input queue for playback.
void Roomba::playScript()
{
  _serial->write(153);
}

// = Create only =
// This command causes Create to wait for the specified time.
// During this time, Create’s state does not change, nor does
// it react to any inputs, serial or otherwise.
// * Serial sequence: [155] [time]
// * Available in modes: Passive, Safe, or Full
// * Changes mode to: No Change
// * Wait Time data byte 1: Time (0 - 255)
// Specifies time to wait in tenths of a second with a resolution
// of 15 ms.

// Each tick is 15ms
void Roomba::wait(uint8_t ticks)
{
  _serial->write(155);
  _serial->write(ticks);
}

// = Create only =
// This command causes iRobot Create to wait until it has
// traveled the specified distance in mm. When Create travels
// forward, the distance is incremented. When Create travels
// backward, the distance is decremented. If the wheels
// are passively rotated in either direction, the distance is
// incremented. Until Create travels the specified distance,
// its state does not change, nor does it react to any inputs,
// serial or otherwise.
void Roomba::waitDistance(int16_t mm)
{
  _serial->write(156);
  _serial->write((mm & 0xff00) >> 8);
  _serial->write(mm & 0xff);
}

// = Create only =
// This command causes Create to wait until it has rotated
// through specified angle in degrees. When Create turns
// counterclockwise, the angle is incremented. When Create
// turns clockwise, the angle is decremented. Until Create
// turns through the specified angle, its state does not change,
// nor does it react to any inputs, serial or otherwise.
void Roomba::waitAngle(int16_t degrees)
{
  _serial->write(157);
  _serial->write((degrees & 0xff00) >> 8);
  _serial->write(degrees & 0xff);
}

// = Create only =
// This command causes Create to wait until it detects the
// specified event. Until the specified event is detected,
// Create’s state does not change, nor does it react to any
// inputs, serial or otherwise.

// Can use the negative of an event type to wait for the inverse of an event
void Roomba::waitEvent(EventType type)
{
  _serial->write(158);
  _serial->write(type);
}

// Reads at most len bytes and stores them to dest
// If successful, returns true.
// If there is a timeout, returns false
// Blocks until all bytes are read
// Caller must ensure there is sufficient space in dest
bool Roomba::getData(uint8_t* dest, uint8_t len)
{
  while (len-- > 0)
  {
    unsigned long startTime = orv::time::millis();
    while (!_serial->available())
    {
      // Look for a timeout
      if (orv::time::millis() > startTime + ROOMBA_READ_TIMEOUT)
        return false; // Timed out
    }
	uint8_t data = 0; _serial->read(&data, 1);
    *dest++ = data;
  }
  return true;
}

// Requests the ROI to send a packet of sensor data bytes. The
// user can select one of four different sensor packets. 
// The ROI must be in passive, safe, or full mode to accept this
// command. This command does not change the mode.
// 0 - 26 bytes
// 1 - 10 bytes
// 2 - 6 bytes
// 3 - 10 bytes
// = Create =
// 4 - 14 bytes 
// 5 - 12 bytes
// 6 - 52 bytes
bool Roomba::getSensors(uint8_t packetID, uint8_t* dest, uint8_t len)
{
  _serial->write(142);
  _serial->write(packetID);
  return getData(dest, len);
}

// = Create only =
// This command lets you ask for a list of sensor packets.
// The result is returned once, as in the Sensors command.
// The robot returns the packets in the order you specify.
bool Roomba::getSensorsList(uint8_t* packetIDs, uint8_t numPacketIDs, uint8_t* dest, uint8_t len)
{
  _serial->write(149);
  _serial->write(numPacketIDs);
  _serial->write(packetIDs, numPacketIDs);
  return getData(dest, len);
}

// Simple state machine to read sensor data and discard everything else
bool Roomba::pollSensors(uint8_t* dest, uint8_t len)
{
    while (_serial->available())
    {
	uint8_t ch = 0; _serial->read(&ch, 1);
	switch (_pollState)
	{
	    case PollStateIdle:
		if (ch == 19)
		    _pollState = PollStateWaitCount;
		break;

	    case PollStateWaitCount:
		_pollChecksum = _pollSize = ch;
		_pollCount = 0;
		_pollState = PollStateWaitBytes;
		break;

	    case PollStateWaitBytes:
		_pollChecksum += ch;
		if (_pollCount < len)
		    dest[_pollCount] = ch;
		if (_pollCount++ >= _pollSize)
		    _pollState = PollStateWaitChecksum;
		break;

	    case PollStateWaitChecksum:
		_pollChecksum += ch;
		_pollState = PollStateIdle;
		return (_pollChecksum == 0);
		break;
	}
    }
    return false;
}

// = Create only =
// This command returns the values of a previously stored
// script, starting with the number of bytes in the script and
// followed by the script’s commands and data bytes. It first
// halts the sensor stream, if one has been started with a
// Stream or Pause/Resume Stream command. To restart the
// stream, send Pause/Resume Stream (opcode 150).

// Returns the number of bytes in the script, or 0 on errors
// Only saves at most len bytes to dest
// Calling with len = 0 will return the amount of space required without actually storing anything
uint8_t Roomba::getScript(uint8_t* dest, uint8_t len)
{
  _serial->write(154);

  unsigned long startTime = orv::time::millis();
  while (!_serial->available())
  {
    // Look for a timeout
    if (orv::time::millis() > startTime + ROOMBA_READ_TIMEOUT)
      return 0; // Timed out
  }

  int count=0; _serial->read(&count, 1);
  if (count > 100 || count < 0)
    return 0; // Something wrong. Cant have such big scripts!!

  // Get all the data, saving as much as we can
  uint8_t i;
  for (i = 0; i < count; i++)
  {
    startTime = orv::time::millis();
    while (!_serial->available())
    {
      // Look for a timeout
      if (orv::time::millis() > startTime + ROOMBA_READ_TIMEOUT)
        return 0; // Timed out
    }
    uint8_t data = 0; _serial->read(&data, 1);
    if (i < len)
      *dest++ = data;
  }

  return count;
}
