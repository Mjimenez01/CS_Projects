/*
 * University of Central Florida
 * COP 3330 Spring 2016
 * Author: Michael Jimenez 
 */
package asteroidgame;

import blobzx.Blob;
import blobzx.BlobProximity;

public class Missile extends Blob implements BlobProximity {

double speed = 10.0;  //Was 5.0 originally. 
   
    public Missile(int x, int y,double theta) { //Missile Constructor
        super(x,y,5); // Call to Blob
    
         int dx = (int) Math.round(speed * Math.cos(theta)); //X component of missile velocity
         int dy = (int) Math.round(speed * Math.sin(theta)); //Y component of missile velocity
         super.setDelta(dx,dy); // Sends velocity to super class. 
    }
}
