#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <point.h>
#include <maze.h>

typedef struct
{
    int x, y; //Node position - little waste of memory, but it allows faster generation
    void *parent; //Pointer to parent node
    char c; //Character to be displayed
    char dirs; //Directions that still haven't been explored
} Node;

Node *nodes; //Nodes array
int width, height; //Maze dimensions
int num_of_walls = 0;

int init( )
{
    int i, j;
    Node *n;

    //Allocate memory for maze
    nodes = (Node*) calloc( (width*height), sizeof(Node) );
    if ( nodes == NULL ) return 1;

    //Setup crucial nodes
    for ( i = 0; i < width; i++ )
    {
        for ( j = 0; j < height; j++ )
        {
            n = nodes + i + j * width;
            if ( i * j % 2 )
            {
                n->x = i;
                n->y = j;
                n->dirs = 15; //Assume that all directions can be explored (4 youngest bits set)
                n->c = ' ';
            }
            else
            {
                n->c = '#'; //Add walls between nodes
                num_of_walls++;
            }
        }
    }
    return 0;
}

Node *link( Node *n )
{
    //Connects node to random neighbor (if possible) and returns
    //address of next node that should be visited

    int x, y;
    char dir;
    Node *dest;

    //Nothing can be done if null pointer is given - return
    if ( n == NULL ) return NULL;

    //While there are directions still unexplored
    while ( n->dirs )
    {
        //Randomly pick one direction
        dir = ( 1 << ( rand() % 4 ) );

        //If it has already been explored - try again
        if ( ~n->dirs & dir ) continue;

        //Mark direction as explored
        n->dirs &= ~dir;

        //Depending on chosen direction
        switch ( dir )
        {
            //Check if it's possible to go right
            case 1:
                if ( n->x + 2 < width )
                {
                    x = n->x + 2;
                    y = n->y;
                }
                else continue;
                break;

            //Check if it's possible to go down
            case 2:
                if ( n->y + 2 < height )
                {
                    x = n->x;
                    y = n->y + 2;
                }
                else continue;
                break;

            //Check if it's possible to go left
            case 4:
                if ( n->x - 2 >= 0 )
                {
                    x = n->x - 2;
                    y = n->y;
                }
                else continue;
                break;

            //Check if it's possible to go up
            case 8:
                if ( n->y - 2 >= 0 )
                {
                    x = n->x;
                    y = n->y - 2;
                }
                else continue;
                break;
        }

        //Get destination node into pointer (makes things a tiny bit faster)
        dest = nodes + x + y * width;

        //Make sure that destination node is not a wall
        if ( dest->c == ' ' )
        {
            //If destination is a linked node already - abort
            if ( dest->parent != NULL ) continue;

            //Otherwise, adopt node
            dest->parent = n;

            //Remove wall between nodes
            nodes[n->x + ( x - n->x ) / 2 + ( n->y + ( y - n->y ) / 2 ) * width].c = ' ';

            //Return address of the child node
            return dest;
        }
    }

    //If nothing more can be done here - return this node//parent's address
    return (Node*) n->parent;
}

void non_perfect_maze(const int&percent_to_remove)
{
   int walls_inside = num_of_walls - (2*width) - (2*height);
   int num_to_remove = percent_to_remove*walls_inside/100;
   int i, j ; //row and column
   while (num_to_remove != 0&&walls_inside!=0)
   {
       int remove = rand()%(width*(height-2))+width;
       i = remove/width; //row
       j = remove%width; //column
       auto symbol = nodes[remove].c;
       if ((j!=0 && j!= (width-1))&&(symbol ==  '#')) {
           nodes[remove].c = ' ';
           num_to_remove--;
           num_of_walls--;
           walls_inside--;
       }
   }

   if (walls_inside == 0)
       std::cout << "no walls in the maze" << std::endl;

}
void generate_maze_image(ecn::Maze* Maze, const int& height, const int& width)
{
    int i, j;

    for ( i = 0; i < height; i++ )
    {
        for ( j = 0; j < width; j++ )
        {
            auto symbol = nodes[j + i * width].c;
            if (symbol !=  '#') {
                Maze->dig(j, i);
            }

        }

    }
    std::cout << "Maze image is saved" << std::endl;
    Maze->save();
}
void generate_maze(int height, int width, int percent_to_remove)
{
    Node *start;
    Node *last;


    //Read maze dimensions from command line arguments
    if ( width + height < 2 )
    {
        std::cout << "invalid maze size value!" << std::endl;
        exit( 1 );
    }

    //Allow only odd dimensions
    if ( !( width % 2 ) || !( height % 2 ) )
    {
        std::cout << "dimensions must be odd!" << std::endl;
        exit( 1 );
    }

    //Do not allow negative dimensions
    if ( width <= 0 || height <= 0 )
    {
        std::cout << "imensions must be greater than 0!" << std::endl;
        exit( 1 );
    }

    //Seed random generator
    srand( time( NULL ) );

    //Initialize mazes
    if ( init() )
    {
        std::cout << "Out of memory" << std::endl;
        exit( 1 );
    }

    //Setup start node
    start = nodes + 1 + width;
    start->parent = start;
    last = start;

    //Connect nodes until start node is reached and can't be left
    while ( ( last = link( last ) ) != start );

    if (percent_to_remove!=0)
        non_perfect_maze(percent_to_remove);

    std::cout << "Maze is generated" << std::endl;

}

int main()
{
    width = 501;
    height = 251;
    ecn::Maze my_maze(height, width);
    generate_maze(height, width, 60);
    generate_maze_image(&my_maze, height, width);
}
