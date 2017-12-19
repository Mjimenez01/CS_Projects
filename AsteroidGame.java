/*
 * University of Central Florida
 * COP 3330 Spring 2016
 * Author: Michael Jimenez 
 */
package asteroidgame;

import blobzx.SandBox;
import blobzx.SandBoxMode;
import java.util.Random;
import blobzx.BlobGUI;

public class AsteroidGame implements BlobGUI{

    SandBox sand; // Sandbox Constructor 
    private static Random random = new Random(); // Creating random var 

    public AsteroidGame() { // Asteroid Field constructor used to access sandbox.
        sand = new SandBox();
        sand.setSandBoxMode(SandBoxMode.FLOW);
        sand.setFrameRate(15);
        sand.init(this);
    }
    public static void main(String[] args) {
        new AsteroidGame();
    }

    public void generate() {
        Rocket ship = new Rocket(300,300,0,sand); //Add rocket to Sandbox
        sand.addBlob(ship);
        
        
        System.out.println("Game is on easy mode if for loop counter says 10. Incrament counter to 20 for medium and 30 for hard");
        System.out.println("Mike's high score is 10,000");
        for(int i =0; i < 30; i++){ // Adds 10 Asteroids to SandBox
        
            int ranDelX = 0;
            while(ranDelX == 0){
            ranDelX = -3 + random.nextInt(7);
            }
            int ranDelY = 0;
            while(ranDelY == 0){
            ranDelY = -3 + random.nextInt(7);
            }
            
            double rot = .1;
            int b = random.nextInt(2);
            if(b == 0){
            rot = -rot;
            }
            Asteroid ast = new Asteroid (ranDelX, ranDelY,rot);
            sand.addBlob(ast);
            
        }    
    }
}