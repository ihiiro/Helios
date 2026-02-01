package main

import (
	"fmt"
	"log"
	"encoding/json"
	"io"
	"bufio"
	"os"
	"net/http"
)

type _context struct {
	WatchContext string `json:"watch_context"`
	BackupContext string `json:"backup_context"`
	Deleted bool `json:"deleted"`
}

type _jsonMap map[uint64]_context

func getSlotsProcessor(w http.ResponseWriter, req *http.Request) {
	fmt.Println("received request")
	/* discard request body */
	io.Copy(io.Discard, req.Body)
	/* read and extract JSON from file into struct */
	file, err := os.Open("slots.json")
	if err != nil {
		fmt.Println(err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer file.Close()
	reader := bufio.NewReader(file)
	completeBuf := make([]byte, 0)
	buf := make([]byte, 1024)
	for {
		n, err := reader.Read(buf)
		if err != nil {
			if err == io.EOF {
				break
			}
			fmt.Println(err)
			w.WriteHeader(http.StatusInternalServerError)
			return
		}
		completeBuf = append(completeBuf, buf[:n]...)
	}
	w.Header().Set("content-type", "application/json")
	w.Write(completeBuf)
}

func postSlotsProcessor(w http.ResponseWriter, req *http.Request) {
	defer req.Body.Close()

	var jsonMap _jsonMap
	err := json.NewDecoder(req.Body).Decode(&jsonMap)
	if err != nil {
		fmt.Println(err)
		w.WriteHeader(http.StatusExpectationFailed)
		return
	}
	for key, v := range jsonMap {
		if v.Deleted == true {
			delete(jsonMap, key)
		}
	}
	json, err0 := json.Marshal(jsonMap)
	if err0 != nil {
		fmt.Println(err0)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	err0 = os.WriteFile("slots.json", json, 0644)
	if err0 != nil {
		fmt.Println(err0)
		w.WriteHeader(http.StatusInternalServerError)
	}
	w.WriteHeader(http.StatusCreated)

}


func main() {
	http.HandleFunc("GET /get-slots", getSlotsProcessor)
	http.HandleFunc("POST /post-slots", postSlotsProcessor)

	fmt.Println("Helios server listening on localhost:4000")
	log.Fatal(http.ListenAndServe(":4000", nil))

}