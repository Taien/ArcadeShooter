﻿
/*	 ██╗  ██╗  █████╗  ████████╗  █████╗  ███╗   ██╗  █████╗ 
	 ██║ ██╔╝ ██╔══██╗ ╚══██╔══╝ ██╔══██╗ ████╗  ██║ ██╔══██╗
	 █████╔╝  ███████║    ██║    ███████║ ██╔██╗ ██║ ███████║
	 ██╔═██╗  ██╔══██║    ██║    ██╔══██║ ██║╚██╗██║ ██╔══██║
	 ██║  ██╗ ██║  ██║    ██║    ██║  ██║ ██║ ╚████║ ██║  ██║
	 ╚═╝  ╚═╝ ╚═╝  ╚═╝/\  ╚═╝    ╚═╝  ╚═╝ ╚═╝  ╚═══╝ ╚═╝  ╚═╝
   /vvvvvvvvvvvvvvvvvvv \=========================================,
   `^^^^^^^^^^^^^^^^^^^ /---------------------------------------"
        Katana Engine \/ © 2012 - Shuriken Studios LLC              */

#include "KatanaEngine.h"

namespace KatanaEngine
{
	InputState::InputState()
	{
		al_install_keyboard();
		al_install_joystick();
		al_install_mouse();


		for (int i = 0; i < MAX_NUM_GAMEPADSTATES; i++)
		{
			m_currentGamePadStates[i].ID = nullptr;
			m_previousGamePadStates[i].ID = nullptr;
			m_currentGamePadStates[i].IsConnected = false;
			m_previousGamePadStates[i].IsConnected = false;
		}

		if (al_is_joystick_installed())
		{
			InitializeGamePads();
		}
	}

	void InputState::Update()
	{
		if (al_is_keyboard_installed())
		{
			m_previousKeyboardState = m_currentKeyboardState;
			al_get_keyboard_state(&m_currentKeyboardState);
		}

		if (al_is_mouse_installed())
		{
			m_previousMouseState = m_currentMouseState;
			al_get_mouse_state(&m_currentMouseState);
		}

		for (int i = 0; i < MAX_NUM_GAMEPADSTATES; i++)
		{
			m_previousGamePadStates[i] = m_currentGamePadStates[i];
		}
	}

	void InputState::InitializeGamePads()
	{
		for (int i = 0; i < al_get_num_joysticks(); i++)
		{
			ALLEGRO_JOYSTICK *joystick = al_get_joystick(i);

			if (strcmp(al_get_joystick_name(joystick), "Controller (XBOX 360 For Windows)") == 0)
			{
				for (int i = 0; i < MAX_NUM_GAMEPADSTATES; i++)
				{
					if (m_currentGamePadStates[i].ID == nullptr)
					{
						m_currentGamePadStates[i].ID = joystick;
						m_previousGamePadStates[i].ID = joystick;
						m_currentGamePadStates[i].IsConnected = true;

						m_map.emplace(std::pair<ALLEGRO_JOYSTICK *, int>(joystick, i));

						break;
					}
				}
			}
		}
	}

	void InputState::UpdateConfigurationEvent()
	{
		if (al_is_joystick_installed())
		{
			if (al_reconfigure_joysticks())
			{
				InitializeGamePads();
			}
		}
	}

	void InputState::UpdateAxisEvent(ALLEGRO_EVENT alEvent)
	{
		int index = GetGamePadIndex(alEvent.joystick.id);

		if (index > -1)
		{
			if (alEvent.joystick.stick == 0 && alEvent.joystick.axis == 0)
			{
				m_currentGamePadStates[index].Thumbsticks.Left.X = alEvent.joystick.pos;
			}
			else if (alEvent.joystick.stick == 0 && alEvent.joystick.axis == 1)
			{
				m_currentGamePadStates[index].Thumbsticks.Left.Y = alEvent.joystick.pos;
			}
			else if (alEvent.joystick.stick == 0 && alEvent.joystick.axis == 2)
			{
				if (alEvent.joystick.pos >= 0) m_currentGamePadStates[index].Triggers.Left = alEvent.joystick.pos;
				else if (alEvent.joystick.pos >= 0) m_currentGamePadStates[index].Triggers.Right = -alEvent.joystick.pos;
			}
			else if (alEvent.joystick.stick == 1 && alEvent.joystick.axis == 0)
			{
				m_currentGamePadStates[index].Thumbsticks.Right.X = alEvent.joystick.pos;
			}
			else if (alEvent.joystick.stick == 1 && alEvent.joystick.axis == 1)
			{
				m_currentGamePadStates[index].Thumbsticks.Right.Y = alEvent.joystick.pos;
			}
		}
	}

