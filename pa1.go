/* “I Michael Jimenez mi535599 afﬁrm that this program is entirely my own work and that I have neither developed my code
 * together with any another person, nor copied any code from any other person, nor permitted my code to be copied
 * or otherwise used by any other person, nor have I copied, modiﬁed, or otherwise used programs created by others.
 * I acknowledge that any violation of the above terms will be treated as academic dishonesty.”
 */

package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"strconv"
	"strings"
)

//This is our process sturct. This will hold all the information we need to know about an individual process.
type pcb struct {
	processName string
	arrivalTime int
	burstTime   int
	waitTime    int
	turnaround  int
	lastRan     int
	running     bool
	arrived     bool
	finished    bool
}

func main() {
	args := os.Args //This is how we obtain the command line arguments from the terminal.

	if len(args) > 2 {
		_, err := ioutil.ReadFile(args[1])
		check(err)
	} else {
		fmt.Println("You have not given enough command line agruments! Exiting . . .")
		os.Exit(3)
	}
	//Now that we have checked if we can access our input file, we need to get all of the information from our input file.
	totalRuntime, numProcess, algorithm, quantum := extractInfo(args[1])

	//Check to see which algorithm to use.
	if algorithm == "rr" {
		rr(totalRuntime, numProcess, quantum, args[1], args[2])
	} else if algorithm == "sjf" {
		sjf(totalRuntime, numProcess, args[1], args[2])
	} else if algorithm == "fcfs" {
		fcfs(totalRuntime, numProcess, args[1], args[2])
	}
	return
}

func sjf(totalRuntime, numProcess int, file, outfile string) {

	//Here we create our output file.
	ofp, err := os.Create(outfile)
	check(err)

	//Print the header information for First Come First Served.
	fmt.Fprintf(ofp, "%3d processes\n", numProcess) //All integer numeric data is formatted using %3d.
	fmt.Fprintf(ofp, "Using preemptive Shortest Job First\n")

	//This is how we know which processes are currently in line to be run.
	var requestQueue []pcb

	//This array will hold all of our completed processes.
	var completedProcesses []pcb

	//At this point, we need to obtain the process information. Return an array of pcb objects from the extractProcessInfo function.
	//FIX THIS TOMORROW! MY PARSE FUNCTION TRIES TO CONVERT "TO" INTO AN INTEGER. THIS IS INCORRECT!!!!!!
	var pcbArray = extractProcessInfo(file, numProcess)

	//Before we begin Shortest Job First, we need to have the pcbArray in ascnding order of arrival time.
	var pcbArrayAT = ascendingArrivalTime(pcbArray)

	for i := 0; i < totalRuntime; i++ {

		//Check to see if a process arrives.
		for j := 0; j < len(pcbArrayAT); j++ {

			if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
				fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
				pcbArrayAT[j].arrived = true
				requestQueue = append(requestQueue, pcbArrayAT[j])
			}
		}

		var needToBurst bool

		//If we have processes that can be ran, needToBurst = true, else needToBurst = false.
		for r := 0; r < len(requestQueue); r++ {
			if requestQueue[r].burstTime > 0 {
				needToBurst = true
				break
			}
			needToBurst = false
		}

		//If there are currently no processes able to run, then we are idling.
		if needToBurst == false {
			fmt.Fprintf(ofp, "Time %3d : Idle\n", i)
			i++
		}

		//If we have current processes in our requestQueue, then find the shortest and run it at every increment.
		if len(requestQueue) > 0 && needToBurst {

			var currentProcess pcb
			var MIN = math.MaxInt32
			var index int
			var leaveLoop = false

			//Select the lowest burst time process in our requestQueue.
			for k := 0; k < len(requestQueue); k++ {

				if requestQueue[k].burstTime < MIN && requestQueue[k].burstTime > 0 {
					MIN = requestQueue[k].burstTime
					currentProcess = requestQueue[k]
					index = k
				}

				//Remove the element from the requestQueue once it is chosen as the current process.
				if k == len(requestQueue)-1 {
					requestQueue = requestQueue[:index+copy(requestQueue[index:], requestQueue[index+1:])]
				}
			}

			//Update the wait time for our current process.
			if currentProcess.lastRan == 0 {
				currentProcess.waitTime += i - (currentProcess.arrivalTime)
			} else {
				currentProcess.waitTime += i - (currentProcess.lastRan)
			}

			fmt.Fprintf(ofp, "Time %3d : %s selected (burst %3d)\n", i, currentProcess.processName, currentProcess.burstTime)

			//Loop until the current process is completed or, until another process has a shorter burst time than the current process.
			for {
				currentProcess.burstTime--
				i++

				//Check to see if a process arrives.
				for j := 0; j < len(pcbArrayAT); j++ {

					if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
						fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
						pcbArrayAT[j].arrived = true
						requestQueue = append(requestQueue, pcbArrayAT[j])
					}
				}

				//Check to see if our process still has the lowest burst time.
				for k := 0; k < len(requestQueue); k++ {

					if requestQueue[k].burstTime < currentProcess.burstTime && requestQueue[k].burstTime > 0 && requestQueue[k].processName != currentProcess.processName {
						leaveLoop = true
						break
					}
				}

				if leaveLoop == true {
					break
				}

				//Check to see if our process has completed.
				if currentProcess.burstTime == 0 {
					currentProcess.finished = true
					currentProcess.turnaround = (i - currentProcess.arrivalTime)
					completedProcesses = append(completedProcesses, currentProcess)
					fmt.Fprintf(ofp, "Time %3d : %s finished\n", i, currentProcess.processName)
					break
				}
			}

			//Dont forget to add the process to the back of our pcbArrayAT if it still has more burst time.
			if currentProcess.burstTime > 0 {
				currentProcess.running = false
				currentProcess.lastRan = i
				requestQueue = append(requestQueue, currentProcess)
			}
		}
		i--
		continue
	}
	fmt.Fprintf(ofp, "Finished at time %3d\n\n", totalRuntime)
	printOrderedPcb(completedProcesses, numProcess, ofp)
	ofp.Close()
}

