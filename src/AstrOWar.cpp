//============================================================================
// Name        : AstrOWar.cpp
// Author      : Lászlóffy Barnabás (lasba)
// Version     :
// Copyright   : I'm all right!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "tools/structure.hpp"
#include "game/settings/GameSettings.hpp"
#include "graphics/Ogre/OgreFramework.hpp"
#include "graphics/Ois/OisFramework.hpp"
#include "graphics/Cegui/CeguiFramework.hpp"
#include "graphics/utils/OisOgreWindowListener.hpp"
#include "graphics/utils/OisCeguiInputListener.hpp"
#include "astrowar/settings/AstrOWarInitHelper.hpp"
#include "astrowar/mainmenu/AOWMainMenuState.hpp"

ostream& operator <<(ostream& out, const OIS::Axis& axis)
{
	out << "(" << axis.abs << ", ";
	if (axis.absOnly)
		cout << "X";
	else
		cout << axis.rel;
	cout << ")";
	return out;
}
ostream& operator <<(ostream& out, const OIS::MouseState& ms)
{
	out << "{X: " << ms.X << ", Y:" << ms.Y << ", Z: " << ms.Z << "}";
	return out;
}

ostream& operator <<(ostream& out, const OIS::Slider& slider)
{
	out << "(" << slider.abX << ", " << slider.abY << ")";
	return out;
}

ostream& operator <<(ostream& out, const OIS::Pov& pov)
{
	out << "(Dir: " << pov.direction << ")";
	return out;
}

class InputDumpListener: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
	InputDumpListener()
	{
		OisFrameworkSingleton.addKeyListener(this);
		OisFrameworkSingleton.addMouseListener(this);
		OisFrameworkSingleton.addJoyStickListener(this);
	}
	~InputDumpListener()
	{
		OisFrameworkSingleton.removeKeyListener(this);
		OisFrameworkSingleton.removeMouseListener(this);
		OisFrameworkSingleton.removeJoyStickListener(this);
	}
	//*** Listeners ***/
	// KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg)
	{
		cout << "Key pressed: [Dev: " << arg.device->getID() << ", Key: " << arg.key << endl;
		return true;
	}
	virtual bool keyReleased(const OIS::KeyEvent &arg)
	{
		cout << "Key released: [Dev: " << arg.device->getID() << ", Key: " << arg.key << endl;
		return true;
	}
	// MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg)
	{
		cout << "Mouse moved: [Dev: " << arg.device->getID() << ", State: " << arg.state << "]" << endl;
		return true;
	}
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		cout << "Mouse pressed: [Dev: " << arg.device->getID() << ", Button: " << id << "]" << endl;
		return true;
	}
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		cout << "Mouse released: [Dev: " << arg.device->getID() << ", Button: " << id << "]" << endl;
		return true;
	}
	// JoyStrickListener
	virtual bool buttonPressed(const OIS::JoyStickEvent &arg, int button)
	{
		cout << "Joystick pressed: [Dev: " << arg.device->getID() << ", Button: " << button << "]" << endl;
		return true;
	}
	virtual bool buttonReleased(const OIS::JoyStickEvent &arg, int button)
	{
		cout << "Joystick released: [Dev: " << arg.device->getID() << ", Button: " << button << "]" << endl;
		return true;
	}
	virtual bool axisMoved(const OIS::JoyStickEvent &arg, int axis)
	{
		cout << "Joystick moved: [Dev: " << arg.device->getID() << ", Axes: ";
		for (auto axis : arg.state.mAxes)
			cout << axis << " ";
		cout << endl;
		return true;
	}
	virtual bool sliderMoved(const OIS::JoyStickEvent &arg, int index)
	{
		cout << "Joystick Slider Moved [Dev: " << arg.device->getID() << ", Sliders: ";
		for (auto slider : arg.state.mSliders)
			cout << slider << " ";
		cout << "]" << endl;
		return true;
	}
	virtual bool povMoved(const OIS::JoyStickEvent &arg, int index)
	{
		cout << "Joystick povMoved [Dev: " << arg.device->getID() << ", Povs:";
		for (auto pov : arg.state.mPOV)
			cout << pov << " ";
		cout << "]" << endl;
		return true;
	}
	virtual bool vector3Moved(const OIS::JoyStickEvent &arg, int index)
	{
		cout << "Joystick vector3 moved [Dev: " << arg.device->getID() << "]" << endl;
		return true;
	}
};

//
class DumpState: public app::State
{
public:
	DumpState(int i) :
			mI(i), i(0)
	{
		cout << "State #" << mI << " created" << endl;
	}
	~DumpState()
	{
		cout << "State #" << mI << " destroyed" << endl;
	}
	void onActivate()
	{
		cout << "State #" << mI << " activated" << endl;
	}
	void onDeactivate()
	{
		cout << "State #" << mI << " deactivated" << endl;
	}
	void onRun()
	{
		cout << "State #" << mI << " running #" << i << endl;
		if (mI < 3 && i == 3)
			pushState(app::State::Ptr(new DumpState(mI + 1)));
		else if (i >= 4)
			stop();
		++i;
	}
private:
	int mI, i;
};

int main(int argc, char** argv)
{
	// Init settings
	GameSettingsSingleton.init(argc, argv, "config/astrowar.ini", "data/astrowar.lang");
	// If 'Hello World' easter egg!
	if (GameSettingsSingleton.getGeneralSettings().isHelloWorld())
		cout << GameSettingsSingleton.getLanguage().textForCode("main.helloworld") << endl;
	else
	{
//		InputDumpListener dumpListener;
		{ // Init with Inithelper
			AstrOWarInitHelper astrowarInitHelper;
			OgreFrameworkSingleton.init(&astrowarInitHelper);
			OisFrameworkSingleton.init(&astrowarInitHelper);
			CeguiFrameworkSingleton.init(&astrowarInitHelper);
			OisOgreWindowListener::getSingleton().init();
			OisCeguiInputListener::getSingleton().init();
		}
		// Start application
//		app::MainSingleton.run(app::State::Ptr(new DumpState(0)));
		app::MainSingleton.run(app::State::Ptr(new AOWMainMenuState));
		// Destroy Frameworks
		OisOgreWindowListener::getSingleton().shutdown();
		OisCeguiInputListener::getSingleton().shutdown();
		CeguiFrameworkSingleton.shutdown();
		OisFrameworkSingleton.shutdown();
		OgreFrameworkSingleton.shutdown();
	}
	return 0;
}
