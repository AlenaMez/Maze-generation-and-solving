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
    Position(int _x, int _y, int _d) : Point(_x, _y), d(_d) {}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {
        // in cell-based motion, the distance to the parent is always 1
        return d;
    }

    std::vector<PositionPtr> children(Position& goal)
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;
        int i, j, d;

        for (int incr = -1; incr<=2; incr++)
        {
            d=1;
            i = incr%2; // incr = (-1, 0, 1, 2) => i = (-1, 0, 1, 0)
            j = (incr-1)%2; //incr-1 = (-2, -1, 0, 1) => i = (0, -1, 0, 1)

            if (maze.isFree(x+i,y+j))
            {
                while (is_line(x+d*i, y+d*j) &&
                       (x+d*i != goal.x || y+d*j != goal.y))
                {
                    d++;
                }

                generated.push_back(PositionPtr(new Position(x+d*i, y+d*j, d)));
            }
        }

        return generated;
    }

    bool is_line (int _x, int _y) //_x, _y are coordinates of a child
    {
        int x_incr(0), y_incr(0);
        if(x - _x)
        {
            x_incr = x - _x < 0 ? 1 : -1;
            if (maze.isFree(x+x_incr*abs(x-_x)+x_incr, _y)&&
                not(maze.isFree(x+x_incr*abs(x-_x), _y+1))&&
                not(maze.isFree(x+x_incr*abs(x-_x), _y-1)))
            {
                return true;
            }
        }

        else
        {
            y_incr = y - _y < 0 ? 1 : -1;
            if (maze.isFree(_x, y+y_incr*abs(y-_y)+y_incr)&&
            not(maze.isFree(_x+1, y+y_incr*abs(y-_y)))&&
            not(maze.isFree(_x-1, y+y_incr*abs(y-_y))) )
                {
                    return true;
                }
        }

        return false;

    }



private:
    int d;
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
    Position::maze.saveSolution("line");
    cv::waitKey(0);

}
