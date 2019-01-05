/*
 * University of Central Florida    
 * CAP 4630 - Fall 2018
 * Authors: <Michael Jimenez, Jake Knudson>
 */

import java.awt.Point;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import pacsim.BFSPath;
import pacsim.PacAction;
import pacsim.PacCell;
import pacsim.PacFace;
import pacsim.PacSim;
import pacsim.PacUtils;
import pacsim.PacmanCell;

//	Compilation notes:
// "javac -cp lib/PacSimLib.jar PacSimRNNA.java"                                         *** Use this to compile the project ***
// "java -cp .;lib/PacSimLib.jar PacSimRNNA tsp-tiny" <-(Whatever maze you want to use)  *** Use this to run the project     ***

public class PacSimRNNA implements PacAction
{
    public static ArrayList<Point> fullSolution;
    public static ArrayList<Point> fullSolutionCopy;
    public static int[][] ucsTable;
    public static List<Point> food;
    private int simTime;
    
    public class Move // This is our Move object. It will be used to keep track of our points and costs at each step throughout the path.   
	{
    	Point point;
    	int cost;
    	
    	public Move(Point point, int cost)
    	{
    		this.point = point;
    		this.cost = cost;
    	}
    }
    
    // This is our step by step solution path of moves. A solution path consists of multiple Moves. 
    public class CandidatePath implements Comparable<CandidatePath>{
    	private int totalCost = 0; // This is the sum of all of the move costs in an individual solution path. 
    	private ArrayList<Move> moves; // This is the data structure that we will use to store our solution paths.
    	private ArrayList<Point> foodLeft; // This is a ArrayList of all the points for food that has not been eaten yet
    	
    	public CandidatePath()
    	{
    		moves = new ArrayList<Move>();
    		foodLeft = new ArrayList<Point>(food);
    	}
    	
    	public CandidatePath(CandidatePath oldPath)    	
    	{
    		moves = new ArrayList<Move>(oldPath.getMoves());
    		foodLeft = new ArrayList<Point>(oldPath.getFoodLeft());
    		totalCost = oldPath.totalCost();
    	}
    	
    	public void addMove(Move move)
    	{
    		Move newMove = new Move(move.point, move.cost);
    		moves.add(newMove);
    		this.removeFood(move.point);
    		totalCost += move.cost;
    	}
    	
    	public void addMove(Point point, int cost) // Method overloading, giving the user two different ways to add moves to the solution path. 
    	{
    		Move move = new Move(point, cost);
    		moves.add(move);
    		this.removeFood(point);
    		totalCost += move.cost;
    	}
    	
    	public Move getMove(int index)
    	{
    		return moves.get(index);
    	}
    	
    	public int size()
    	{
    		return moves.size();
    	}
    	
    	public ArrayList<Move> getMoves()
    	{
    		return moves;
    	}
    	
    	public Move getLastMove()
    	{
    		return moves.get(moves.size() - 1);
    	}
    	
    	public int totalCost()
    	{
    		return totalCost;
    	}
    	
    	public ArrayList<Point> getFoodLeft()
    	{
    		return foodLeft;
    	}
    	
    	public boolean removeFood(Point point)
    	{
    		return null == foodLeft.remove(foodLeft.indexOf(point));
    	}
    	
    	public int compareTo(CandidatePath otherPath)
    	{	
    		return this.totalCost() - otherPath.totalCost();
    	}
    }
    
    public class SolutionList // This will hold the costs of all of our solution paths. 
    {
    	private ArrayList<CandidatePath> solutionList; // This is essentially an ArrayList of ArrayLists.
    	
    	public SolutionList()
    	{
    		solutionList = new ArrayList<CandidatePath>();
    	}
    	
    	public SolutionList(ArrayList<CandidatePath> costArray2)
    	{
			solutionList = new ArrayList<CandidatePath>(costArray2);
		}

		public void addPath(CandidatePath path)
    	{
    		solutionList.add(path);
    	}
    	
    	public CandidatePath getPath(int index)
    	{
    		return solutionList.get(index);
    	}
    	
    	public ArrayList<CandidatePath> getList()
    	{
    		return solutionList;
    	}
    	
    	public void sort()
    	{
    		Collections.sort(solutionList);
    	}

    	public int size()
    	{
    		return solutionList.size();
    	}
    }

    public PacSimRNNA(String fname) 
    {
        PacSim sim = new PacSim(fname);
        sim.init(this);
    }

    public static void main(String[] args)
    {
        System.out.println("TSP using Repetitive Nearest Neighbor Algorithm by Michael Jimenez and Jake Knudson:\n");
        System.out.println("Maze : " + args[0] + "\n");
        new PacSimRNNA(args[ 0 ]);    
    }	

