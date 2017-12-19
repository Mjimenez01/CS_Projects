//Michael Jimenez NID: mi535599
//Program assginment #4 SkipList

import java.io.*;
import java.util.*;
import java.util.ArrayList;
import java.util.ArrayDeque;
import java.lang.*;

class Node<T>  //This is our Generic Structure that holds Generic Data and a next "pointer". 
{
	T data;
        int height;
        ArrayList<Node<T>> pointers; //This is an arraylist of pointers to generic nodes!
        
        //This constructor creates a new node with the specified height, which will be greater than zero. 
        //Useful for the head node in general.
	Node(int height) 
        {
           this.height = height; 
           pointers = new ArrayList<Node<T>>(height);
           
           for(int i = 0; i < (height); i++) //Set all of the nodes next pointers to null. 
                pointers.add(i,null);           
        }
        //This constructor creates a new node with the specified height, which will be greater than zero,
        //and initializes the node’s value to data. 
        Node(T data, int height)
        {
            this.data = data;
            this.height = height;
            pointers = new ArrayList<Node<T>>(height);
            
           for(int i = 0; i < height; i++) //Set all of the nodes next pointers to point to null. 
               pointers.add(i, null);
        }
        
        //An O(1) method that returns the value stored at this node.
        public T value()
        {
            return data;
        }
        
        //An O(1) method that returns the height of this node.
        public int height()
        {
            return height;
        }
        
        //An O(1) method that returns a reference to the next node in the skip list at this particular level. 
        public Node<T> next(int level)
        {
            if(level >= height || level < 0) //If trying to access area out of bounds. 
                return null;
            
            return pointers.get(level);
        }
}

public class SkipList<T extends Comparable<T>>
{   
    private Node<T> head;       //The head of our awesome skip list. 
    private int numNodes = 0;   //Number of nodes in our skip list.
    
    HashSet<T> containsData = new HashSet<>();
    
    //This constructor creates a new skip list.
    SkipList()
    {
        head = new Node<T>(1); //Create a new head of our skip list and give it height 1.
    }        
    
    //This constructor creates a new skip list and initializes the head node to have the height specified
    //by the height parameter.
    SkipList(int height) 
    {
        if(height <= 0)
            height = 1;
        
            if(head == null)
               head = new Node<T>(height); //Creating the head of our skiplist.
    }
        
    //In O(1) time, return the number of nodes in the skip list (excluding the head node, since it does
    //not contain a value).
    public int size()
    {    
        return numNodes;
    }
    
    //In O(1) time, return the current height of the skip list, which is also the height of the head node. 
    public int height()
    {
       return head.height; //Should return log base 2 of n or the height of our head node. 
    }
    
    //Return the head of the skip list.
    public Node<T> head()
    {
        return head; 
    }
    
