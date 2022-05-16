// HTML style attributes
const rootStyle = getComputedStyle(document.documentElement);

// Retrieve color values from stylesheet
const colorEmpty   = rootStyle.getPropertyValue('--col-empty');
const colorAbsent  = rootStyle.getPropertyValue('--col-absent');
const colorPresent = rootStyle.getPropertyValue('--col-present');
const colorCorrect = rootStyle.getPropertyValue('--col-correct');
const colorActive  = rootStyle.getPropertyValue('--col-border-active');
const colorBorder  = rootStyle.getPropertyValue('--col-border');

var tiles;                // Array of all tile elements
var activeTileIndex = 0;  // The index of the currently active tile

var validWords = [];      // List of all valid words
var showAllWords = false; // State of "valid words" display

// Set of characters that can occupy tiles
const allowedCharacters = new Set([
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 
    'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
]);


function updateValidWordsDisplay() {
    // Reset display
    $("#valid-words").html("");

    // Update display depending on state
    if (showAllWords) {
        validWords.forEach((word) => {
            $("#valid-words").append(word + " ");
        });
    } else {
        for (i = 0; i < Math.min(5, validWords.length - 1); i++) {
            $("#valid-words").append("<li>" + validWords[i] + "</li>");
        }
    }
}

function changeDisplayState(event) {
    if (showAllWords) {
        $("#btn-results").html("Top 5");
    } else {
        $("#btn-results").html("All Words");
    }

    showAllWords = !showAllWords;

    updateValidWordsDisplay();
}

/* Update data string for backend */
function update() {
    // Data sent to backend
    let dataString = "";

    // Update data for each tile
    for (i = 0; i < tiles.length; i++) {
        
        let c = tiles[i].innerHTML;
        let state = tiles[i].dataset.state;

        if (c == '')
            dataString += '-';
        else
            dataString += c;

        if (state == 'present')
            dataString += '?';
        else if (state == 'correct')
            dataString += '!';

        // Separate words by spaces
        if ((i + 1) % 5 == 0 && i < tiles.length - 1)
            dataString += ' ';
    }

    // Append newline character
    dataString += '\n';

    // Send data to cgi script
    $.post(
        "/cgi-bin/script.cgi",
        dataString,
        (results) => { 
            validWords = results.split(" "); 
            updateValidWordsDisplay();
        },
        "text"
    );

}

/* Return the tile that will receive the next keyboard input */
function activeTile() {
    return tiles[activeTileIndex];
}

/* Sets the currently active tile and highlights it */
function setActiveTile(index) {

    // Check that index is in bounds
    if (index >= tiles.length)
        index = tiles.length - 1;
    else if (index < 0)
        index = 0;

    // Remove highlight from previously active tile
    activeTile().style.borderColor = colorBorder;
    activeTile().style.borderWidth = '1px';
    activeTile().style.margin = '1px';
    
    // Update active tile index
    activeTileIndex = index;
    
    // Highlight the currently active tile
    activeTile().style.borderColor = colorActive;
    activeTile().style.borderWidth = '2px';
    activeTile().style.margin = '0px 0px';

}

