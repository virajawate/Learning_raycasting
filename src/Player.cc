#include "Player.h"

std::vector<float> Player::get_player_pose(){
    /* Player Pose
        Pose -> X
        Pose -> Y
        Pose -> Orientation
    */
    std::vector<float> player_pose;
    player_pose[0] = position.x;
    player_pose[1] = position.y;
    player_pose.push_back(angle);
    return player_pose;
}