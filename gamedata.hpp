#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left, Down, Up };
enum class State { Initial, Playing};

struct GameData {
  State m_state{State::Initial};
};

#endif