/*
 * Main.cpp
 *
 *  Created on: 2013.01.24.
 *      Author: Baarnus
 */

#include "Main.hpp"

namespace app
{

using namespace std;

/**
 * Push a new state into the state machine.
 * Only work if the new state is non-nil.
 * @param newState the new state to the state machine.
 */
void Main::pushState(const State::Ptr newState)
{
	if (!newState)
		return;
	m_states.push(newState);
}

/**
 * Replace the top state of the state machine.
 * Only work if the new state is non-nil.
 * @param newState the new state to the state machine replacing the top state.
 */
void Main::replaceState(const State::Ptr newState)
{
	if (!newState)
		return;
	if (!m_states.empty())
		m_states.pop();
	pushState(newState);
}

/**
 * Pop the top state from the state machine
 */
void Main::popState()
{
	if (!m_states.empty())
		m_states.pop();
}

/**
 * Pop all the state from the stack
 */
void Main::stop()
{
	mRunning = false;

	if (!m_states.empty())
		m_states.top()->onDeactivate();
	while (!m_states.empty())
	{
		m_states.pop();
		updateState();
	}
}

/**
 * Running the state machine of the main class with a start state.
 * Run the state then call update the state.
 * @param startState starting state of the application.
 */
int Main::run(State::Ptr startState)
{
	pushState(startState);
	startState.reset();
	mRunning = true;

	updateState();
	while (!m_states.empty())
	{
		m_states.top()->onRun();
		updateState();
	}
	return 0;
}

/**
 * Singleton getter function.
 * @return Main::Ptr the main instance of the main function.
 */
Main& Main::getSingleton()
{
	static Main instance;
	return instance;
}

Main& MainSingleton = Main::getSingleton();

/**
 * Default constructor.
 */
Main::Main() :
		mRunning(false)
{
}

/**
 * Update the active state and run the 'onActivate' and 'onDeactivate' functions
 * on the states.
 */
void Main::updateState()
{
	static State::Ptr s_prev_state;
	State::Ptr actState;

	// Set the actual state
	if (!m_states.empty())
		actState = m_states.top();

	// Run the corresponding functions
	if (mRunning && s_prev_state != actState)
	{
		if (s_prev_state)
			s_prev_state->onDeactivate();
		if (actState)
			actState->onActivate();
	}

	// Set the previous state!
	s_prev_state = actState;
}

} /* namespace app */
