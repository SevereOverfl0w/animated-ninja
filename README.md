animated-ninja
==============

A set of programs utilizing blockchain.info's ticker API written as language evaluations.

#### Program Requirements ####
* Must grab a JSON result from [Blockchain's Ticker](https://blockchain.info/ticker)
* Must parse this JSON, this can be done via standard library, or external library. Parsing this JSON is interesting, it poses 2 sides of the coin. First you must parse the "unknown" by looping over each currency. You **could** do this manually, but it ruins the point.
* Following parsing, the program should store the information in an object/structure of some kind. Preferably as an array or equivalent.
* Output in format [Currency]: [Last/24h/etc. etc.]

#### Each program ####
Each program will be stored in it's own directory dictated by it's language. A Readme will exist in each directory which will give instructions for compiling and/or running.
