/*
 * University of Central Florida    
 * CAP 4630 - Fall 2018
 * Author(s): <Michael Jimenez>
 */

package multiclass;

import weka.core.Capabilities;
import weka.core.Instance;
import weka.core.Instances;

public class MulticlassPerceptron implements weka.classifiers.Classifier{

	int Bias = 1;				
	int TotalWeightUpdates = 0;	// This will hold the total number of updated weights throughout our training epochs.  
	Instances data;				// This is the data taken from our input.arff file.
	String FileName;			// This is the name of the *.arff file that we have read from (args[0]).
	int NumEpochs;				// This is the max amount of epochs that we will calculate in our program (args[1]). 
	double weights[][];			// Each row represents a class vector, while ever col represents a weight in the class vector.
	int EpochIteration;			// This is the current epoch iteration that we are on in our training model.
	double inputArray[];		// This holds our input vector which we will use to adjust the weights. 
	int ExtraIteration = 0;			// If we iterate an extra time the counter will be higher then it should. Set to 1 if true. 0 otherwise. 
	
	public MulticlassPerceptron(String[] args){
		
		//Program Header
		System.out.println("University of Central Florida\nCAP4630 Artifical Intelligence - Fall 2018\nMulti-Class Perceptron Classifier");
		System.out.println("Author(s): Michael Jimenez (NID: MI535599)");
		
		FileName = args[0];
		NumEpochs = Integer.parseInt(args[1]);
	}
	
	@Override
	public void buildClassifier(Instances DataSet) throws Exception {
				
		//getCapabilities().testWithFail(data);
		String bitString = "";
		int WeightUpdates = 0;
		data = DataSet;		
		data = new Instances(data);
		data.deleteWithMissingClass();
		
		//All of our weights will be set to zero by default. The last weight will be for the Bias.
		weights = new double[data.numClasses()][data.numAttributes()];
		
		System.out.println();
				
		//Loop as many times as specified as from the command line (args[1]) for each epoch.
		for(EpochIteration = 1; EpochIteration < NumEpochs + 1; EpochIteration++) 
		{
			// Loop over each instance of data for each epoch. 
			for(int j = 0; j < data.numInstances(); j++)
			{	
				//Get the current data row for further examination.
				Instance instance = data.get(j);
						
				//Returns the index of the predicted class.  
				double ClassIndexPrediction = predict(instance);				
								
				//This returns a double indicating the value of the class value. If our class value is a string, it returns the index of that string. 
				double activation = instance.classValue();
				
				//If our model predicts correctly, concatenate a 1 to our bit string.
				if(ClassIndexPrediction == activation)
					bitString = bitString.concat("1");
				
				//If our model predicts incorrectly, concatenate a 0 to our bit string.
				else
				{
					bitString = bitString.concat("0");
					
					//Here we need the index of the incorrect class prediction and the index of the correct class.
					UpdateWeights(ClassIndexPrediction, activation);
					TotalWeightUpdates++;
					WeightUpdates++;
				}
			}
			
			System.out.println("Epoch	" + EpochIteration + " [" + WeightUpdates + "] " + bitString);
			
			//If we are able to predict our classes with no weights needing to be updated, then we can exit our loop.
			if(WeightUpdates == 0)
			{
				ExtraIteration = 0; 
				break;
			}
			
			//Reset our bit string and weight updates for the next epoch.
			bitString = new String("");
			WeightUpdates = 0;
			ExtraIteration = 1;
		}
	}
	
	//If we have chosen incorrectly, we need to adjust our current weights. Lower the weight of the incorrect
	//choice and increase the weight of the correct choice.
	public void UpdateWeights(double prediction, double activation){
		
		//Increase the weights in the activation and decrease the weights indexed at the prediction because we predicted incorrectly.
		//We will only ever need to update 2 weights, the incorrect and the correct choice vectors.
		//Update the incorrect weight.
		for(int i = 0; i < data.numAttributes(); i++)
			weights[(int)prediction][i] -= inputArray[i]; 
		
		//Update the correct weight.
		for(int k = 0; k < data.numAttributes(); k++)
			weights[(int)activation][k] += inputArray[k]; 
	}
	
	@Override 
	public double classifyInstance(Instance arg0) throws Exception {
		return 0;
	}

	@Override //method returns a distribution array where the value for the predicted class is 1 and all other values are zero.
	public double[] distributionForInstance(Instance instance) throws Exception {
		
		double[] result = new double[data.numClasses()];
		result[predict(instance)] = 1;
		return result;
	}

	@Override
	public Capabilities getCapabilities() {
		return null;
	}
	
	public String toString() {
				
		String s = "";
		s = s.concat("Source file: " + FileName + "\n");
		s = s.concat("Training epoch limit   : " + NumEpochs + "\n");
		s = s.concat("Actual training epochs : " + (EpochIteration - ExtraIteration) + "\n");
		s = s.concat("Total # weight updates : " + TotalWeightUpdates + "\n");
		s = s.concat("\n" + "Final weights:" + "\n\n");
		s = s.concat(ClassWeights()); 
				
		return s;
	}
	
	//Add the class weights to our toString method.
	public String ClassWeights()
	{		
		String s = "";
		
		for(int i = 0; i < data.numClasses(); i++)
		{
			s = s.concat("Class " + i + " weights: ");
			
			for(int k = 0; k < data.numAttributes(); k++)
				s= s.concat(String.format("%8.3f", weights[i][k]) + " ");
			
			s = s.concat("\n");
		}	
		return s;
	}
	
	//applies the perceptron algorithm to predict the class	for	a particular instance. Returns the index for the predicted class index values range from 0
	//to (data.numClasses() – 1) class 0 is the first class label encountered in the training set, class 1	is the second, and so on.
	public int predict(Instance instance){
				
		//This is the result from argmax(over all y)[weights * input].
		double DotProduct = 0;
		int index = 0;
		double MAXVAL = Double.MIN_VALUE;
		//This creates the input vector with the last cell reserved for our Bias with a value of 1.0.
		inputArray = new double[instance.numAttributes()];
		inputArray[inputArray.length-1] = 1.0;
				
		//Set the values of our input vector to the current instances row values.
		for(int i = 0; i < instance.numValues() - 1; i++)
			inputArray[i] = instance.value(i);
		
		//Now that we have our weight vectors and our input vector, we need to compute the dot product of the input vector with
		//all of the weight vectors and take the max of all of these products as our prediction. Return the index of this value.
		for(int i = 0; i < data.numClasses(); i++)
		{	
			for(int k = 0; k < data.numAttributes(); k++)
				DotProduct += inputArray[k] * weights[i][k];	
						
			if(DotProduct > MAXVAL)
			{
				MAXVAL = DotProduct;
				index = i;
			}
			
			//Reset the DotProduct and MAXVAL per computation.
			MAXVAL = Double.MIN_VALUE;
			DotProduct = 0;
		}		
		return index;
	}
}
