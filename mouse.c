#include "mouse.h"
#include "math.h"
#include "debug_frmwrk.h"
#include "uart.h"

#define r 10
int32_t x_move;
int32_t y_move;

void cb(uint8_t buttons, int8_t X, int8_t Theta) {
	//if there is a change in the Y value it is added to the integer y_move;
	if(Theta != 0) {
		_DBG_("The Mouse has moved Left/right by: ");
		_DBC(Theta);
		_DBG_("\n");
		_DBG_("Value of y_move is: ");
		_DBC(y_move);
		_DBG_("\n");
	}
	
	//If there is a change in the X value it is added to the integer x_move;
	if(X != 0) {
		_DBG_("The Mouse has moved Forward/Backward by: ");
		_DBC(X);
		_DBG_("\n");
		x_move += X;
		_DBG_("Value of x_move is: ");
		_DBC(x_move);
		_DBG_("\n");
	}

}

int spin(int y) {
	y = y*r;
	return y;
}

int change_in_y(int O) {
	y = r(1 - cos(O));
}

int change_in_x(int O) {
	x = r(sin(O));
}

void save_values() {
	{{Y}, {X}} = {{cos(Theta), -sin(Theta)}, {sin(Theta), cos(Theta)}}*{{x}, {y}};
}

void attach() {
	set_x_move(0);
	set_y_move(0);
	_DBG_("I'm attached, YAY!");
}

void detach() {
	_DBG_("I'm detached, BOO!");
	printToLCD();
}

int32_t get_x_move() {	
	//Returns the value of x_move
	return x_move;
}

int32_t get_y_move() {
	//Returns the value of y_move
	return y_move;
}

void set_x_move(int8_t x) {
	x_move = x;
}

void set_y_move(int8_t y) {
	y_move = y;
}

int distanceMoved(int x, int y) {
	//
	int d;
	d = ((x^2) + (y^2));
	d = sqrt(d);
	_DBG_("the Distance moved by the Polulu robot is: ");
	_DBC(d); _DBG_("");
	return d;
}

void printToLCD() {
	int x = get_x_move();
	int y = get_y_move();
 	int	distance = distanceMoved(x, y);
	print(x);
}
