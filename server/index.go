package main

import (
	"fmt"
	"log"
	"encoding/json"
	"io"
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
	log.Printf("%s %s content-type:%s", req.Method, req.URL, req.Header["Content-Type"])
	io.Copy(io.Discard, req.Body)
	bytes, err := os.ReadFile("slots.json")
	if err != nil {
		log.Print(err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	w.Header().Set("content-type", "application/json")
	w.Write(bytes)
}

func postSlotsProcessor(w http.ResponseWriter, req *http.Request) {
	log.Printf("%s %s content-type:%s", req.Method, req.URL, req.Header["Content-Type"])
	defer req.Body.Close()
	var jsonMap _jsonMap
	err := json.NewDecoder(req.Body).Decode(&jsonMap)
	if err != nil {
		log.Print(err)
		w.WriteHeader(http.StatusExpectationFailed)
		return
	}
	for key, v := range jsonMap {
		if v.Deleted == true { 
			delete(jsonMap, key)
		}
	}
	json, err := json.Marshal(jsonMap)
	if err != nil {
		log.Print(err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	err = os.WriteFile("slots.json", json, 0644)
	if err != nil {
		log.Print(err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	w.WriteHeader(http.StatusCreated) 

}


func main() {
	http.HandleFunc("GET /get-slots", getSlotsProcessor)
	http.HandleFunc("POST /post-slots", postSlotsProcessor)

	fmt.Println("Helios server listening on localhost:4000")
	log.Fatal(http.ListenAndServe(":4000", nil))

}