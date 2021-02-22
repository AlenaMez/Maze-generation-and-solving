#include <a_star.h>
#include <maze.h>

using namespace std;
using namespace ecn;

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;

public:
    // constructor from coordinates
    Position(int _x, int _y, int _d) : Point(_x, _y), distance(_d) {}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {
        // in cell-based motion, the distance to the parent is always 1
        return distance;
    }

    std::vector<PositionPtr> children(Position& goal)
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;
        int d, i, j;

        for (int incr = -1; incr<=2; incr++)
        {
            d = 1;
            i = incr%2; // incr = (-1, 0, 1, 2) => i = (-1, 0, 1, 0)
            j = (incr-1)%2; //incr-1 = (-2, -1, 0, 1) => j = (0, -1, 0, 1)

            if (maze.isFree(x+i,y+j))
            {
                int next_x = x+i;
                int next_y = y+j;
                int next_incr = incr;
                while ( is_corridor(next_x, next_y, next_incr) &&
                        (next_x != goal.x || next_y != goal.y))
                {                 
                    d++;
                    next_x += next_incr%2;
                    next_y += (next_incr-1)%2;
                }

                //check if a child that we found does not exist, or if it does, then use the smallest distance
                bool is_new_child = true;
                for (auto& child : generated)
                {
                    if (next_x == child->x && next_y == child->y)
                    {
                        is_new_child = false;
                        if (d < child->distance)
                            child->distance = d;
                    }
                }
                if (is_new_child)
                    generated.push_back(PositionPtr(new Position(next_x, next_y, d)));
            }
        }

        return generated;
    }

    bool is_corridor (int _x, int _y, int& _last_incr) //_x, _y are coordinates of a child
    {
        int last_i = _last_incr%2;
        int last_j = (_last_incr-1)%2;

        int i, j;
        std::list<int> next_incrs;

        for (int incr = -1; incr<=2; incr++)
        {
            i = incr%2;     //  incr = (-1,  0, 1, 2) => i = (-1, 0, 1, 0)
            j = (incr-1)%2; //incr-1 = (-2, -1, 0, 1) => j = (0, -1, 0, 1)

            if ( (i != -last_i || j != -last_j) && // Check it is not where we come from
                 maze.isFree(_x+i, _y+j))
            {
                next_incrs.push_back(incr); //remember the incr, that is responsible for the direction that is free and not where we come from
            }
        }
        if (next_incrs.size() == 1) //if only one direction is found
        {
            _last_incr = next_incrs.front(); //change the last incr to the incr that is next to go
            return true;
        }
        return false;
    }


    // final print
    // maze deals with the color, just tell the points
    void print(const Point &parent) override
    {
        int i, j, d_current;
        bool is_right = false; //is_right - is the pasth in this direction is the one with the minimum distance

        for (int incr = -1; incr<=2; incr++)
        {
            d_current=1;
            i = incr%2; // incr = (-1, 0, 1, 2) => i = (-1, 0, 1, 0)
            j = (incr-1)%2; //incr-1 = (-2, -1, 0, 1) => j = (0, -1, 0, 1)

            if (maze.isFree(x+i,y+j)) //take one over four directions
            {                
                int next_x = x+i;
                int next_y = y+j;
                int next_incr = incr;
                while (is_corridor(next_x, next_y, next_incr) ||
                       (next_x==parent.x && next_y==parent.y))
                {

                    if (d_current>distance) // Compare if it is bigger than the child has
                    {
                        break; // Going to far away
                    }
                    else if (d_current == distance &&
                             (next_x==parent.x && next_y==parent.y)) // it is right path, if distance is the same as needed and we are in the parent cell
                    {
                        is_right = true;
                        break;
                    }

                    d_current++;
                    next_x += next_incr%2;
                    next_y += (next_incr-1)%2;
                }
            }

            if (is_right)
            {
                maze.passThrough(x, y);
                int next_x = x+i;
                int next_y = y+j;
                int next_incr = incr;
                while (is_corridor(next_x, next_y, next_incr) &&
                       (next_x != parent.x || next_y != parent.y))
                {
                    maze.passThrough(next_x, next_y);
                    next_x += next_incr%2;
                    next_y += (next_incr-1)%2;
                }
                maze.passThrough(next_x, next_y);
                break;
            }
        }
    }

private:
    int distance;
};



int main( int argc, char **argv )
{
    // load file
    std::string filename = "maze.png";
    if(argc == 2)
        filename = std::string(argv[1]);

    // let Point know about this maze
    Position::maze.load(filename);

    // initial and goal positions as Position's
    Position start = Position::maze.start(),
             goal = Position::maze.end();

    // call A* algorithm
    ecn::Astar(start, goal);

    // save final image
    Position::maze.saveSolution("corridor");
    cv::waitKey(0);

}