//Function that implements the First come first served CPU scheduling algorithm.
func fcfs(totalRuntime, numProcess int, file, outfile string) {

	ofp, err := os.Create(outfile)
	check(err)

	//Print the header information for First Come First Served.
	fmt.Fprintf(ofp, "%3d processes\n", numProcess) //All integer numeric data is formatted using %3d.
	fmt.Fprintf(ofp, "Using First-Come First-Served\n")

	//This is how we know which processes are currently in line to be run.
	var requestQueue []pcb

	//This array will hold all of our completed processes.
	var completedProcesses []pcb

	//At this point, we need to obtain the process information. Return an array of pcb objects from the extractProcessInfo function.
	var pcbArray = extractProcessInfo(file, numProcess)

	//Before we begin Round Robin, we need to have the pcbArray in ascnding order of arrival time.
	var pcbArrayAT = ascendingArrivalTime(pcbArray)

	for i := 0; i < totalRuntime; i++ {

		//Check to see if a process arrives.
		for j := 0; j < len(pcbArrayAT); j++ {

			if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
				fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
				pcbArrayAT[j].arrived = true
				requestQueue = append(requestQueue, pcbArrayAT[j])
			}
		}

		//If there are currently no processes.
		if len(requestQueue) == 0 {
			fmt.Fprintf(ofp, "Time %3d : Idle\n", i)
			i++
		}

		if len(requestQueue) > 0 {

			//Select the first process in our requestQueue.
			var currentProcess = requestQueue[0]

			//Once we have selected the process to execute, set it to running.
			currentProcess.running = true

			//Update the wait time for our current process.
			if currentProcess.lastRan == 0 {
				currentProcess.waitTime += i - (currentProcess.arrivalTime)
			} else {
				currentProcess.waitTime += i - (currentProcess.lastRan)
			}

			//Remove the first element of the array after the move has been selected.
			requestQueue = requestQueue[1:]

			fmt.Fprintf(ofp, "Time %3d : %s selected (burst %3d)\n", i, currentProcess.processName, currentProcess.burstTime)

			burstLength := currentProcess.burstTime

			//Loop until the current processes burst time is completed.
			for t := 0; t < burstLength; t++ {
				currentProcess.burstTime--
				i++

				for j := 0; j < len(pcbArrayAT); j++ {

					if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
						fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
						pcbArrayAT[j].arrived = true
						requestQueue = append(requestQueue, pcbArrayAT[j])
					}
				}

				if currentProcess.burstTime == 0 {
					currentProcess.finished = true
					currentProcess.turnaround = (i - currentProcess.arrivalTime)
					completedProcesses = append(completedProcesses, currentProcess)
					fmt.Fprintf(ofp, "Time %3d : %s finished\n", i, currentProcess.processName)
					break
				}
			}
			i--
			continue
		}

		if len(requestQueue) == 0 && i < totalRuntime {
			fmt.Fprintf(ofp, "Time %3d : Idle\n", i)
		}

		for j := 0; j < len(pcbArrayAT); j++ {

			if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
				fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
				pcbArrayAT[j].arrived = true
				requestQueue = append(requestQueue, pcbArrayAT[j])
			}
		}
	}
	fmt.Fprintf(ofp, "Finished at time %3d\n\n", totalRuntime)
	printOrderedPcb(completedProcesses, numProcess, ofp)
	ofp.Close()
}

