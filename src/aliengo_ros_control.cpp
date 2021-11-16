#include "wb_aliengo_interface/aliengo_ros_control.hpp"


namespace aliengo2ros {


AliengoROSControl::AliengoROSControl()
{

}


AliengoROSControl::~AliengoROSControl()
{

}


void AliengoROSControl::init()
{
    ROS_INFO_NAMED(CLASS_NAME,"Initializing AliengoROSControl");

	// Reset RobotHW
	robot_hw_.reset(new aliengo2ros::AliengoRobotHw);

	// Reseting the namespace of the node handle
	node_handle_.reset(new ros::NodeHandle(robot_hw_->getRobotName()));

	// Initializing the hardware interface
	robot_hw_->init();

	// Reseting the controller manager
	controller_manager_.reset(new controller_manager::ControllerManager(robot_hw_.get(), *node_handle_.get()));
}


void AliengoROSControl::update(const ros::Time& time, const ros::Duration& period)
{
	// Reading sensor information
	robot_hw_->read();

	// Updating the controller manager
	controller_manager_->update(time, period);

	// Writing to the actuator
	robot_hw_->write();
}


}
