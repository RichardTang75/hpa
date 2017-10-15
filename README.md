# hpa

To build this, SDL (https://www.libsdl.org/download-2.0.php), SDL_Image (https://www.libsdl.org/projects/SDL_image/), and Boost/functional (http://www.boost.org/) are required. Lodepng(http://lodev.org/lodepng/) is also needed to build this, but is only used for debugging purposes.

This is an (as of yet) unfinished RTS that I created over the summer, aiming to build off of my term project. I also used this as a tool to learn C++. It implements terrain generation and one level of hierarchical pathfinding. The relevant parts for hierarchical pathfinding are in pathfinding.cpp and hierarchical_pathfind.cpp. Furthermore, to implement the map being displayed when this is run, I used SDL pixel access to create the "masks" made of a transparent surface with certain parts nontransparent. That function is the back_text surface processing.

Inspiration for hierarchical pathfinding came from https://webdocs.cs.ualberta.ca/~mmueller/ps/hpastar.pdf and https://harablog.files.wordpress.com/2009/01/haa.pdf. I implemented it by taking a standard 2d list and cutting it up into sections, then going through all the edges of each cluster to find the cheapest ways between each cluster. I stored each cluster-cluster-movement cost transition as a key in a map. Then, to utilize this information to pathfind, I created a function used A* to travel to the first node, then allowed to traverse the hierarchical map before exiting at a node and using A* to travel to the final destination.

The pixel access code came from http://sol.gfxile.net/gp/ch02.html and https://www.gamedev.net/forums/topic/358269-copying-pixels-to-an-sdl-surface/.
