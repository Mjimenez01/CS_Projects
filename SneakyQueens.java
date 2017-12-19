// Michael Jimenez NID: mi535599
// COP 3503C, Fall 2017
//Sneaky Queens Program # 1
//package sneakyqueens;

import java.io.*;
import java.util.*;
import java.util.ArrayList;

public class SneakyQueens {

        // returns true if none of the queens can attack one another. Otherwise, return false. 
    public static boolean allTheQueensAreSafe(ArrayList<String> coordinateStrings, int boardSize)
    {               
            int i=0, r=0, k=0, col=0 , row=0 , LeftDiagonalRow=0, LeftDiagonalCol=0;
            int RightDiagonalRow=0, RightDiagonalCol=0;
            int[] rowBitmask = new int[boardSize+1];           // Frequency array of rows.
            int[] colBitmask = new int[boardSize+1];           // Frequency array of cols.
            int[] leftorigin = new int[((boardSize-1)*2) + 1]; //Stores Diagonal values with respect to left origin.
            int[] rightorigin= new int[((boardSize-1)*2) + 1]; //Stores Diagonal vlaues with respect to right origin.
            
            // Checking to see that more than 1 Queen is present and that the chess board is of sufficient size.
            if(coordinateStrings.size() < 2 || boardSize < 2)
            return true;
            
            for(String x: coordinateStrings) // Iterates through given arraylist
            {
                col = 0;
                //Splits string into char and int parts. This was found on Stack Overflow!! Dont reinvent the wheel!
                String[] part = coordinateStrings.get(i).split("(?<=\\D)(?=\\d)");
                
                for(k=0;k < part[0].length();k++) // Calculates the col value using base 26
                    col += (Math.pow(26, (part[0].length()) - (k+1))) * (((part[0].charAt(k)) - '`'));
                    
                    row = Integer.parseInt(part[1]);
                    
                    LeftDiagonalRow = row - 1; // Left Origin Row
                    LeftDiagonalCol = col - 1; // Left Origin Col
                    
                    RightDiagonalRow = LeftDiagonalRow;                     // Right Origin Row
                    RightDiagonalCol = ((boardSize - 1) - LeftDiagonalCol); // Right Origin Col
                    
                // Checks that no other Queens are in the same left oriented diagonal ie m = -1 slope
                if(leftorigin[LeftDiagonalRow + LeftDiagonalCol] == 0) 
                    leftorigin[LeftDiagonalRow + LeftDiagonalCol]++;
                else 
                    return false;
                
                // Checks that no other Queens are in the same right oriented diagonal ie m = 1 slope 
                if(rightorigin[RightDiagonalRow + RightDiagonalCol] == 0)
                    rightorigin[RightDiagonalRow + RightDiagonalCol]++;
                else 
                    return false;

                // Checks for multiple queens in a row
                if(rowBitmask[Integer.parseInt(part[1])] == 0)
                        rowBitmask[Integer.parseInt(part[1])]++;
                else
                    return false;
                
                // Checks for multiple queens in a col
                if(colBitmask[col] == 0)
                    colBitmask[col]++;
                else 
                    return false;
                i++;
            }
        return true; //It it makes it here Queens cannot attack each other!
    }

    //Returns a double indicating how difficult you found this assignment on a scale of 1.0 (ridiculously easy) through 5.0 (insanely difficult).
    public static double difficultyRating() 
    {
        double Difficulty = 3.2;
        return Difficulty;
    }

    //Returns an estimate (greater than zero) of the number of hours you spent on this assignment.
    public static double hoursSpent()
    {
        double TimeSpent = 13.0;
        return TimeSpent;
    }    
}
