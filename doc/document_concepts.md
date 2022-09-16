# document concept

The concept of a "document" in the context of libimgdoc2 is discussed here.

## DocumentType

At the top level, the concept is that a document (represented by the IDoc-interface) is of a certain type,
as represented by the enum DocumentType. General speaking, different types of documents can have different 
ways of interacting with them and be different in their concepts. They are therefore represented
by different interfaces.

As a general rule, interfaces for read-only access and write access are strictly separated.

For the time being, two document types are implemented:
* Image2d
* Image3d

## Image2D

As far as the persistence layer is concerned, the functionality provided here is:

* storage of blob data (containing the pixel information of a tile)
* a tile is a rectangular image, and it is described by information describing its bitmap-content 
   (width/height/pixeltype), its logical position, its pyramid-level and a _coordinate_
* the document can be queried for tiles with certain conditions (including spatial queries)

The term coordinate refers to the following concept: a document can have an arbitrary number of dimensions
associated with a coordinate, and a tile is associated with a specific value for those dimensions.
The dimensions are to be chosen so that grouping the tiles can be done as appropriate.
