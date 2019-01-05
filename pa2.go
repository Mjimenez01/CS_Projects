/* I Michael Jimenez (NID: MI535599) afﬁrm that this program is entirely my own work and that I have neither developed my code with
 * any another person, nor copied any code from any other person, nor permitted my code to be copied or otherwise used
 * by any other person, nor have I copied, modiﬁed, or otherwise used programs created by others.
 * I acknowledge that any violation of the above terms will be treated as academic dishonesty.
 */

package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"strconv"
	"strings"
)

func main() {

	args := os.Args //This is how we obtain the command line arguments from the terminal.

	if len(args) > 1 {
		_, err := ioutil.ReadFile(args[1])
		check(err)
	} else {
		fmt.Println("You have not given enough command line agruments! Exiting . . .")
		os.Exit(3)
	}

	algorithm, lowerCYL, upperCYL, initCYL := extractInfo(args[1])

	//Check to see which algorithm to use.
	if algorithm == "fcfs" {
		fcfs(algorithm, lowerCYL, upperCYL, initCYL, args[1])
	} else if algorithm == "c-look" {
		clook(algorithm, lowerCYL, upperCYL, initCYL, args[1])
	} else if algorithm == "c-scan" {
		cscan(algorithm, lowerCYL, upperCYL, initCYL, args[1])
	} else if algorithm == "look" {
		look(algorithm, lowerCYL, upperCYL, initCYL, args[1])
	} else if algorithm == "scan" {
		scan(algorithm, lowerCYL, upperCYL, initCYL, args[1])
	} else if algorithm == "sstf" {
		sstf(algorithm, lowerCYL, upperCYL, initCYL, args[1])
	}
	return
}

