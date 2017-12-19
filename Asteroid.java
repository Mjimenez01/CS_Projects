/*
 * University of Central Florida
 * COP 3330 Spring 2016
 * Author: Michael Jimenez 
 */
package asteroidgame;

import java.awt.Point;
import java.lang.Math;
import blobzx.BlobUtils;
import blobzx.PolyBlob;
import java.util.Random; 

public class Asteroid extends PolyBlob{ //Asteroid Extends PolyBlob(INHERITENCE).
    private static Random random = new Random(); 
    
    public Asteroid(int idx,int jdx, double rot)//Will come from asteroid field class
    {    
    super(-100,-100,rot);
    setDelta(idx,jdx);
    
    int numSides = 5 + random.nextInt(5);
    
    int[] vertex = new int[numSides];
    for(int i =0; i < numSides; i++){
        vertex[i] = 5 + random.nextInt(11);
    }
    
    double region = (2 * Math.PI)/numSides;
    double[] angle = new double[numSides];
    for(int i =0; i < numSides; i++){
    angle[i] = (i * region) + (Math.random() * region);
    }
    
    int[]x = new int [numSides];
    int[]y = new int[numSides];
    for(int i=0; i < numSides; i++){
    Point p = BlobUtils.rotatePoint(vertex[i], angle[i]);
    x[i] = p.x;
    y[i] = p.y;
    }
    setPolygon(x,y);
    }
}