//Function that implements the Round Robin CPU scheduling algorithm.
func rr(totalRuntime, numProcess, quantum int, file, outfile string) {

	ofp, err := os.Create(outfile)
	check(err)

	//Print the header information for Round Robin.
	fmt.Fprintf(ofp, "%3d processes\n", numProcess) //All integer numeric data is formatted using %3d.
	fmt.Fprintf(ofp, "Using Round-Robin\n")
	fmt.Fprintf(ofp, "Quantum %3d\n\n", quantum)

	//This is how we know which processes are currently in line to be run.
	//Whatever that I put in this queue, I need to be able to save the modifications!
	var requestQueue []pcb

	//This array will hold all of our completed processes.
	var completedProcesses []pcb

	//At this point, we need to obtain the process information. Return an array of pcb objects from the extractProcessInfo function.
	var pcbArray = extractProcessInfo(file, numProcess)

	//Before we begin Round Robin, we need to have the pcbArray in ascnding order of arrival time.
	var pcbArrayAT = ascendingArrivalTime(pcbArray)

	//Here we begin our scheduling algorithm and loop for the amount of totalRuntime.
	for i := 0; i < totalRuntime; i++ {

		//Check to see if a process arrives.
		for j := 0; j < len(pcbArrayAT); j++ {

			if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
				fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
				pcbArrayAT[j].arrived = true
				requestQueue = append(requestQueue, pcbArrayAT[j])
			}
		}

		//If there are currently no processes.
		if len(requestQueue) == 0 {
			fmt.Fprintf(ofp, "Time %3d : Idle\n", i)
			i++
		}

		//If we have arrived processes, then we need to begin scheduling.
		if len(requestQueue) > 0 {

			//Select the first process in our requestQueue.
			var currentProcess = requestQueue[0]

			//Once we have selected the process to execute, set it to running.
			currentProcess.running = true

			//Update the wait time for our current process.
			if currentProcess.lastRan == 0 {
				currentProcess.waitTime += i - (currentProcess.arrivalTime)
			} else {
				currentProcess.waitTime += i - (currentProcess.lastRan)
			}

			//Remove the first element of the array after the move has been selected.
			requestQueue = requestQueue[1:]

			fmt.Fprintf(ofp, "Time %3d : %s selected (burst %3d)\n", i, currentProcess.processName, currentProcess.burstTime)

			for t := 0; t < quantum; t++ {
				currentProcess.burstTime--
				i++

				for j := 0; j < len(pcbArrayAT); j++ {

					if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
						fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
						pcbArrayAT[j].arrived = true
						requestQueue = append(requestQueue, pcbArrayAT[j])
					}
				}

				if currentProcess.burstTime == 0 {
					currentProcess.finished = true
					currentProcess.turnaround = (i - currentProcess.arrivalTime)
					completedProcesses = append(completedProcesses, currentProcess)
					fmt.Fprintf(ofp, "Time %3d : %s finished\n", i, currentProcess.processName)
					break
				}
			}

			//Dont forget to add the process to the back of our pcbArrayAT if it still has more burst time.
			if currentProcess.burstTime > 0 {
				currentProcess.running = false
				currentProcess.lastRan = i
				requestQueue = append(requestQueue, currentProcess)
			}

			i--
			continue
		}

		if len(requestQueue) == 0 && i < totalRuntime {
			fmt.Fprintf(ofp, "Time %3d : Idle\n", i)
		}

		for j := 0; j < len(pcbArrayAT); j++ {

			if pcbArrayAT[j].arrivalTime == i && pcbArrayAT[j].arrived == false {
				fmt.Fprintf(ofp, "Time %3d : %s arrived\n", i, pcbArrayAT[j].processName)
				pcbArrayAT[j].arrived = true
				requestQueue = append(requestQueue, pcbArrayAT[j])
			}
		}
	}
	fmt.Fprintf(ofp, "Finished at time %3d\n\n", totalRuntime)
	printOrderedPcb(completedProcesses, numProcess, ofp)
	ofp.Close()
}