    @Override
    public void init()
    {
       System.out.println("");
       if (fullSolution != null)
       {
    	   fullSolution.clear();
       }
       simTime = 0;
    }

   @Override
   public PacFace action( Object state )
   {
      PacCell[][] grid = (PacCell[][]) state;
      PacmanCell pc = PacUtils.findPacman(grid);
      int i, x;
      ArrayList<Move> closestFoodMoves;
     
      // Make sure Pac-Man is in this game
      if (pc == null) return null;
      
      // If this is the first pass-through
      if (fullSolution == null)
      {
    	  // Begin timer for path-planning
    	  long start = System.currentTimeMillis();
    	  
    	  // Print Adjacency Matrix and Food Array
    	  ucsTable = generateAndPrintCostTable(grid, pc);
    	  food = generateAndPrintFoodArray(grid);
     	  
     	  Point PacManPoint = new Point(pc.getX(), pc.getY()); // Here we obtain pac-man's location as a point.  
          
          SolutionList solutionList = new SolutionList();
          int solutionListSizeBeforeAddingSplits = 0;
          
          int foodSize = food.size();
          
          for(x = 0; x < foodSize; x++)
          {
         	 if (x == 0) // Start off with starting a path at each food point
         	 {
         		 for (i = 0; i < foodSize; i++)
         		 {
         			 CandidatePath candidatePath = new CandidatePath();
         			 candidatePath.addMove(food.get(i), BFSPath.getPath(grid, PacManPoint, food.get(i)).size());
         			 solutionList.addPath(new CandidatePath(candidatePath));
         		 }
         	 }
         	 else
         	 {
         		 // We need to make sure we don't add another move to the splits we might add
         		 solutionListSizeBeforeAddingSplits = solutionList.size();
         		 
         		 for (i = 0; i < solutionListSizeBeforeAddingSplits; i++)
         		 {
         			 closestFoodMoves = findClosestFood(solutionList.getPath(i).getLastMove().point, solutionList.getPath(i).getFoodLeft());
         			 
         			 for (int j = 1; j < closestFoodMoves.size(); j++)
         			 {
         				 CandidatePath splitPath = new CandidatePath(solutionList.getPath(i));
         				 splitPath.addMove(closestFoodMoves.get(j));
         				 solutionList.addPath(splitPath);
         			 }
         			 
         			 solutionList.getPath(i).addMove(closestFoodMoves.get(0));
         		 }
         	 }
         	 
         	 // Sorts the candidatePaths in ascending cost order
         	 solutionList.sort();
         	 printPopulation(solutionList, x + 1);
          }
          
          long end = System.currentTimeMillis(); // Output the time it took to calculate the moves in milliseconds. 
     	  System.out.println("Time to generate plan: " + (end - start) + " msec\n");
     	 
     	  // We need to get the path to every point 
 		  PacCell[][] mazeCopy = grid.clone();
 		  ArrayList<Move> shortestSolution = solutionList.getPath(0).getMoves();
     	 
     	  // Finally, we need to call a function to give us our absoluteMazeSolution (BFSPath distances everywhere).
     	  fullSolution = FinalRnnaPath(pc.getLoc(), shortestSolution, mazeCopy);
     	  fullSolutionCopy = new ArrayList<Point>(fullSolution);
     	  
     	 System.out.println("Solution moves:\n");
      }
      
      if (fullSolution.size() < 1)
      {
    	  fullSolution = new ArrayList<Point>(fullSolutionCopy);
      }
      
      // Before being able to solve the maze, we have to compute the BFSPath from PacMan to each food dot.
      // take the next step on the current path  
      
      Point next = fullSolution.remove(0);
      PacFace face = PacUtils.direction( pc.getLoc(), next );
      
      System.out.printf( "%5d : From [ %2d, %2d ] go %s%n", ++simTime, pc.getLoc().x, pc.getLoc().y, face );
      return face;
   }
   
   public ArrayList<Point> FinalRnnaPath(Point PacManPoint, ArrayList<Move> shortSolution,PacCell[][] maze)
   {
	   ArrayList<Point> BFSpoints = new ArrayList<Point>();
	   List<Point> temp;
	   
	   // Complete the BFSPath's from pacman to every point in our shortSolution.
	   for(int i = 0; i < shortSolution.size(); i++)
	   {		   
		   // Get the ArrayList of the the moves in the set so that we can iterate over them.   
		    temp = BFSPath.getPath(maze, PacManPoint,shortSolution.get(i).point);

		    // Place each point in our BFS path in our point array.
		    for(int j = 0; j < temp.size(); j++)
		    	BFSpoints.add(temp.get(j));
		    
		   // Place pac-man's position at the new food dot.
		   PacManPoint = shortSolution.get(i).point;
	   }	   
	   return BFSpoints; // Return Full Solution.
   }
   