    //Insert data into the skip list with an expected (average-case) runtime of O(log n). If the value
    //being inserted already appears in the skip list, this new copy should be inserted before the first
    //occurrence of that value in the skip list. 
    public void insert(T data)
    {
        int height = 1; //This will be the randomly generated default height of our newly inserted node. 
        
        Node<T> tmp = head; //The traversal node ptr that we follow through the list. 
        
        Node<T> Growtmp = head; //The traversal node ptr that we use to traverse only the max nodes if we need to grow. 
                
        while(Math.random() < 0.5) //This is simulating our coin flip which determines the height of our node. 
            height++;
                       
        if(height > tmp.height) //If our randomly generated node is too tall, scale it down. 
            height = tmp.height;
       
        ArrayDeque<Node<T>> droplvl = new ArrayDeque<>(); // Stores all the nodes that we drop at.

        Node<T> node = new Node<T>(data,height); //Create the new node to be inserted!            
        
          for(int i=tmp.height - 1; i >= 0; i--) //Loop through the heights of the head node.  
            {
                //If the pointed to nodes data is greater than the newly created node or were pointing to null
                //drop down a level of height and repeat search process.
                //Checks to see if inserted node is less than current node
                if(tmp.pointers.get(i) == null || tmp.pointers.get(i).data.compareTo(node.data) > 0 || tmp.pointers.get(i).data.compareTo(node.data) == 0) 
                {
                    droplvl.push(tmp); //Store our drop nodes into a stack.
                   continue; //This is essentially how we are dropping levels. 
                }
                             
                //If the pointed to nodes data is less than the inserting node move to that node. 
                if(tmp.pointers.get(i).data.compareTo(node.data) < 0) 
                {
                    if(tmp.next(i) != null)
                    tmp = tmp.next(i);
                    
                    if(tmp.pointers.get(i) != null)
                    {                        
                        while(tmp.pointers.get(i) != null && tmp.pointers.get(i).data.compareTo(node.data) < 0)
                        {
                            if(tmp.next(i) != null)
                            tmp = tmp.next(i);
                            
                            else 
                                break;
                        }
                    }
                    
                    droplvl.push(tmp); //Store the drop   
                    continue; //Continue down our list
                }
            }

          //Once here our temp has traversed the list and is currently where our node needs to be inserted.           
          //We now need to pop the stack node height number of times. 
          
          for(int z = 0; z < node.height; z++)
          {
              node.pointers.set(z , droplvl.peek().pointers.get(z));
              droplvl.pop().pointers.set(z , node);
          }
       
          //Once here our node has been inserted. 
                    
          numNodes++; //Increase our node counter. 
          containsData.add(node.data); //Stores all the inserted <T>Data. 
                    
          double logHeight = Math.log(numNodes)/Math.log(2);
          logHeight = Math.ceil(logHeight); //Takes the ceiling of log base 2 of n ie rounds up. 
          int NewHeight = (int)logHeight;   //The new height of our skiplist after inserting 
                    
          if(NewHeight <= 0) // If our list contains 1 element it should be of height 1 not 0; 
              NewHeight = 1;
          
          if(NewHeight > Growtmp.height) //If this is the case we need to grow our skiplist!
          {
              //We need to traverse the prior max height and flip a coin on each node to see if it will grow!
              
              int p = NewHeight - 2; //Variable used to traverse previous max level
              
              ArrayList<Node<T>> NodesThatGrow = new ArrayList<>();

              Growtmp = head;
              
              Growtmp = Growtmp.next(p);// Point Growtmp to the first node of maxheight 
              
              while(Growtmp != null) //Determines if a node of maxheight will grow or not.  
              {      
                  if(Growtmp != null && Math.random() < 0.5) //Flip a coin for each node.
                  {
                      NodesThatGrow.add(Growtmp);
                      Growtmp.height = NewHeight;
                  }
                  Growtmp = Growtmp.next(p);
              }
             
              Growtmp = head;
              
              head.height = NewHeight; //We need to update the height of our head node. 
                                
              Growtmp.pointers.add(p+1,null); //Growing the head node by height 1.

              for(int b = 0; b < NodesThatGrow.size(); b++)
              {
                  NodesThatGrow.get(b).pointers.add(p+1,null); //Grow the height of the nodes on our maxlevel.
                  Growtmp.height = NewHeight; //Update the heights of our grown nodes!!!!!
                  Growtmp.pointers.set(p+1,NodesThatGrow.get(b));
                  Growtmp = NodesThatGrow.get(b);
              }
          }
    }
    