func printOrderedPcb(completedProcesses []pcb, numProcess int, ofp *os.File) {

	var buffer bytes.Buffer

	//Once we have finished scheduling our processes, we need to print out the wait time and turnaround for each process.
	for p := 0; p < len(completedProcesses); p++ {

		buffer.WriteString("P")

		if numProcess == 10 && p < 9 {
			buffer.WriteString("0")
		}

		buffer.WriteString(strconv.Itoa(p + 1))

		//write a funciton that looks at the process name and returns them in ascending order.
		var orderedPcb = orderedPcb(completedProcesses, buffer.String())
		fmt.Fprintf(ofp, "%s wait %3d turnaround %3d\n", orderedPcb.processName, orderedPcb.waitTime, orderedPcb.turnaround)
		buffer.Reset()
	}
}

//This function will return the process specified using the string in the given pcbArray.
func orderedPcb(pcbArray []pcb, processName string) (foundProcess pcb) {

	for i := 0; i < len(pcbArray); i++ {

		if pcbArray[i].processName == processName {
			return pcbArray[i]
		}
	}

	return foundProcess
}

//Sort our pcb array in ascending order of arrival time use the classic bubble array sort.
func ascendingArrivalTime(pcbArray []pcb) (ascendingArrivalTimePcbArray []pcb) {

	exit := len(pcbArray) - 1
	var temp pcb

	for {

		if exit == 0 {
			break
		}

		for k := 0; k < len(pcbArray)-1; k++ {

			if pcbArray[k].arrivalTime > pcbArray[k+1].arrivalTime {
				temp = pcbArray[k+1]
				pcbArray[k+1] = pcbArray[k]
				pcbArray[k] = temp
			}
		}
		exit--
	}
	return pcbArray
}

//Function that returns an array of pcb objects.
func extractProcessInfo(file string, numProcess int) (pcbArray []pcb) {

	ifp, err := os.Open(file)
	check(err)
	scanner := bufio.NewScanner(ifp)
	scanner.Split(bufio.ScanLines)

	var text string
	var process = pcb{}

	for scanner.Scan() {

		text = scanner.Text()
		var splitText = strings.Fields(text)

		for i := 0; i < len(splitText); i++ {

			if splitText[i] == "name" {
				process.processName = splitText[i+1]
			} else if strings.Contains(splitText[i], "#") {
				break
			} else if splitText[i] == "arrival" {
				process.arrivalTime, err = strconv.Atoi(splitText[i+1])
				check(err)
			} else if splitText[i] == "burst" {
				process.burstTime, err = strconv.Atoi(splitText[i+1])
				check(err)
				pcbArray = append(pcbArray, process)
				process = pcb{}
			}
		}
	}
	return pcbArray
}

//Function that extracts the information from our input file.
func extractInfo(inputFile string) (totalRuntime int, numProcesses int, algorithm string, quantum int) {

	file, _ := os.Open(inputFile) //Return the file pointer to the inputfile. The '_' denotes the use of only a subset of the return values.
	scanner := bufio.NewScanner(file)
	scanner.Split(bufio.ScanLines)
	var text string
	var err error //The variable that we will use if an error is thrown.

	for scanner.Scan() {

		text = scanner.Text()
		var splitText = strings.Fields(text)

		for i := 0; i < len(splitText); i++ {

			if strings.Contains(splitText[i], "#") {
				break
			} else if splitText[i] == "processcount" {
				numProcesses, err = strconv.Atoi(splitText[i+1])
				check(err)
			} else if splitText[i] == "use" {
				algorithm = splitText[i+1]

				if algorithm != "rr" { //We have to break here so that we dont accadently read an incorrect algorithm.
					break
				}

			} else if splitText[i] == "runfor" {
				totalRuntime, err = strconv.Atoi(splitText[i+1])
				check(err)
			} else if splitText[i] == "quantum" {
				quantum, err = strconv.Atoi(splitText[i+1])
				check(err)
				break //Break after quantum because all of the information will have been found.
			}
		}
	}
	return totalRuntime, numProcesses, algorithm, quantum //We can return multiple variables!!!
}

//A small function that reports errors in I/O. Taken from golang.org
func check(e error) {
	if e != nil {
		panic(e)
	}
}