   // Returns a list of the closest food points
   public ArrayList<Move> findClosestFood(Point PacManLocation, ArrayList<Point> remainingFood)
   {
	   ArrayList<Move> closestFoodPoints = new ArrayList<Move>();
	   int minCost = Integer.MAX_VALUE;
	   int cost;
	   
	   // Find the index of the current food point that pacman is on
	   int pacmanIndex = food.indexOf(PacManLocation) + 1;
	   
	   for (int i = 0; i < remainingFood.size(); i++)
	   {
		   // calculate cost of pacman to current remaining food point
		   cost = ucsTable[pacmanIndex][food.indexOf(remainingFood.get(i)) + 1];
		   
		   // we need to make sure cost isn't 0 to account for the distance to a vertex and itself
		   if (cost > 0 && cost < minCost)
		   {
			   closestFoodPoints.clear();
			   closestFoodPoints.add(new Move(remainingFood.get(i), cost));
			   minCost = cost;
		   }
		   else if (cost > 0 && cost == minCost)
		   {
			   closestFoodPoints.add(new Move(remainingFood.get(i), cost));
		   }
	   }
	   
	   return closestFoodPoints;
   }
   
   public List<Point> generateAndPrintFoodArray(PacCell[][] maze)
   {
	   List<Point> food = PacUtils.findFood(maze); // Here we create a ArrayList containing the [x,y] points where the food dots can be found. 
	   int n = food.size(), i; // The integer n stores the amount of food dots in our current Maze.
	   
	   System.out.println("\n\nFood Array:\n");  //This is where we print our "Food Array".    

		for(i = 0; i < n; i++)
			System.out.println(i + " : (" + (int)food.get(i).getX() + "," + (int)food.get(i).getY() + ")");

		System.out.println("\n");
		
		return food;
   }
   
   public int[][] generateAndPrintCostTable(PacCell[][] maze, PacmanCell currentCell) // Method that creates and fills our Cost Table, then prints and returns it.
   {
	   List<Point> food = PacUtils.findFood(maze);
	   int n = food.size();
	   
	   int [][] costMatrix = new int[n + 1][n + 1]; // This is where we create our cost matrix.
	   int distance, i;
    
	   for(i = 0; i < n; i++)
	   {
		   // Returns a list of points to our food dots. The length of that list is the distance from pacman's current position.     	
		   distance = BFSPath.getPath(maze, currentCell.getLoc(), food.get(i)).size();        	
		   costMatrix[0][i+1] = distance;
		   costMatrix[i+1][0] = distance; // Since this is a symmetric Maze, fill in both spots at the same time. 
	   }
	   
	   // Thus far we have only filled in our Cost Table for the outer row and column. We still need to fill in the inner cells.
	   int innerDistance, y;
	        
	   for(i = 1; i <= n; i++)
	   {
		   for(y = 1; y <= n; y++)
		   {	// We need the BFSPath distance between the two food dots.
	    		innerDistance = BFSPath.getPath(maze, food.get(i-1), food.get(y-1)).size(); // We want the BFS distance between the 2 food dots.
	    		costMatrix[i][y] = innerDistance;
		   }
	   }   
	    
	   // This is where we print our Cost Table:
	   System.out.println("Cost Table:\n");

	   for(i = 0; i <= n; i++)
	   {
		   for(y = 0; y <= n; y++)
			   System.out.printf("%4d", costMatrix[i][y]);
		
		   System.out.println("");
	   }
	    
	   return costMatrix;
   }
   
   public void printPopulation(SolutionList solutionList, int step)
   {
	   System.out.println("Population at step " + step + " :");
	   
	   for (int i = 0; i < solutionList.size(); i++)
	   {
		   System.out.print("   " + i + " : cost=" + solutionList.getPath(i).totalCost() + " : ");
		   
		   for (int j = 0; j < solutionList.getPath(i).size(); j++)
		   {
			   System.out.print("[(" + solutionList.getPath(i).getMove(j).point.x + "," + solutionList.getPath(i).getMove(j).point.y + ")" + "," + solutionList.getPath(i).getMove(j).cost + "]");
		   }
		   
		   System.out.println("");
	   }
	   
	   System.out.println("");
   }
}