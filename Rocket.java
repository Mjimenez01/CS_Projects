/*
 * University of Central Florida
 * COP 3330 Spring 2016
 * Author: Michael Jimenez 
 */
package asteroidgame;

import blobzx.BlobAction;
import blobzx.PolyBlob;
import blobzx.BlobUtils;
import blobzx.BlobGUI;
import blobzx.BlobProximity;
import blobzx.SandBox;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.lang.Math;

public class Rocket extends PolyBlob implements BlobAction, BlobProximity {
    
    private double angle = 0.0;          // direction of motion
    private final double delta = 0.30;  // angular change per key press originally .15
    private final double speed = 8.0;  // speed in direction of motion originally 5.0
    
    private final int[] x = {10,-10,-5,-10};//motion set Passed to setPolygon
    private final int[] y = {0,-7,0,7};
    
    private final int[] Rx = {10,-10,-5,-10};//Reference Set 
    private final int[] Ry = {0,-7,0,7};
    
            SandBox sand; //Sandbox object used with missile 
    
            int xloc = 300; //set x location to starting position
            int yloc = 300;//set y location to starting position
           
    public Rocket(int x, int y, double r,SandBox s) { //Rocket constructor.
        super(x,y,r);
        this.sand = s;
        super.getLoc();
        super.setPolygon(this.x,this.y); //Passed motion set of polygon arrays
    }

    public interface BlobAction{
    
        void keyAction(KeyEvent e);
    }
    
    public void keyAction(KeyEvent e) {
    
        double TwoPi = (Math.PI * 2); 
        
      if(e.getKeyCode()==37)//left arrow
      {
          angle = angle - delta;
          
          if(angle < 0)
          {
              angle = angle + TwoPi;
          }
          turn();
      }
      else if(e.getKeyCode()==38)//up arrow
      {
            xloc = xloc + (int) Math.round(speed * Math.cos(angle));
            yloc = yloc + (int) Math.round(speed * Math.sin(angle));
            super.setLoc(xloc, yloc);
      }
      else if(e.getKeyCode()==39)//right arrow
      {
          angle = angle + delta;
          
          if(angle > TwoPi)
          { 
              angle = angle - TwoPi;
          }
          turn();
      }
      
      else if(e.getKeyCode()==32)//spacebar 
      {
          launch(sand); //Calls launch method and sends it sandbox obj.
      }
    }
    
    public void turn(){
    
        Point p = new Point();
        
    for(int i=0;i<Rx.length;i++)
        {
            p = BlobUtils.rotatePoint(Rx[i], Ry[i], angle); //Rotates vertices 
            x[i] = p.x;
            y[i] = p.y;
        }
    }
    
    public void launch(SandBox sb){
            BlobUtils.playSound(); // Makes sounds when you shoot / press spacebar.

            int BoundingRadius = super.getSize()/2 + 5; //Bonding radius 5 pixels infron of rocket
                      
            // Current direction is angle 
            // Current location is under getLoc().
            int getX = getLoc().x; //Contains current X coord
            int getY = getLoc().y; //Contains current Y coord
            
             Point gunLocation = BlobUtils.rotatePoint(BoundingRadius, angle); // Rotates point to desired location
            
            Missile gun = new Missile(xloc + gunLocation.x,yloc + gunLocation.y,angle); 
            // Sends point which is 5 pixels in front of rocket to missile class ^^^ 
            sand.addBlob(gun); // Adds guns to sandbox
    }
    
}
