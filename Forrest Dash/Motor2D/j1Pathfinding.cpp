#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "Brofiler/Brofiler.h"
#include "J1Console.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}


bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");
	App->console->write("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}


void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width * height];
	memcpy(map, data, width * height);
}


bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}


bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}


uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y * width) + pos.x];

	return INVALID_WALK_CODE;
}


const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}


p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}


PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}


uint PathNode::FindWalkableAdjacents(PathList& list_to_fill)
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}


int PathNode::Score() const
{
	return g + h;
}


int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}


int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("Pathfinding", Profiler::Color::Gold);

	// TODO 1: if origin or destination are not walkable, return -1
	if (IsWalkable(origin) == false || IsWalkable(destination) == false) {
		return -1;
	}
	// TODO 2: Create two lists: open, close

	last_path.Clear();

	PathList open; PathList closed;
	// Add the origin tile to open
	open.list.add(PathNode(0, origin.DistanceTo(destination), origin, NULL));
	// Iterate while we have tile in the open list

	PathNode* current_node;

	while (open.GetNodeLowestScore() != NULL)
	{
		current_node = new PathNode(open.GetNodeLowestScore()->data);
		// TODO 3: Move the lowest score cell from open list to the closed list
		closed.list.add(*current_node);
		open.list.del(open.Find(current_node->pos));
		// TODO 4: If we just added the destination, we are done!
		if (current_node->pos == destination) {

			PathNode* iterator = current_node;

			for (iterator; iterator->pos != origin; iterator = iterator->parent) {
				last_path.PushBack(iterator->pos);
			}
			last_path.PushBack(origin);

			last_path.Flip();
			return 0;

		}

		// TODO 5: Fill a list of all adjancent nodes
		PathList adjList;
		uint limit = current_node->FindWalkableAdjacents(adjList);
		// TODO 6: Iterate adjancent nodes:
		for (uint i = 0; i < limit; i++) {
			
			if ((closed.Find(adjList.list[i].pos)) == NULL) {
				
				if ((open.Find(adjList.list[i].pos)) == NULL) {
					adjList.list[i].CalculateF(destination);
					open.list.add(adjList.list[i]);
				}
				else { 
					adjList.list[i].CalculateF(destination);
					if (adjList.list[i].g < open.Find(adjList.list[i].pos)->data.g) {
						
						open.list.del(open.Find(adjList.list[i].pos));
						open.list.add(adjList.list[i]);

					}
				}
			}
		}


	}

}