	void InputState::UpdateButtonEvent(ALLEGRO_EVENT alEvent, ButtonState state)
	{
		int index = GetGamePadIndex(alEvent.joystick.id);

		if (index > -1)
		{
			switch (alEvent.joystick.button)
			{
			case 0:	m_currentGamePadStates[index].Buttons.A = state;				break;
			case 1:	m_currentGamePadStates[index].Buttons.B = state;				break;
			case 2:	m_currentGamePadStates[index].Buttons.X = state;				break;
			case 3:	m_currentGamePadStates[index].Buttons.Y = state;				break;
			case 4:	m_currentGamePadStates[index].Buttons.LeftShoulder = state;		break;
			case 5:	m_currentGamePadStates[index].Buttons.RightShoulder = state;	break;
			case 6:	m_currentGamePadStates[index].Buttons.Back = state;				break;
			case 7:	m_currentGamePadStates[index].Buttons.Start = state;			break;
			case 8:	m_currentGamePadStates[index].Buttons.LeftStick = state;		break;
			case 9:	m_currentGamePadStates[index].Buttons.RightStick = state;		break;
			}
		}
	}

	bool InputState::IsKeyDown(Key key) const
	{
		return al_key_down(&m_currentKeyboardState, (int)key);
	}

	bool InputState::IsKeyUp(Key key) const
	{
		return !al_key_down(&m_currentKeyboardState, (int)key);
	}

	bool InputState::IsNewKeyPress(Key key) const
	{
		return IsKeyDown(key) && !al_key_down(&m_previousKeyboardState, (int)key);
	}

	bool InputState::IsNewKeyRelease(Key key) const
	{
		return IsKeyUp(key) && al_key_down(&m_previousKeyboardState, (int)key);
	}

	bool InputState::IsButtonUp(Button button, int &indexOut, int controllingIndex) const
	{
		if (controllingIndex > -1 && controllingIndex < MAX_NUM_GAMEPADSTATES)
		{
			if (m_currentGamePadStates[controllingIndex].IsButtonUp(button))
			{
				indexOut = controllingIndex;
				return true;
			}

			return false;
		}
		else
		{
			return (IsButtonUp(button, indexOut, 0) || IsButtonUp(button, indexOut, 1) ||
				IsButtonUp(button, indexOut, 2) || IsButtonUp(button, indexOut, 3));
		}
	}

	bool InputState::IsButtonDown(Button button, int &indexOut, int controllingIndex) const
	{
		if (controllingIndex > -1 && controllingIndex < MAX_NUM_GAMEPADSTATES)
		{
			if (m_currentGamePadStates[controllingIndex].IsButtonDown(button))
			{
				indexOut = controllingIndex;
				return true;
			}

			return false;
		}
		else
		{
			return (IsButtonDown(button, indexOut, 0) || IsButtonDown(button, indexOut, 1) ||
				IsButtonDown(button, indexOut, 2) || IsButtonDown(button, indexOut, 3));
		}
	}

	bool InputState::IsNewButtonPress(Button button, int &indexOut, int controllingIndex) const
	{
		if (controllingIndex > -1 && controllingIndex < MAX_NUM_GAMEPADSTATES)
		{
			if (m_currentGamePadStates[controllingIndex].IsButtonDown(button) &&
				m_previousGamePadStates[controllingIndex].IsButtonUp(button))
			{
				indexOut = controllingIndex;
				return true;
			}

			return false;
		}
		else
		{
			return (IsNewButtonPress(button, indexOut, 0) || IsNewButtonPress(button, indexOut, 1) ||
				IsNewButtonPress(button, indexOut, 2) || IsNewButtonPress(button, indexOut, 3));
		}
	}

	bool InputState::IsNewButtonRelease(Button button, int &indexOut, int controllingIndex) const
	{
		if (controllingIndex > -1 && controllingIndex < MAX_NUM_GAMEPADSTATES)
		{
			if (m_currentGamePadStates[controllingIndex].IsButtonUp(button) &&
				m_previousGamePadStates[controllingIndex].IsButtonDown(button))
			{
				indexOut = controllingIndex;
				return true;
			}

			return false;
		}
		else
		{
			return (IsNewButtonRelease(button, indexOut, 0) || IsNewButtonRelease(button, indexOut, 1) ||
				IsNewButtonRelease(button, indexOut, 2) || IsNewButtonRelease(button, indexOut, 3));
		}
	}

	GamePadState InputState::GetGamePadState(const int gamePadIndex) const
	{
		return m_currentGamePadStates[gamePadIndex];
	}

	int InputState::GetGamePadIndex(ALLEGRO_JOYSTICK *pId)
	{
		return m_map[pId];
	}
}