    //Insert data into the skip list using the procedure described above, but do not generate a random
    //height for the new node. Instead, set the node’s height to the value passed in via this method’s
    //height parameter.
    public void insert(T data, int height)
    {       
        Node<T> tmp = head; //The traversal node ptr that we follow through the list. 
        
        Node<T> Growtmp = head; //The traversal node ptr that we use to traverse only the max nodes if we need to grow. 
          
        ArrayDeque<Node<T>> droplvl = new ArrayDeque<>();
        
        Node<T> node = new Node<T>(data,height); //Create the new node to be inserted!            
        
          for(int i=tmp.height - 1; i >= 0; i--) //Loop through the heights of the head node.  
            {
                //If the pointed to nodes data is greater than the newly created node or were pointing to null
                //drop down a level of height and repeat search process.
                //Checks to see if inserted node is less than current node
                if(tmp.pointers.get(i) == null || tmp.pointers.get(i).data.compareTo(node.data) > 0 || tmp.pointers.get(i).data.compareTo(node.data) == 0) 
                {
                    droplvl.push(tmp); //Store our drop nodes into a stack.
                   continue; //This is essentially how we are dropping levels. 
                }
                             
                //If the pointed to nodes data is less than the inserting node move to that node. 
                if(tmp.pointers.get(i).data.compareTo(node.data) < 0) 
                {
                    tmp = tmp.next(i);
                    
                    if(tmp.pointers.get(i) != null)
                    {
                        while(tmp.pointers.get(i) != null && tmp.pointers.get(i).data.compareTo(node.data) < 0)
                        tmp = tmp.next(i);
                    }
                    
                    droplvl.push(tmp); //Store the drop   
                    continue; //Continue down our list
                }
            }

          //Once here our temp has traversed the list and is currently where our node needs to be inserted. 
          
          //We now need to pop the stack node height number of times. 
          
          for(int z = 0; z < node.height; z++)
          {
              node.pointers.set(z , droplvl.peek().pointers.get(z));
              droplvl.pop().pointers.set(z , node);
          }
    
          //Once here our node has been inserted. 
                    
          numNodes++; //Increase our node counter. 
          containsData.add(node.data); //Stores all the inserted <T>Data. 
                    
          double logHeight = Math.log(numNodes)/Math.log(2);
          logHeight = Math.ceil(logHeight); //Takes the ceiling of log base 2 of n ie rounds up. 
          int NewHeight = (int)logHeight;   //The new height of our skiplist after inserting 
                    
          if(NewHeight <= 0) // If our list contains 1 element it should be of height 1 not 0; 
              NewHeight = 1;
          
          if(NewHeight > Growtmp.height) //If this is the case we need to grow our skiplist!
          {
              //We need to traverse the prior max height and flip a coin on each node to see if it will grow!
              
              int p = NewHeight - 2; //Variable used to traverse previous max level
              
              ArrayList<Node<T>> NodesThatGrow = new ArrayList<>();

              Growtmp = head;
              
              
                  Growtmp = Growtmp.next(p);// Point Growtmp to the first node of maxheight 
              
              while(Growtmp != null) //Determines if a node of maxheight will grow or not.  
              {      
                  if(Growtmp != null && Math.random() < 0.5) //Flip a coin for each node.
                  {
                      NodesThatGrow.add(Growtmp);
                      Growtmp.height = NewHeight;
                  }
                  Growtmp = Growtmp.next(p);
              }
             
              Growtmp = head;
              
              head.height = NewHeight; //We need to update the height of our head node. 
                                
              Growtmp.pointers.add(p+1,null); //Growing the head node by height 1.

              for(int b = 0; b < NodesThatGrow.size(); b++)
              {
                  NodesThatGrow.get(b).pointers.add(p+1,null); //Grow the height of the nodes on our maxlevel.
                  Growtmp.height = NewHeight; //Update the heights of our grown nodes!!!!!
                  Growtmp.pointers.set(p+1,NodesThatGrow.get(b));
                  Growtmp = NodesThatGrow.get(b);
              }
          }      
    }
    
    public void print()
    {
        int curr_level = head.height();
        for(int i = curr_level - 1; i >= 0; i--) {
            Node<T> curr = head;
            while(curr != null) {
                System.out.print(((curr.value() == null) ? "null" : curr.value()) + " -> ");
                curr = curr.next(i);
            }
            System.out.println();
        }
        System.out.println("----");
    }