//This is where we implement the c-look disk scheduling alrorithm. This function is identical to c-scan but different in that
//c-look does not need to account for the offset distacne becasue of the lookahead, thus c-look is a much more efficient algorithm.
func clook(algorithm string, lowerCYL, upperCYL, initCYL int, filename string) {

	//Now we need to load in the cylinder requests.
	var CYLrequests = extractCylinderRequests(filename)
	var _, _, greaterArray, lesserArray = getScanArray(CYLrequests, initCYL)
	var cScanArray = greaterArray
	var previousRequest int
	var traversalCount int
	var MAXVAL = math.MinInt32
	var MINVAL = math.MaxInt32

	//Check to see if any of our requests have errors.
	for i := 0; i < len(CYLrequests); i++ {

		var hasError, Abort = errorCheck(lowerCYL, upperCYL, initCYL, CYLrequests[i])

		//If we have to abort, just return before doing anything.
		if Abort == 1 {
			return
		}

		//If an error is known, skip the current request.
		if hasError == true {
			continue
		}
	}

	fmt.Printf("Seek algorithm: C-LOOK\n")
	fmt.Printf("\tLower cylinder: %5d\n", lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", initCYL)
	fmt.Printf("\tCylinder requests:\n")

	//Print the cylinder requests for testing puropses.
	for i := 0; i < len(CYLrequests); i++ {
		if CYLrequests[i] < upperCYL && CYLrequests[i] > lowerCYL {
			fmt.Printf("\t\tCylinder %5d\n", CYLrequests[i])
		}
	}

	//We need to combine our greater array with our lesser array in ascending order.
	for i := len(lesserArray) - 1; i >= 0; i-- {
		cScanArray = append(cScanArray, lesserArray[i])
	}

	for i := 0; i < len(CYLrequests); i++ {

		//If an error is known, skip the current request.
		if cScanArray[i] > upperCYL || cScanArray[i] < lowerCYL {
			continue
		}

		fmt.Printf("Servicing %5d\n", cScanArray[i])

		//Here we need to find the distance from the largest request to the upper cylinder so that we can add it to our traversal.
		if cScanArray[i] > MAXVAL {
			MAXVAL = cScanArray[i]
		}

		if cScanArray[i] < MINVAL {
			MINVAL = cScanArray[i]
		}

		//If we are on the initial cylinder, the seek time is the abs(initCYL - current cylreq)
		if i == 0 {
			traversalCount += positiveDifference(initCYL, cScanArray[i])
			previousRequest = cScanArray[i]
		} else {
			traversalCount += positiveDifference(previousRequest, cScanArray[i])
			previousRequest = cScanArray[i]
		}
	}

	fmt.Printf("C-LOOK traversal count = %5d\n", (traversalCount))
}

//This is where we implement the look disk scheduling algorithm. This function is identical to scan but different in that
//look does not need to account for offset distance becasue of the lookahead, thus look is a much more efficient algorithm.
func look(algorithm string, lowerCYL, upperCYL, initCYL int, filename string) {

	//Now we need to load in the cylinder requests.
	var CYLrequests = extractCylinderRequests(filename)
	//This variable will be used to determine which direction to scan in our array.
	var traversalCount int
	var previousRequest int
	var MAXVAL = math.MinInt32
	//This is our array of requests in scan order. We also return a flag variable telling us if we need to add offset distance.
	var scanArray, _, _, _ = getScanArray(CYLrequests, initCYL)

	//Check to see if any of our requests have errors.
	for i := 0; i < len(CYLrequests); i++ {

		var hasError, Abort = errorCheck(lowerCYL, upperCYL, initCYL, CYLrequests[i])

		//If we have to abort, just return before doing anything.
		if Abort == 1 {
			return
		}

		//If an error is known, skip the current request.
		if hasError == true {
			continue
		}
	}

	fmt.Printf("Seek algorithm: LOOK\n")
	fmt.Printf("\tLower cylinder: %5d\n", lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", initCYL)
	fmt.Printf("\tCylinder requests:\n")

	//Print the cylinder requests for testing puropses.
	for i := 0; i < len(CYLrequests); i++ {
		if CYLrequests[i] < upperCYL && CYLrequests[i] > lowerCYL {
			fmt.Printf("\t\tCylinder %5d\n", CYLrequests[i])
		}
	}

	for i := 0; i < len(scanArray); i++ {

		//If an error is known, skip the current request.
		if scanArray[i] > upperCYL || scanArray[i] < lowerCYL {
			continue
		}

		fmt.Printf("Servicing %5d\n", scanArray[i])

		//Here we need to find the distance from the largest request to the upper cylinder so that we can add it to our traversal.
		if scanArray[i] > MAXVAL {
			MAXVAL = scanArray[i]
		}

		//If we are on the initial cylinder, the seek time is the abs(initCYL - current cylreq)
		if i == 0 {
			traversalCount += positiveDifference(initCYL, scanArray[i])
			previousRequest = scanArray[i]
		} else {
			traversalCount += positiveDifference(previousRequest, scanArray[i])
			previousRequest = scanArray[i]
		}
	}

	fmt.Printf("LOOK traversal count = %5d\n", traversalCount)
}

//This is where we implement the c-scan disk scheduling algorithm.
func cscan(algorithm string, lowerCYL, upperCYL, initCYL int, filename string) {

	//Now we need to load in the cylinder requests.
	var CYLrequests = extractCylinderRequests(filename)
	var _, offsetFlag, greaterArray, lesserArray = getScanArray(CYLrequests, initCYL)
	var cScanArray = greaterArray
	var previousRequest int
	var traversalCount int
	var MAXVAL = math.MinInt32
	var MINVAL = math.MaxInt32

	//Check to see if any of our requests have errors.
	for i := 0; i < len(CYLrequests); i++ {

		var hasError, Abort = errorCheck(lowerCYL, upperCYL, initCYL, CYLrequests[i])

		//If we have to abort, just return before doing anything.
		if Abort == 1 {
			return
		}

		//If an error is known, skip the current request.
		if hasError == true {
			continue
		}
	}

	fmt.Printf("Seek algorithm: C-SCAN\n")
	fmt.Printf("\tLower cylinder: %5d\n", lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", initCYL)
	fmt.Printf("\tCylinder requests:\n")

	//Print the cylinder requests for testing puropses.
	for i := 0; i < len(CYLrequests); i++ {
		if CYLrequests[i] < upperCYL && CYLrequests[i] > lowerCYL {
			fmt.Printf("\t\tCylinder %5d\n", CYLrequests[i])
		}
	}

	//We need to combine our greater array with our lesser array in ascending order.
	for i := len(lesserArray) - 1; i >= 0; i-- {
		cScanArray = append(cScanArray, lesserArray[i])
	}

	for i := 0; i < len(CYLrequests); i++ {

		//If an error is known, skip the current request.
		if cScanArray[i] > upperCYL || cScanArray[i] < lowerCYL {
			continue
		}

		fmt.Printf("Servicing %5d\n", cScanArray[i])

		//Here we need to find the distance from the largest request to the upper cylinder so that we can add it to our traversal.
		if cScanArray[i] > MAXVAL {
			MAXVAL = cScanArray[i]
		}

		if cScanArray[i] < MINVAL {
			MINVAL = cScanArray[i]
		}

		//If we are on the initial cylinder, the seek time is the abs(initCYL - current cylreq)
		if i == 0 {
			traversalCount += positiveDifference(initCYL, cScanArray[i])
			previousRequest = cScanArray[i]
		} else {
			traversalCount += positiveDifference(previousRequest, cScanArray[i])
			previousRequest = cScanArray[i]
		}
	}

	var offset int

	//If we loop around, account for that by adding that offset value to our traversal count.
	if offsetFlag == true {
		offset = ((upperCYL - MAXVAL) * 2) + ((MINVAL - lowerCYL) * 2)
	}

	fmt.Printf("C-SCAN traversal count = %5d\n", (traversalCount + offset))
}

//This is where we implement the scan disk scheduling algorithm.
func scan(algorithm string, lowerCYL, upperCYL, initCYL int, filename string) {

	//Now we need to load in the cylinder requests.
	var CYLrequests = extractCylinderRequests(filename)
	//This variable will be used to determine which direction to scan in our array.
	var traversalCount int
	var previousRequest int
	var MAXVAL = math.MinInt32

	//Check to see if any of our requests have errors.
	for i := 0; i < len(CYLrequests); i++ {

		var hasError, Abort = errorCheck(lowerCYL, upperCYL, initCYL, CYLrequests[i])

		//If we have to abort, just return before doing anything.
		if Abort == 1 {
			return
		}

		//If an error is known, skip the current request.
		if hasError == true {
			continue
		}
	}

	fmt.Printf("Seek algorithm: SCAN\n")
	fmt.Printf("\tLower cylinder: %5d\n", lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", initCYL)
	fmt.Printf("\tCylinder requests:\n")

	//Print the cylinder requests for testing puropses.
	for i := 0; i < len(CYLrequests); i++ {
		if CYLrequests[i] < upperCYL && CYLrequests[i] > lowerCYL {
			fmt.Printf("\t\tCylinder %5d\n", CYLrequests[i])
		}
	}

	//This is our array of requests in scan order. We also return a flag variable telling us if we need to add offset distance.
	var scanArray, offsetFlag, _, _ = getScanArray(CYLrequests, initCYL)

	for i := 0; i < len(scanArray); i++ {

		//If an error is known, skip the current request.
		if scanArray[i] > upperCYL || scanArray[i] < lowerCYL {
			continue
		}

		fmt.Printf("Servicing %5d\n", scanArray[i])

		//Here we need to find the distance from the largest request to the upper cylinder so that we can add it to our traversal.
		if scanArray[i] > MAXVAL {
			MAXVAL = scanArray[i]
		}

		//If we are on the initial cylinder, the seek time is the abs(initCYL - current cylreq)
		if i == 0 {
			traversalCount += positiveDifference(initCYL, scanArray[i])
			previousRequest = scanArray[i]
		} else {
			traversalCount += positiveDifference(previousRequest, scanArray[i])
			previousRequest = scanArray[i]
		}
	}

	//This is how we account for the extra distance when we scan to the upper cylinder from our max request and back.
	var extraScanDistance int

	//If we have to scan to the upper cylinder to account for turn around time, then add the offset to our traversal count.
	if offsetFlag == true {
		extraScanDistance = (upperCYL - MAXVAL) * 2
	}

	fmt.Printf("SCAN traversal count = %5d\n", (traversalCount + extraScanDistance))
}

//In this function, we return an array that simulates the scan function. Start by getting requests ascending and then decreasing.
func getScanArray(CYLrequests []int, currentCYL int) (scanArray []int, offsetFlag bool, gArray []int, lArray []int) {

	var greaterArray []int
	var lesserArray []int
	var sortedGreaterArray []int
	var sortedLesserArray []int
	var seenArray []int

	//The first then we need to do is get all of the requests that are larger than currentCYL and lower in their respective arrays.
	for i := 0; i < len(CYLrequests); i++ {

		if CYLrequests[i] > currentCYL {
			greaterArray = append(greaterArray, CYLrequests[i])
		} else {
			lesserArray = append(lesserArray, CYLrequests[i])
		}
	}

	//Now that we have separate arrays, we sort the greater array in ascending order and we sort the lesser array in descending.

	if len(greaterArray) > 0 {
		_, _, sortedGreaterArray = sortAscendingSeekTime(greaterArray, currentCYL, seenArray)
	}

	if len(lesserArray) > 0 {
		_, _, sortedLesserArray = sortAscendingSeekTime(lesserArray, currentCYL, seenArray)
	}

	var sortedGreaterCopy = sortedGreaterArray
	var sortedLesserCopy = sortedLesserArray

	//Now that we have both arrays, we need to append the lesser array to the end of the greater array.
	if len(sortedLesserArray) > 0 {
		for i := 0; i < len(sortedLesserArray); i++ {
			sortedGreaterArray = append(sortedGreaterArray, sortedLesserArray[i])
		}
		return sortedGreaterArray, true, sortedGreaterCopy, sortedLesserCopy
	}

	return sortedGreaterArray, false, sortedGreaterArray, sortedLesserCopy
}

//This is where we implement the shortest seek time first disk scheduling algorithm.
func sstf(algorithm string, lowerCYL, upperCYL, initCYL int, filename string) {

	//Now we need to load in the cylinder requests.
	var CYLrequests = extractCylinderRequests(filename)
	var traversalCount int
	var previousRequest = initCYL
	var seenArray []int
	var shortestSeekTime int

	//Check to see if any of our requests have errors.
	for i := 0; i < len(CYLrequests); i++ {
		var hasError, Abort = errorCheck(lowerCYL, upperCYL, initCYL, CYLrequests[i])

		//If we have to abort, just return before doing anything.
		if Abort == 1 {
			return
		}

		//If an error is known, skip the current request.
		if hasError == true {
			continue
		}
	}

	fmt.Printf("Seek algorithm: SSTF\n")
	fmt.Printf("\tLower cylinder: %5d\n", lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", initCYL)
	fmt.Printf("\tCylinder requests:\n")

	//Print the cylinder requests for testing puropses.
	for i := 0; i < len(CYLrequests); i++ {

		if CYLrequests[i] < upperCYL && CYLrequests[i] > lowerCYL {
			fmt.Printf("\t\tCylinder %5d\n", CYLrequests[i])
		}
	}

	//Since we are using FCFS, we service the first array item at each iteration and simply examine the next element after each iteration.
	for i := 0; i < len(CYLrequests); i++ {

		shortestSeekTime, seenArray, _ = sortAscendingSeekTime(CYLrequests, previousRequest, seenArray)

		if shortestSeekTime > upperCYL || shortestSeekTime < lowerCYL {
			continue
		}

		fmt.Printf("Servicing %5d\n", shortestSeekTime)

		//If we are on the initial cylinder, the seek time is the abs(initCYL - current cylreq)
		if i == 0 {
			traversalCount += positiveDifference(initCYL, shortestSeekTime)
			previousRequest = shortestSeekTime
		} else {
			traversalCount += positiveDifference(previousRequest, shortestSeekTime)
			previousRequest = shortestSeekTime
		}
	}

	fmt.Printf("SSTF traversal count = %5d\n", traversalCount)
}

//This function sorts the cylinder requests in ascending order.
func sortAscendingSeekTime(requests []int, currentCYL int, seenRequests []int) (shortestSeeekTime int, seenArray []int, ascendingArray []int) {

	exit := len(requests) - 1
	var temp int

	for {

		if exit == 0 {
			break
		}

		for k := 0; k < len(requests)-1; k++ {

			if positiveDifference(currentCYL, requests[k]) > positiveDifference(currentCYL, requests[k+1]) {
				temp = requests[k+1]
				requests[k+1] = requests[k]
				requests[k] = temp
			}
		}
		exit--
	}

	//Here we are checking to see if one of the requests has been seen before.
	for i := 0; i < len(requests); i++ {

		var flag int

		//We need to check to see if our curent lowest request has been seen before.
		for k := 0; k < len(seenRequests); k++ {

			//If we the current request we have seen, move to the next shortest cylinder request.
			if requests[i] == seenRequests[k] {
				flag = 1
			}
		}

		//If we enter this statement, the current request has not been seen yet. Include it into our seen array and return.
		if flag == 0 {
			seenRequests = append(seenRequests, requests[i])
			return requests[i], seenRequests, requests
		}

	}
	return requests[len(requests)-1], seenRequests, requests
}

//This is where we implement the first come first served disk scheduling algorithm.
func fcfs(algorithm string, lowerCYL, upperCYL, initCYL int, filename string) {

	//Now we need to load in the cylinder requests.
	var CYLrequests = extractCylinderRequests(filename)
	var traversalCount int
	var previousRequest int

	//Check to see if any of our requests have errors.
	for i := 0; i < len(CYLrequests); i++ {

		var hasError, Abort = errorCheck(lowerCYL, upperCYL, initCYL, CYLrequests[i])

		//If we have to abort, just return before doing anything.
		if Abort == 1 {
			return
		}

		//If an error is known, skip the current request.
		if hasError == true {
			continue
		}
	}

	fmt.Printf("Seek algorithm: FCFS\n")
	fmt.Printf("\tLower cylinder: %5d\n", lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", initCYL)
	fmt.Printf("\tCylinder requests:\n")

	//Print the cylinder requests for testing puropses.
	for i := 0; i < len(CYLrequests); i++ {
		if CYLrequests[i] < upperCYL && CYLrequests[i] > lowerCYL {
			fmt.Printf("\t\tCylinder %5d\n", CYLrequests[i])
		}
	}

	//Since we are using FCFS, we service the first array item at each iteration and simply examine the next element after each iteration.
	for i := 0; i < len(CYLrequests); i++ {

		if CYLrequests[i] > upperCYL || CYLrequests[i] < lowerCYL {
			continue
		}

		fmt.Printf("Servicing %5d\n", CYLrequests[i])

		//If we are on the initial cylinder, the seek time is the abs(initCYL - current cylreq)
		if i == 0 {
			traversalCount += positiveDifference(initCYL, CYLrequests[i])
			previousRequest = CYLrequests[i]
		} else {
			traversalCount += positiveDifference(previousRequest, CYLrequests[i])
			previousRequest = CYLrequests[i]
		}
	}

	fmt.Printf("FCFS traversal count = %5d\n", traversalCount)
}

//This function checks to see if any error/abort messages should be thrown.
//TODO: Change this to take a single cylinder request as input and output the following if anything is wrong.
func errorCheck(lowerCYL, upperCYL, initCYL, request int) (hasError bool, Abort int) {

	if lowerCYL >= upperCYL {
		fmt.Printf("ABORT(13):upper (%d) < lower (%d)\n", upperCYL, lowerCYL)
		return true, 1
	} else if initCYL >= upperCYL {
		fmt.Printf("ABORT(11):initial (%d) > upper (%d)\n", initCYL, upperCYL)
		return true, 1
	} else if initCYL <= lowerCYL {
		fmt.Printf("ABORT(12):initial (%d) < lower (%d)\n", initCYL, lowerCYL)
		return true, 1
	} else if request > upperCYL || request < lowerCYL {
		fmt.Printf("ERROR(15):Request out of bounds: req (%d) > upper (%d) or  < lower (%d)\n", request, upperCYL, lowerCYL)
		return true, 0
	}

	return false, 0 //false if all good, false if something is error ridden so that our loop can skip over it.
}

//This is where we take the positive difference
func positiveDifference(req1, req2 int) (difference int) {

	if req1 < req2 {
		return req2 - req1
	}

	return req1 - req2
}

//Function that returns an array of cylinder requests.
func extractCylinderRequests(file string) (cylreqs []int) {

	ifp, err := os.Open(file)
	check(err)
	scanner := bufio.NewScanner(ifp)
	scanner.Split(bufio.ScanLines)

	var text string
	var request int
	var requests []int

	for scanner.Scan() {

		text = scanner.Text()
		var splitText = strings.Fields(text)

		for i := 0; i < len(splitText); i++ {

			if strings.Contains(splitText[i], "#") {
				break
			} else if splitText[i] == "end" {
				break
			} else if splitText[i] == "cylreq" {
				request, err = strconv.Atoi(splitText[i+1])
				check(err)
				requests = append(requests, request)
				request = 0
			}
		}
	}
	return requests
}

//Function that extracts the information from our input file.
func extractInfo(inputFile string) (algorithm string, lowerCYL int, upperCYL int, initCYL int) {

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
			} else if splitText[i] == "cylreq" {
				break
			} else if splitText[i] == "lowerCYL" {
				lowerCYL, err = strconv.Atoi(splitText[i+1])
				check(err)
			} else if splitText[i] == "use" {
				algorithm = splitText[i+1]

			} else if splitText[i] == "upperCYL" {
				upperCYL, err = strconv.Atoi(splitText[i+1])
				check(err)
			} else if splitText[i] == "initCYL" {
				initCYL, err = strconv.Atoi(splitText[i+1])
				check(err)
				break //Break after quantum because all of the information will have been found.
			}
		}
	}
	return algorithm, lowerCYL, upperCYL, initCYL //We can return multiple variables!!!
}

//A small function that reports errors in I/O. Taken from golang.org
func check(e error) {
	if e != nil {
		panic(e)
	}
}
