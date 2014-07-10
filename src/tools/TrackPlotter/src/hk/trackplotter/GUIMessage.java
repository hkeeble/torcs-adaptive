package hk.trackplotter;

/**
 * Represents GUI messages that can be passed from subjects to the mainframe observer upon notificaiton.
 * @author Henri
 *
 */
public enum GUIMessage {
	PLOT_DISTANCE_DIFF,
	PLOT_SPEED_DIFF,
	TOGGLE_POINT_RENDER,
	TOGGLE_SPEED_RENDER,
	CHANGE_SPD_RENDER,
	CHANGE_SPEED_RES,
	EXIT,
	LOAD_NEW_DATA
}