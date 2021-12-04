#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

enum class State { Initial, Playing, Paused};

struct GameData {
  State m_state{State::Initial};
};

#endif