// RobotBuilder Version: 1.5
//
// This file was generated by RobotBuilder. It contains sections of
// code that are automatically generated and assigned by robotbuilder.
// These sections will be updated in the future when you export to
// C++ from RobotBuilder. Do not put any code or make any change in
// the blocks indicating autogenerated code or it will be lost on an
// update. Deleting the comments indicating the section will prevent
// it from being updated in the future.


#include "DriveCmd.h"

DriveCmd::DriveCmd(int mode, double timeout, double speed, double distance) {
	// Use requires() here to declare subsystem dependencies
	// eg. requires(chassis);
	this->mode = mode;					// Mode 1=Distance, 2=Contact, 3=Sonar Distance, 4=Timeout
	this->timeout = timeout;
	this->speed = speed;				// Fwd = +    backt = -    Rt = -    Lft = +
	this->distance = distance;

	// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=REQUIRES
	Requires(Robot::drivetrain);

    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=REQUIRES
}

// Called just before this Command runs the first time
void DriveCmd::Initialize() {
	SetTimeout(timeout);
	Robot::drivetrain->ResetEncoders();
	Robot::drivetrain->SetFirstTimeFlag( 0 );		// set flag to 0 for gyro correction

	// --------------- mode 0 fwd/back distance -------------------
	if (mode == 0){
		beginstartdist = Robot::drivetrain->GetDistance();
		if (speed > 0)		// going forward
			beginenddist = beginstartdist + distance;
		else
			beginenddist = beginstartdist - distance;
	}

	// --------------- Mode 10 left/right distance -----------------
	if (mode == 10){
		beginstartdist = Robot::drivetrain->GetHorizontalDistance();
		if (speed > 0)		// going right increasing number
			beginenddist = beginstartdist + distance;
		else						// going left decreasing number
			beginenddist = beginstartdist - distance;
	}

	// ----------------- Mode 12 looks like mode 10 Lft/Right but look at contact sw for direction ---
	if (mode == 12){	// Look at contact switches for lateral movement
		beginstartdist = Robot::drivetrain->GetHorizontalDistance();
		speed = .3;
		if  ((Robot::fork->contactLeft->Get() ==  Robot::drivetrain->kLmtSwPressed) &&
			 (Robot::fork->contactRight->Get() ==  Robot::drivetrain->kLmtSwPressed)){
			// both are pressed we dont need to move
			speed = 0;
		}
		else if (Robot::fork->contactLeft->Get() ==  Robot::drivetrain->kLmtSwPressed){
			// hit left switch need to go left + speed
			beginenddist = beginstartdist + distance;
			if (speed < 0) speed = speed * -1; // speed needs to be positive for left
		}
		else if (Robot::fork->contactRight->Get() ==  Robot::drivetrain->kLmtSwPressed){
			// hit right switch need to go right - speed
			beginenddist = beginstartdist - distance;
			if (speed > 0) speed = speed * -1;	// must be a negative number to go right
			}
		else {
			// neither button pressed get out
			speed = 0;
		}
	}
}

// Called repeatedly when this Command is scheduled to run
void DriveCmd::Execute() {

	if (mode < 10){
	//speed gets reversed because of motor controller now -=fwd +=back
	Robot::drivetrain->DriveStraight(0, speed * -1); // 0 = Fwd/Back & 1 = left or right direction
	return;
	}

	if (mode >= 10){
		// this drive right/left
		Robot::drivetrain->DriveStraight(1, speed * -1); // 1 = left or right direction, (speed + = right, - = left)
	}
}

// Make this return true when this Command no longer needs to run execute()
bool DriveCmd::IsFinished() {
	// -------------------- Timeout -------------------------------------------------
	if (IsTimedOut())	// used in all modes
		return true;

	// --------------------- Mode 0 : Fwd/Back based on Distance --------------------------
	if (mode == 0){
		if (speed >= 0)	// Going Forward
			if (Robot::drivetrain->GetDistance() >= beginenddist){
				return true;}
		if (speed <=0 ){
			if (Robot::drivetrain->GetDistance() <= beginenddist)
				return true;}
		return false;
	}
	// --------------------- Mode 1 : Fwd/Back till Contact --------------------------
	if (mode == 1){
		if (this->speed <=0 ) return true;	// if going backwards will never hit object
		if (Robot::drivetrain->GetContact() == Robot::drivetrain->kLmtSwPressed)
				return true;	// we hit the object
		else
			return false;
	}

	// --------------------- Mode 10 : Left/Right based on Distance --------------------------
	if ((mode == 10) || (mode == 12)) {
		if (speed > 0 && (Robot::drivetrain->GetHorizontalDistance() >= beginenddist))
			return true;
		if (speed < 0 && (Robot::drivetrain->GetHorizontalDistance() <= beginenddist))
			return true;
		if (speed == 0)
			return true;
		return false;
	}

	// --------------------- Default continue running -------------------------------------
	return false;
}

// Called once after isFinished returns true
void DriveCmd::End() {
	Robot::drivetrain->Stop();
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void DriveCmd::Interrupted() {
	End();
}
