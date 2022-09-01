# database structure

In database we a dealing with the following tables:

| Name | purpose | description
| - | - |
| GENERAL | contains configuration | Here we gather configuration and global data (affecting the operation). This table holds a simple key-value property-bag.
| TILESINFO | contains "logical information" about tiles | In this table we have the "logical position" of the tiles, and their coordinates.
| TILESDATA | contains "physical information" about tiles | Here we list "physical information" about the tiles, and have a link to the actual pixeldata.
| BLOBS | store binary blobs | This table contains binary blobs.


## The TILESINFO table



