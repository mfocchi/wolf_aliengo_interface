#include "aliengo_hal/aliengo_hal.h"

#include <iostream>
#include <algorithm>


namespace aliengohal
{

// ---------
// Low-Level
// ---------
LowLevelInterface::LowLevelInterface(): safe_(unitree::LeggedType::Aliengo), udp_(unitree::LOWLEVEL)
{
    udp_.InitCmdData(lowcmd_);

    lowstate_.imu.quaternion[0] = 1.;
}

LowState LowLevelInterface::ReceiveObservation()
{
    udp_.Recv();
    udp_.GetRecv(lowstate_);
    return lowstate_;
}

void LowLevelInterface::InitCmdData(LowCmd& motorcmd) {
    udp_.InitCmdData(motorcmd);
}

void LowLevelInterface::SendLowCmd(LowCmd& motorcmd)
{
    // // TODO: Is it OK to force PMSM mode. Is it OK to do this operation here?
    // motorcmd.levelFlag = unitree::LOWLEVEL;
    // for (int motor_id = 0; motor_id < 12; ++motor_id)
    // {
    //   motorcmd.motorCmd[motor_id].mode = 0x0A; // Motor operation mode || 0x00: Electronic brake mode | 0x0A: Servo (PMSM) mode
    //   // std::cout << motorcmd.motorCmd[motor_id].tau << '\n';
    // }
    // // std::cout << "***********" << std::endl;

    // Safety
    safe_.PositionLimit(motorcmd);
    // safe.PowerProtect(motorcmd, lowstate_, 1); // 3rd argument is the input factor:1~10. Means 10%~100% power limit. If you are new, then use 1; if you are familiar, then can try bigger number or even comment this function. */
    safe_.PowerProtect(motorcmd, lowstate_, 5); // 3rd argument is the input factor:1~10. Means 10%~100% power limit. If you are new, then use 1; if you are familiar, then can try bigger number or even comment this function. */

    // safe.PositionProtect(motorcmd, lowstate_, 0.087); // TODO: This throws an error. Define a better limit?. Default limit is 5 degree

    // // // FORCE Motor mode
    // // for (int motor_id = 0; motor_id < 12; motor_id++) {
    // //   motorcmd.motorCmd[motor_id].mode = 0x0A; // Motor operation mode || 0x00: Electronic brake mode | 0x0A: Servo (PMSM) mode
    // // }
    //
    // // FORCE Motor mode
    // int motor_id = 8;
    // motorcmd.motorCmd[motor_id].mode = 0x0A; // Motor operation mode || 0x00: Electronic brake mode | 0x0A: Servo (PMSM) mode
    // motorcmd.motorCmd[motor_id].q = std::max<float>(-2.30, std::min<float>(motorcmd.motorCmd[motor_id].q, -1.20));  // Range: [-2.775, -0.611]
    // motorcmd.motorCmd[motor_id].dq = std::max<float>(-8.00, std::min<float>(motorcmd.motorCmd[motor_id].dq, 8.00));
    // motorcmd.motorCmd[motor_id].tau = std::max<float>(-0.50, std::min<float>(motorcmd.motorCmd[motor_id].tau, 25.00));

    // Set command data
    udp_.SetSend(motorcmd); // Unitree SDK

    // Send UDP package to
    udp_.Send();
}

void LowLevelInterface::SendCommand(std::array<float, 60> motorcmd)
{
    lowcmd_.levelFlag = unitree::LOWLEVEL;
    for (int motor_id = 0; motor_id < 12; ++motor_id)
    {
        lowcmd_.motorCmd[motor_id].q = motorcmd[motor_id*5];
        lowcmd_.motorCmd[motor_id].dq = motorcmd[motor_id*5 + 1]; // NOTE: this order is different than google
        lowcmd_.motorCmd[motor_id].Kp = motorcmd[motor_id*5 + 2]; // NOTE: this order is different than google
        lowcmd_.motorCmd[motor_id].Kd = motorcmd[motor_id*5 + 3];
        lowcmd_.motorCmd[motor_id].tau = motorcmd[motor_id*5 + 4];
    }
    LowLevelInterface::SendLowCmd(lowcmd_);
}


// ----------
// High-Level
// ----------
HighLevelInterface::HighLevelInterface(): safe_(unitree::LeggedType::Aliengo), udp_(unitree::HIGHLEVEL)
{
}

HighState HighLevelInterface::ReceiveObservation()
{
    udp_.Recv();
    udp_.GetRecv(highstate_);
    return highstate_;
}

void HighLevelInterface::SendHighCmd(HighCmd& descmd)
{
    // // TODO: Is it OK to force PMSM mode. Is it OK to do this operation here?
    // motorcmd.levelFlag = unitree::LOWLEVEL;
    // for (int motor_id = 0; motor_id < 12; motor_id++) {
    //   motorcmd.motorCmd[motor_id].mode = 0x0A; // Motor operation mode || 0x00: Electronic brake mode | 0x0A: Servo (PMSM) mode
    //   // std::cout << motorcmd.motorCmd[motor_id].tau << '\n';
    // }
    // // std::cout << "***********" << std::endl;

    // // Safety
    // safe.PositionLimit(descmd);
    // safe.PowerProtect(descmd, highstate_, 1);

    // Set command data
    udp_.SetSend(descmd); // Unitree SDK

    // Send UDP package to
    udp_.Send();
}


} // namespace aliengohal
