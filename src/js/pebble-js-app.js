// pebble-js-app.js ////////////////////////////////////////////////////////////
// Javascript for WWDC 2014 Schedule for Pebble
// Created by: Brian Jett
//             bdjett@me.com
//             http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

var schedule; // will hold cached JSON schedule from Apple
var currentSessions = [];
var timeGroups = {};
var httpTimeout = 12000;
var maxAppMessageBuffer = 100;
var maxAppMessageTries = 3;
var appMessageRetryTimeout = 3000;
var appMessageTimeout = 100;
var appMessageQueue = [];

// void getSchedule() //////////////////////////////////////////////////////////
// Purpose:   Retrieves the schedule from Apple and cache it on the watch in
//            JSON format
// Receives:  Nothing
// Returns:   Nothing
////////////////////////////////////////////////////////////////////////////////
function getSchedule() {
    var req = new XMLHttpRequest();
    var requestUrl = "http://devimages.apple.com.edgekey.net/wwdc-services/d20ft1ql/fj38fbvcm103bng7fgp2/sessions.json";
    req.open('GET', requestUrl, false);
    req.responseType = "json";
    req.onload = function(e) {
        if (req.readyState == 4) {
            if (req.status == 200) {
                if (req.response) {
                    schedule = req.response;
                    getCurrentEvents();
                } else {
                    sendError("Please try again.");
                }
            } else {
                sendError("Please try again.");
            }
        } else {
            sendError("Please try again.");
        }
    };
    req.timeout = httpTimeout;
    req.ontimeout = function() {
        console.log("HTTP request timed out");
        sendError("Please try again.");
        schedule = null;
    };
    req.onerror = function() {
        console.log("HTTP request returned error");
        sendError("Please try again.");
        schedule = null;
    };
    req.send(null);
}

// void getCurrentEvents() /////////////////////////////////////////////////////
// Purpose:   Retrieves the events scheduled for the current day and groups
//            them by start time
// Receives:  Nothing
// Returns:   Nothing
////////////////////////////////////////////////////////////////////////////////
function getCurrentEvents() {
    var date = new Date(); // create an event object to compare with
    console.log(date);
    if (schedule) {
        // check to ensure that there is a cached version of the schedule
        schedule['response']['sessions'].forEach(function(element, index, array) {
            var sessionDate = new Date(element['startGMT']);
            if ((date.getDate() == sessionDate.getDate()) && (date.getMonth() == sessionDate.getMonth()) && (date.getFullYear() == sessionDate.getFullYear())) {
                // if the event is on the current date, push it to the events
                // array for sorting
                currentSessions.push(element);
            }
        });
        if (currentSessions.length > 0) {
            currentSessions.sort(function(a, b) {
                // sort the array by date
                var keyA = new Date(a['startGMT']);
                var keyB = new Date(b['startGMT']);
                if (keyA < keyB) return -1;
                if (keyA > keyB) return 1;
                return 0;
            });
            currentSessions.forEach(function(element, index, array) {
                // push the events to the pebble
                if (timeGroups[element['startGMT']]) {
                    timeGroups[element['startGMT']].push(element);
                } else {
                    timeGroups[element['startGMT']] = [];
                    timeGroups[element['startGMT']].push(element);
                }
            });
            // Send over sections
            var count = 0;
            for (var k in timeGroups) {
                // format the time
                var time = new Date(k);
                var hour = time.getHours();
                var min = time.getMinutes();
                if (hour >= 12) {
                    var ampm = "PM";
                    hour = hour % 12;
                } else {
                    var ampm = "AM";
                }
                if (hour == 0) {
                    hour = 12;
                }
                if (min < 10) {
                    min = "0" + min;
                }
                appMessageQueue.push({'message': {
                                        'sectionIndex': count,
                                        'sectionName': hour + ":" + min + " " + ampm,
                                        'sectionKey': k,
                                        'sectionNumber': timeGroups[k].length + " Sessions"
                }});
                //console.log(k + " " + timeGroups[k].length);
                count++;
            }
            sendAppMessage();
        } else if (date < new Date(2014, 5, 2)) {
            sendError("Check back June 2!");
        } else {
            sendError("WWDC 2014 is Over!")
        }
    }
}

