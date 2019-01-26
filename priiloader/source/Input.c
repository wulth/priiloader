/*

priiloader - A tool which allows to change the default boot up sequence on the Wii console

Copyright (C) 2008-2017 DacoTaco

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


*/

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <sys/time.h>

#include "Input.h"
#include "gecko.h"

s8 _input_init = 0;
u32 _STM_input_pressed = 0;
struct timeval start;

void HandleSTMEvent(u32 event)
{
	struct timeval stop;
	gettimeofday(&stop, NULL);	

	//only accept input every +/- 100ms
	//this stops it from detecting one press as multiple fires
	if(stop.tv_sec == start.tv_sec && (stop.tv_usec - start.tv_usec < 100000))
		return;

	gettimeofday(&start, NULL);
	switch(event)
	{
		case STM_EVENT_POWER:
			_STM_input_pressed = INPUT_BUTTON_DOWN;
			break;
		case STM_EVENT_RESET:
			_STM_input_pressed = INPUT_BUTTON_A;
			break;
		default:
			_STM_input_pressed = 0;
			break;
	}
	return;
}

s8 Input_Init( void ) 
{
	if(_input_init != 0)
		return 1;

	s8 r;
	r = PAD_Init();
	r |= WPAD_Init();

	STM_RegisterEventHandler(HandleSTMEvent);

	_input_init = 1;
	return r;
}
void Input_Shutdown( void )
{
	WPAD_Shutdown();
	_input_init = 0;
}
u32 Input_ScanPads( void )
{
	WPAD_ScanPads();
	PAD_ScanPads();
	return 1;
}
u32 Input_ButtonsDown( void )
{
	u32 pressed = 0;
	if(_STM_input_pressed != 0)
	{
		pressed = _STM_input_pressed;
		_STM_input_pressed = 0;
	}
	else
	{
		pressed = 0;
		pressed |= (PAD_ButtonsDown(0) | PAD_ButtonsDown(1) | PAD_ButtonsDown(2) | PAD_ButtonsDown(3));
		
		u32 WPAD_Pressed = (WPAD_ButtonsDown(0) | WPAD_ButtonsDown(1) | WPAD_ButtonsDown(2) | WPAD_ButtonsDown(3));
		
		if( WPAD_Pressed & WPAD_BUTTON_DOWN || WPAD_Pressed & WPAD_CLASSIC_BUTTON_DOWN )
			pressed |= INPUT_BUTTON_DOWN;
		if( WPAD_Pressed & WPAD_BUTTON_UP || WPAD_Pressed & WPAD_CLASSIC_BUTTON_UP )
			pressed |= INPUT_BUTTON_UP;
		if( WPAD_Pressed & WPAD_BUTTON_LEFT || WPAD_Pressed & WPAD_CLASSIC_BUTTON_LEFT )
			pressed |= INPUT_BUTTON_LEFT;
		if( WPAD_Pressed & WPAD_BUTTON_RIGHT || WPAD_Pressed & WPAD_CLASSIC_BUTTON_RIGHT )
			pressed |= INPUT_BUTTON_RIGHT;
		if(WPAD_Pressed & WPAD_BUTTON_A || WPAD_Pressed & WPAD_CLASSIC_BUTTON_A)
			pressed |= INPUT_BUTTON_A;
		if(WPAD_Pressed & WPAD_BUTTON_B || WPAD_Pressed & WPAD_CLASSIC_BUTTON_B)
			pressed |= INPUT_BUTTON_B;
		if(WPAD_Pressed & WPAD_BUTTON_PLUS || WPAD_Pressed & WPAD_CLASSIC_BUTTON_PLUS)
			pressed |= INPUT_BUTTON_START;
		if(WPAD_Pressed & WPAD_BUTTON_1 || WPAD_Pressed & WPAD_CLASSIC_BUTTON_1)
			pressed |= INPUT_BUTTON_Y;
		if(WPAD_Pressed & WPAD_BUTTON_2 || WPAD_Pressed & WPAD_CLASSIC_BUTTON_2)
			pressed |= INPUT_BUTTON_X;
	}

	return pressed;
}