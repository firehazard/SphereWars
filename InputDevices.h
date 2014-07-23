#ifndef INPUTDEVICES_H_
#define INPUTDEVICES_H_

#define STEER_BUTTON 0
#define FIRE_BUTTON  1
#define CAM_BUTTON   2
#define PAUSE_BUTTON 9

	struct Keys {
		bool upArrow;
		bool downArrow;
		bool leftArrow;
		bool rightArrow;
		bool a;
		bool d;
		bool s;
		bool w;
		bool f;
		bool l_alt;
		bool r_alt;
		bool escape;
		int m_xRel;
		int m_yRel;
		int m_yZoom;
	};

	struct JoyStick {
		short xAxis;
		short yAxis;
		short numButtons;
		Uint8* buttons;
		bool xPressedLastTic;
		bool yPressedLastTic;
		bool buttonPressedLastTic;
	};
	
	struct Mouse {
		bool buttonIsDown;
		bool buttonNeedsHandling;
		short clickX;
		short clickY;
		short hoverX;
		short hoverY;
	};

#endif /*INPUTDEVICES_H_*/