/* Update the active tile when a key is pressed */
document.addEventListener('keydown', (event) => {
    
    // Cycle through currently active tile state on spacebar press
    if (event.code == 'Space') {
        changeState(activeTile());
    }

    // Delete a character on backspace
    else if (event.code == 'Backspace') {
        // If active tile contains a character
        if (activeTile().innerHTML != '') { 
            // Delete character
            activeTile().innerHTML = '';
            activeTile().style.backgroundColor = colorEmpty;
            activeTile().dataset.state = 'absent';

            update();
        }
        // If active tile does not contain a character
        else {
            // And preceding tile contains a character
            if (tiles[Math.max(0, activeTileIndex - 1)].innerHTML != '') {
                // Delete preceding tile's character
                setActiveTile(activeTileIndex - 1);
                activeTile().innerHTML = '';
                activeTile().style.backgroundColor = colorEmpty;
                activeTile().dataset.state = 'absent';

                update();
            }
        }
    }

    // If a character key was pressed
    else if (allowedCharacters.has(event.key.toLowerCase())) {
        // Do nothing if character doesn't change
        if(activeTile().innerHTML == event.key) return;

        // Update the tile with the pressed key
        activeTile().innerHTML = event.key;
        
        // Get background color of tile as string
        let bg = getComputedStyle(activeTile())
            .getPropertyValue('background-color');

        // If background color is empty, update to absent
        if (bg.trim() != colorPresent.trim() 
        &&  bg.trim() != colorCorrect.trim())
            activeTile().style.backgroundColor = colorAbsent;

        // Advance active letter
        setActiveTile(activeTileIndex + 1);

        update();
    }

    // If a navigation key was pressed
    else if (event.key.includes('Arrow')) {
        direction = event.key.substring(5).toLowerCase();
        if (direction == 'left')  setActiveTile(activeTileIndex - 1);
        if (direction == 'right') setActiveTile(activeTileIndex + 1);
        if (direction == 'up')    setActiveTile(activeTileIndex - 5);
        if (direction == 'down')  setActiveTile(activeTileIndex + 5);
    }
});


/* Cycle through the states of a tile */
function changeState(tile) {
    
    // Make this tile the active tile
    setActiveTile(tiles.indexOf(tile));

    // Do nothing to state if tile does not contain a character
    if (tile.innerHTML == '') return;

    // Get the current tile state
    let state = tile.dataset.state;

    // Cycle through states
    if (state == 'absent') {
        tile.dataset.state = 'present';
        tile.style.backgroundColor = colorPresent;
    } else if (state == 'present') {
        tile.dataset.state = 'correct';
        tile.style.backgroundColor = colorCorrect;
    } else if (state == 'correct') {
        tile.dataset.state = 'absent';
        tile.style.backgroundColor = colorAbsent;
    }

    update();
}

/* Cycle through the states of a tile */
function changeStateScroll(tile, deltaY) {

    // Do nothing if tile does not contain a character
    if (tile.innerHTML == '') return;

    // Get the current tile state
    let state = tile.dataset.state;

    // If scrolled down, cycle in reverse
    if (deltaY < 0) {
        if (state == 'absent') {
            tile.dataset.state = 'correct';
            tile.style.backgroundColor = colorCorrect;
        } else if (state == 'present') {
            tile.dataset.state = 'absent';
            tile.style.backgroundColor = colorAbsent;
        } else if (state == 'correct') {
            tile.dataset.state = 'present';
            tile.style.backgroundColor = colorPresent;
        }
        return;
    // If scrolled up, cycle normally
    } else {
        if (state == 'absent') {
            tile.dataset.state = 'present';
            tile.style.backgroundColor = colorPresent;
        } else if (state == 'present') {
            tile.dataset.state = 'correct';
            tile.style.backgroundColor = colorCorrect;
        } else if (state == 'correct') {
            tile.dataset.state = 'absent';
            tile.style.backgroundColor = colorAbsent;
        }
    }

    update();
}

/* Handle the different interactions with tiles */
function handleTileEvent(event) {
    if (event.type == 'click') {
        changeState(event.target);
        $("#mobile-keyboard").focus(); // Bring up mobile keyboard
    } else if (event.type == 'wheel') {
        changeStateScroll(event.target, event.deltaY);
    }
}

/* Get tiles and set behaviors once page has loaded */
$(function(){
    // Retrieve list of tiles
    tiles = Array.from(document.getElementsByClassName('tile'));
    setActiveTile(activeTileIndex); // Set active tile

    // Assign tile behaviors
    tiles.forEach((item, index) => {
        item.addEventListener('click', handleTileEvent);
        item.addEventListener('wheel', handleTileEvent);
    });

    $("#btn-results").on('click', changeDisplayState);
});
