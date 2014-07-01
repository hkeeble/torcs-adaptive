package hk.trackplotter;

import java.util.ArrayList;

public class Subject {

	ArrayList<Observer> observers;
	
	public Subject() {
		observers = new ArrayList<Observer>();
	}
	
	public void register(Observer observer) {
		observers.add(observer);
	}

	public void unregister(Observer observer) {
		observers.remove(observer);
	}
	
	public void notifyObservers(GUIMessage message) {
		for(Observer o : observers) {
			o.notify(message);
		}
	}
}