    //Delete a single occurrence of data from the skip list (if it is present). If there are multiple copies
    //of data in the skip list, delete the first node (that is, the leftmost node) that contains data
    public void delete(T data)
    {        
        Node<T> tmp = head; //The traversal node ptr that we follow through the list. 
        Node<T> Growtmp = head;
        
        ArrayDeque<Node<T>> droplvl = new ArrayDeque<>(); // Stores all the nodes that we drop at.

        Node<T> node = new Node<T>(data,1); //Create the new node used to traverse the list with dummy height 1.            
        
          for(int i=tmp.height - 1; i >= 0; i--) //Loop through the heights of the head node.  
            {
                //If the pointed to nodes data is greater than the newly created node or were pointing to null
                //drop down a level of height and repeat search process.
                //Checks to see if inserted node is less than current node
                if(tmp.pointers.get(i) == null || tmp.pointers.get(i).data.compareTo(node.data) > 0 || tmp.pointers.get(i).data.compareTo(node.data) == 0) 
                {
                    droplvl.push(tmp); //Store our drop nodes into a stack.
                   continue; //This is essentially how we are dropping levels. 
                }
                             
                //If the pointed to nodes data is less than the inserting node move to that node. 
                if(tmp.pointers.get(i).data.compareTo(node.data) < 0) 
                {
                    if(tmp.next(i) != null)
                    tmp = tmp.next(i);
                    
                    if(tmp.pointers.get(i) != null)
                    {                        
                        while(tmp.pointers.get(i) != null && tmp.pointers.get(i).data.compareTo(node.data) < 0)
                        {
                            if(tmp.next(i) != null)
                            tmp = tmp.next(i);
                            
                            else 
                                break;
                        }
                    }
                    
                    droplvl.push(tmp); //Store the drop   
                    continue; //Continue down our list
                }
            }

          //Once here our temp has traversed the list and is currently where our node needs to be inserted. 
          //We now need to pop the stack node height number of times. 
          
          Node<T> Delptr = tmp.next(0); //Points to the node to be deleted. 
         
          if(Delptr != null && Delptr.data.compareTo(data) == 0) //Checks that Delptr is not null and that it is in our list. 
          if(containsData.contains(Delptr.data) == true) //Checks to see that the element to be deleted is actually in our list. 
          {
              
          
          for(int z = 0; z < Delptr.height; z++) //This is where we delete our node. 
          {
              droplvl.pop().pointers.set(z, Delptr.pointers.get(z));
              Delptr.pointers.set(z, null);
          }
          Delptr.height = 0; //The deleted node should have no height.
          
          //Once here our desired node has been deleted. 
          
        numNodes--;
        containsData.remove(Delptr.data); //Stores all the inserted <T>Data. 
        
        //Now we need to check if our list must be trimmed.
        
          double logHeight = Math.log(numNodes)/Math.log(2);
          logHeight = Math.ceil(logHeight); //Takes the ceiling of log base 2 of n ie rounds up. 
          int NewHeight = (int)logHeight;   //The new height of our skiplist after inserting 
                    
          if(NewHeight <= 0) // If our list contains 1 element it should be of height 1 not 0; 
              NewHeight = 1;
          
          if(NewHeight < Growtmp.height) //If this is the case we need to trim our skiplist!
          {  
              ArrayList<Node<T>> NodesThatTrim = new ArrayList<>(); //This will hold the nodes that will shrink. 
   
              int p = NewHeight; //Variable which we will use to traverse the maxheight of our list. 
              
              Growtmp = head;
                            
                  Growtmp = Growtmp.next(p);// Point Growtmp to the first node of maxheight 
              
              while(Growtmp != null)
              {      
                      NodesThatTrim.add(Growtmp);
                      Growtmp = Growtmp.next(p);
              }
              
              Growtmp = head;
              
              Growtmp.pointers.add(p,null); //Setting our heads pervious maxheight to null. 
              
              head.height = NewHeight; //We need to update the height of our head node. 
                                
              for(int j=0; j < NodesThatTrim.size();j++)
              {
                  NodesThatTrim.get(j).pointers.set(p, null);
                  NodesThatTrim.get(j).height = NewHeight; //Setting the heights of our nodes after being trimmed. 
              }
          }
          }
    }
    
    //Return true if the skip list contains data. Otherwise, return false. 
    public boolean contains(T data)
    {
        if(containsData.contains(data) == true) //Checking my hashset of data to see if our data has been inserted!
            return true;
        
        return false;//If we didnt find the node within our list, return false. 
    }
    
    //Return a reference to a node in the skip list that contains data. If no such node exists, return
    //null. If multiple such nodes exist, return the first such node that would be found by a properly
    //implemented contains() method.
    public Node<T> get(T data)
    {
        Node<T> CN = head();
        Node<T> RN;                 //Return node
        int p = head().height() -1; //Top of list varable. 
    
        while(p >= 0) //This is the traversal through our list. 
        {
            if(CN.next(p) == null) 
            {
                    p--;
                continue;
            }
            if(CN.next(p).data.compareTo(data) == 0) //Compares our node data to see if its equal. 
            {
                RN = CN.next(p);
                while(p > 0)
                {
                    p--;
                    if(CN.next(p).data.compareTo(data) == 0)
                        RN = CN.next(p);
                }
                return RN; //If found return our node. 
            }
            else if(CN.next(p).data.compareTo(data) < 0)
                CN = CN.next(p);
            
                p--;
            continue;
        }
        return null; //If we make is here the data is not in the list. 
    }
    
    //Return a double on the range 1.0 (ridiculously easy) through 5.0 (insanely difficult).
    public static double difficultyRating()
    {
        double howhard = 4.0;
        return howhard;
    }
    
    //Return an estimate (greater than zero) of the number of hours you spent on this assignment.
    public static double hoursSpent()
    {
        double howlong = 24.0;
        return howlong;
    }   
}