// void getEventsForTime(int index) ////////////////////////////////////////////
// Purpose:   Gets all of the events in the selected section index
// Recieves:  A section index
// Returns:   Nothing
////////////////////////////////////////////////////////////////////////////////
function getEventsForTime(sectionIndex) {
    timeGroups[sectionIndex].forEach(function(element, index, array) {
        // format the time
        var startTime = new Date(element['startGMT']);
        var endTime = new Date(element['endGMT']);
        var startHour = startTime.getHours();
        var startMin = startTime.getMinutes();
        var endHour = endTime.getHours();
        var endMin = endTime.getMinutes();
        if (startHour >= 12) {
            var startAmpm = "PM";
            startHour = startHour % 12;
        } else {
            var startAmpm = "AM"
        }
        if (startHour == 0) {
            startHour = 12;
        }
        if (startMin < 10) {
            startMin = "0" + startMin;
        }
        if (endHour >= 12) {
            var endAmpm = "PM";
            endHour = endHour % 12;
        } else {
            var endAmpm = "AM"
        }
        if (endHour == 0) {
            endHour = 12;
        }
        if (endMin < 10) {
            endMin = "0" + endMin;
        }
        //console.log(element['title']);
        appMessageQueue.push({'message': {
                                'index': index,
                                'id': element['id'],
                                'title': element['title'],
                                'type': element['type'],
                                'room': element['room'],
                                'time': startHour + ":" + startMin + " " + startAmpm + " - " + endHour + ":" + endMin + " " + endAmpm
        }});
    });
    sendAppMessage();
}

// void getEventInfo(string eventID) ///////////////////////////////////////////
// Purpose:   Given an event ID, returns the event information for the details
//            screen
// Receives:  A string with the event ID
// Returns:   Nothing
////////////////////////////////////////////////////////////////////////////////
function getEventInfo(eventID) {
    var eventInfo;
    if (currentSessions) {
        currentSessions.forEach(function(element, index, array) {
            if (element['id'] === eventID) {
                eventInfo = element;
            }
        });
        // format the time
        var startTime = new Date(eventInfo['startGMT']);
        var endTime = new Date(eventInfo['endGMT']);
        var startHour = startTime.getHours();
        var startMin = startTime.getMinutes();
        var endHour = endTime.getHours();
        var endMin = endTime.getMinutes();
        if (startHour >= 12) {
            var startAmpm = "PM";
            startHour = startHour % 12;
        } else {
            var startAmpm = "AM"
        }
        if (startHour == 0) {
            startHour = 12;
        }
        if (startMin < 10) {
            startMin = "0" + startMin;
        }
        if (endHour >= 12) {
            var endAmpm = "PM";
            endHour = endHour % 12;
        } else {
            var endAmpm = "AM"
        }
        if (endHour == 0) {
            endHour = 12;
        }
        if (endMin < 10) {
            endMin = "0" + endMin;
        }
        appMessageQueue.push({'message': {
                                'title': eventInfo['title'],
                                'time': startHour + ":" + startMin + " " + startAmpm + " - " + endHour + ":" + endMin + " " + endAmpm,
                                'room': eventInfo['room']
        }});
        sendAppMessage();
    }
}

// void sendError(string error) ////////////////////////////////////////////////
// Purpose:   Sends error message to Pebble
// Recieves:  String with the error text
// Returns:   Nothing
////////////////////////////////////////////////////////////////////////////////
function sendError(error) {
    appMessageQueue.push({'message': {
                            'error': error
    }});
    sendAppMessage();
}

// void sendAppMessage() ///////////////////////////////////////////////////////
// Purpose:   Sends array of app messages to Pebble
// Receives:  Nothing
// Returns:   Nothing
////////////////////////////////////////////////////////////////////////////////
function sendAppMessage() {
    if (appMessageQueue.length > 0) {
        var currentAppMessage = appMessageQueue[0];
        currentAppMessage.numTries = currentAppMessage.numTries || 0;
        currentAppMessage.transactionId = currentAppMessage.transactionId || -1;

        if (currentAppMessage.numTries < maxAppMessageTries) {
            //console.log("Sending message");
            Pebble.sendAppMessage(
                currentAppMessage.message,
                function(e) {
                    appMessageQueue.shift();
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageTimeout);
                }, function(e) {
                    console.log("Faled sending AppMessage for transactionId: " + e.data.transactionId + ". Error: " + e.data.error.message);
                    appMessageQueue[0].transactionId = e.data.transactionId;
                    appMessageQueue[0].numTries++;
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageRetryTimeout);
                }
            );
        } else {
            console.log("Faled sending AppMessage after multiple attemps for transactionId: " + currentAppMessage.transactionId + ". Error: None. Here's the message: " + JSON.stringify(currentAppMessage.message));
        }
    }
}

// PEBBLE EVENT LISTENERS //////////////////////////////////////////////////////

Pebble.addEventListener("appmessage", function(e) {
    if (e.payload.getSessions) {
        // Get sessions for provided time
        getEventsForTime(e.payload.getSessions);
    } else if (e.payload.getEventInfo) {
        // Get info for provided id
        getEventInfo(e.payload.getEventInfo);
    }
})

Pebble.addEventListener("ready", function(e) {
    getSchedule();
});
